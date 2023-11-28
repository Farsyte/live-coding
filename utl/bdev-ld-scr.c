#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "simext/hex.h"
#include "simext/mapdrive.h"

// bdev-ld-str: load a text file as FORTH screens
//
// USAGE:
//      bdev-ld-str DRV < $scrfile
//
// DESCRIPTION:
//      The bdev-ld-str command reads newline-delimited text from
//      standard input and converts it to FORTH SCREEN data.
//
//      FORTH divides the drive up into fixed size screens, each
//      containing sixteen lines of sixty four characters.
//
//      Screen Zero starts in the first sector of the SECOND track, to
//      allow us to keep a bootable 7 KiB image of FORTH on the disk
//      while also still having data in Screen 4.
//
//      The first line of the input data contains the destination
//      screen number as decimal digits, followed by a newline.
//
//      Incoming data is copied byte-by-byte from to the active drive
//      memory, with some processing.
//
//      FORM-FEED (Control-L) advances the output to the next multiple
//      of the screen length, which is configured to 16 lines.
//
//      NEWLINE (Control-J) advances the output to the next multiple
//      of the line length, which is configured to 64 bytes. As a
//      special case, a newline immediately after a form-feed is
//      simply ignored.
//
//      TAB (Control-I) advances the output to the next multiple of
//      the tab stop size, which is configured to 8 bytes.
//
//      Before writing data to a screen, the entire screen is set to
//      the ASCII SPACE character.

#define LINE_BYTES 	64      /* bytes per line */
#define SCR_LINES 	16      /* lines per screen */
#define SCR_BYTES 	(LINE_BYTES*SCR_LINES)  /* bytes per screen */

#define	TAB_WIDTH 	8

#define	SEC_BYTES	128
#define TRK_SECTS	52
#define	TRK_BYTES	(TRK_SECTS*SEC_BYTES)

int                 drive_number;
pDriveData          drive_meta;
int                 drive_data_offset;
int                 drive_data_length;

pByte               write_data_base;
unsigned            screen_count;

#define BMAX        512
char                _lbuf[BMAX];
int                 llen = 0;

unsigned            scr = 0;
unsigned            line = 0;

unsigned            offset_hwm = 0;

Cstr                prog = 0;

int                 debug = 0;
int                 verbose = 0;
int                 dryrun = 0;

Cstr                input_fn = 0;
int                 input_ln = 0;
FILE               *input_fp = 0;
Cstr                index_fn = "-";
Cstr                gloss_fn = "-";
FILE               *index_fp = 0;               /* todo open index file in append mode */
FILE               *gloss_fp = 0;               /* todo open gloss file in append mode */

static int          lbuf_get(int ix);
static void         lbuf_set(int ix, int ch);
static int          nextline();
static void         seekpage();
static void         emitline();
static void         maybeindex();
static void         maybegloss();

#define DBG_D(var) if (debug) STUB("%16s = %lu", #var, (unsigned long)(var))
#define DBG_S(var) if (debug) STUB("%16s = '%s'", #var, (Cstr)(var))

int main(int argc, Cstr *argv)
{
    Cstr                argp;
    char               *arge;

    assert(argc > 1);
    prog = *argv++;
    assert(NULL != prog);
    assert('\0' != prog[0]);

    index_fp = stderr;  /* todo open index file in append mode */
    gloss_fp = stderr;  /* todo open gloss file in append mode */

    while ((NULL != argv[0]) && (argv[0][0] == '-')) {
        argp = *argv++;
        if (argp[1] != '-') {
            while (*++argp)
                switch (*argp) {
                  case 'd':
                      ++debug;
                      break;
                  case 'v':
                      ++verbose;
                      break;
                  case 'n':
                      ++dryrun;
                      break;
                  case 'i':
                      index_fn = *argv++;
                      assert(NULL != index_fn);
                      assert('\0' != index_fn[0]);
                      DBG_S(index_fn);
                      index_fp = fopen(index_fn, "w");
                      assert(NULL != index_fp);
                      break;
                  case 'g':
                      gloss_fn = *argv++;
                      assert(NULL != gloss_fn);
                      assert('\0' != gloss_fn[0]);
                      DBG_S(gloss_fn);
                      gloss_fp = fopen(gloss_fn, "w");
                      assert(NULL != gloss_fp);
                      break;
                }
        } else if (!strcmp("--verbose", argp)) {
            ++verbose;
        } else if (!strcmp("--debug", argp)) {
            ++debug;
        } else if (!strcmp("--dryrun", argp)) {
            ++dryrun;
        }
    }

    ASSERT(0 == dryrun, "dryrun not yet implemented.");

    argp = *argv++;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    drive_number = strtol(argp, &arge, 0);
    DBG_D(drive_number);
    assert(argp < arge);
    assert('\0' == *arge);

    drive_meta = map_drive(drive_number);
    ASSERT(NULL != drive_meta,
           "Unable to access media in drive %c\n"
           "map_drive indicates error %d: %s\n", 'A' + drive_number, errno, strerror(errno));

    // screen zero is at the start of the SECOND track.
    drive_data_offset = drive_meta->data_offset + TRK_BYTES;
    DBG_D(drive_data_offset);
    drive_data_length = drive_meta->data_length - TRK_BYTES;
    DBG_D(drive_data_length);
    write_data_base = (pByte)drive_meta + drive_data_offset;

    screen_count = drive_data_length / SCR_BYTES;
    DBG_D(screen_count);

    for (;;) {

        input_fn = *argv++;
        if (NULL == input_fn)
            break;
        if ('\0' == input_fn[0])
            break;
        input_fp = fopen(input_fn, "r");
        ASSERT(NULL != input_fp,
               "unable to open %s\nerror %d: %s", input_fn, errno, strerror(errno));
        input_ln = 0;

        if (verbose)
            fprintf(stderr, "%s: reading %s\n", prog, input_fn);

        offset_hwm = 0;

        while (nextline()) {
            if (lbuf_get(0) == '\f') {
                seekpage();
            } else {
                emitline();
            }
        }
    }

    DBG_D(screen_count);
    free_drive(drive_meta);

    if (verbose)
        fprintf(stderr, "%s: done\n", prog);

    return 0;
}

