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

#define	TAB_BYTES 	8

#define	SEC_BYTES	128
#define TRK_SECTS	52
#define	TRK_BYTES	(TRK_SECTS*SEC_BYTES)

int main(int argc, Cstr *argv)
{
    Cstr                argp;
    char               *arge;
    int                 drv;
    pDriveData          drive;
    int                 doff;
    int                 dlen;
    int                 escr;
    int                 targ;
    int                 wrto;
    int                 last;
    pByte               wbase;
    char                pch;
    char                ch;
    int                 scr;
    int                 cscr;

    assert(argc > 1);

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    drv = strtol(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);

    drive = map_drive(drv);
    ASSERT(NULL != drive,
           "Unable to access media in drive %c\n"
           "map_drive indicates error %d: %s\n", 'A' + drv, errno, strerror(errno));

    // screen zero is at the start of the SECOND track.
    doff = drive->data_offset + TRK_BYTES;
    dlen = drive->data_length - TRK_BYTES;
    wbase = (pByte)drive + doff;

    escr = dlen / SCR_BYTES;

    cscr = -1;

    scr = 0;
    for (;;) {
        ch = getchar();
        ASSERT(ch != EOF, "EOF while reading screen number");
        if (ch == '\n')
            break;
        if ((scr == 0) && (ch == '\f'))
            continue;
        ASSERT(('0' <= ch) && (ch <= '9'), "first line must have only the screen number");
        scr = scr * 10 + ch - '0';
    }
    ASSERT(scr < escr, "screen number %d invalid: drive capacity is %d screens", scr, escr);
    wrto = scr * SCR_BYTES;
    scr = 0;

    pch = -1;
    last = 0;
    while (EOF != (ch = getchar())) {

        if (pch == '\f') {
            if (ch == '\n') {
                if (scr != 0) {
                    targ = scr * SCR_BYTES;
                    ASSERT(last <= targ,
                           "oops, trying to seek backwards by %d bytes to screen %d",
                           last - targ, scr);
                    wrto = targ;
                }
                pch = ch;
                scr = 0;
                continue;
            }
            if (('0' <= ch) && (ch <= '9')) {
                scr = scr * 10 + ch - '0';
                continue;
            }
        }

        switch (ch) {

          case '\t':

              // tab: move to the next tab stop.

              wrto = wrto + TAB_BYTES - (wrto % TAB_BYTES);
              break;

          case '\f':

              // form-feed: End this screen. may be followed
              // by a screen number and a newline. If followed
              // by just a newline, moves to next screen.

              wrto = wrto + SCR_BYTES - (wrto % SCR_BYTES);
              scr = 0;
              break;

          case '\n':

              // newlines are ignored if we are exactly at
              // the start of a screen.

              if (0 == (wrto % SCR_BYTES))
                  break;

              // newlines are ignored at the start of a line
              // if we wrapped around, which is true if the
              // previous character was not a newline.

              if ((0 == (wrto % LINE_BYTES)) && (pch != '\n'))
                  break;

              wrto = wrto + LINE_BYTES - (wrto % LINE_BYTES);
              break;

          default:

              // Nearly everything else: put it on the FORTH screen.

              ASSERT((ch >= ' ') && (ch <= '~'),
                     "input file has unexpected character 0x%02X", ch);
              if (cscr != (wrto / SCR_BYTES)) {
                  // clear each screen before we write to it.
                  cscr = wrto / SCR_BYTES;
                  ASSERT(cscr < escr, "Attempting to write screen %d on a %d-screen drive",
                         cscr, escr);
                  memset(wbase + cscr * SCR_BYTES, ' ', SCR_BYTES);
                  // STUB("precleared screen %d (will write line %d col %d)",
                  //      cscr, (wrto / 64) % 16, wrto % 64);
              }

              last = wrto;
              wbase[wrto++] = ch;
              break;
        }
        pch = ch;
    }

    free_drive(drive);
    return 0;
}