static int lbuf_get(int ix)
{
    if ((ix >= 0) && (ix < BMAX))
        return _lbuf[ix];
    return 0;
}

static void lbuf_set(int ix, int ch)
{
    if ((ix >= 0) && (ix < BMAX))
        _lbuf[ix] = ch;
}

static int nextline()
{
    int                 pp = ' ';
    int                 pc = ' ';
    int                 ch = ' ';
    int                 skiptoeol = 0;

    for (llen = 0; llen < BMAX; ++llen)
        lbuf_set(llen, '\0');
    llen = 0;
    input_ln++;
    for (;;) {
        pp = pc;
        pc = ch;
        ch = fgetc(input_fp);
        if (ch == EOF) {
            return llen > 0;
        } else if ((ch == '/') && (pc == '/') && (pp == ' ')) {
            if (debug)
                fprintf(stderr, "comment noted at scr %d line %d llen %d\n", scr, line, llen);
            lbuf_set(--llen, '\0');
            skiptoeol = 1;
        } else if (ch == '\n') {
            while ((llen > 0) && ((lbuf_get(llen - 1) == ' ') || (lbuf_get(llen - 1) == 0)))
                lbuf_set(--llen, '\0');
            if (llen > 0) {
                ASSERT(llen <= 64, "line too long\n%s:%d: %-*.*s...",
                       input_fn, input_ln, llen, llen, _lbuf);
                return 1;
            }
            skiptoeol = 0;
            pp = ' ';
            pc = ' ';
            ch = ' ';
            input_ln++;
        } else if (skiptoeol) {
            ;
        } else if (ch == '\t') {
            do {
                lbuf_set(llen++, ' ');
            } while (0 != (llen % TAB_WIDTH));
        } else {
            lbuf_set(llen++, ch);
        }
    }
}

static void seekpage()
{
    if (debug)
        STUB("scr was %d", scr);
    // we get here if lbuf_get(0) is form-feed.
    if (lbuf_get(1) == '\0') {
        scr++;
        DBG_D(scr);
    } else {
        scr = 0;
        for (int i = 1; i < llen; ++i) {
            int                 ch = lbuf_get(i);
            ASSERT(('0' <= ch) && (ch <= '9'), "non-digit after form-feed");
            scr = scr * 10 + ch - '0';
        }
        DBG_D(scr);
    }
    DBG_D(screen_count);
    ASSERT(scr < screen_count,
           "screen number %d invalid: drive capacity is %d screens", scr, screen_count);
    line = 0;

    unsigned offset = scr * SCR_BYTES + line * LINE_BYTES;

    ASSERT(offset_hwm <= offset,
           "attempt to rewind output cursor detected\n%s:%d: %-*.*s...",
               input_fn, input_ln, llen, llen, _lbuf);
    offset_hwm = offset;
}

static void emitline()
{
    unsigned offset = scr * SCR_BYTES + line * LINE_BYTES;

    ASSERT(offset_hwm <= offset,
           "attempt to rewind output cursor detected\n%s:%d: %-*.*s...",
               input_fn, input_ln, llen, llen, _lbuf);

    offset_hwm = offset + LINE_BYTES;

    pByte               op = write_data_base + offset;
    memset(op, ' ', line ? LINE_BYTES : SCR_BYTES);


    while ((llen > 0) && ((' ' == lbuf_get(llen - 1)) || ('\0' == lbuf_get(llen - 1))))
        lbuf_set(--llen, '\0');
    lbuf_set(llen, '\0');

    if (llen > 0) {
        ASSERT(llen <= 64, "line too long\n%s:%d: %-*.*s...",
               input_fn, input_ln, llen, llen, _lbuf);
        for (int i = 0; i < llen; ++i)
            op[i] = lbuf_get(i);
        maybeindex();
        maybegloss();
    }

    line++;
    if (line < 16)
        return;

    scr ++;
    DBG_D(scr);
    DBG_D(screen_count);
    ASSERT(scr < screen_count,
           "screen number %d invalid: drive capacity is %d screens", scr, screen_count);
    line = 0;
}

static void maybeindex()
{
    if (NULL == index_fp)
        return;
    DBG_D(line);
    if (line != 0)
        return;
    DBG_D(llen);

    // index is all first lines that are not blank.
    if (llen < 1)
        return;
    fprintf(index_fp, "%3d    ", scr);
    for (int i = 0; i < llen; ++i)
        fprintf(index_fp, "%c", lbuf_get(i));
    fprintf(index_fp, "\n");
}

static void maybegloss()
{
    if (NULL == gloss_fp)
        return;

    // original gloss form:
    //   lines ending with *)
    // revised gloss form:
    //   any line containing \*

    if ((lbuf_get(llen - 1) == ')') && (lbuf_get(llen - 2) == '*')) {
        fprintf(gloss_fp, "%3d %2d ", scr, line);
        for (int i = 0; i < llen; ++i)
            fprintf(gloss_fp, "%c", lbuf_get(i));
        fprintf(gloss_fp, "\n");
    }
}
