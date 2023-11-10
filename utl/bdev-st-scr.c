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

// bdev-st-str: extract FORTH screens to text
//
// USAGE:
//      bdev-st-str drv-no scr-min scr-max > $scrfile
//
// DESCRIPTION:
//      The bdev-sld command reads the FORTH screens present on the
//      currently mounte drive $drv-no, from $scr-min to $scr-max,
//      inclusive, and generates an output file in the form expected
//      by bdev-ld-str.
//
//      The output form has a form feed, followed by the screen number
//      in decimal, then each line of the screen. Trailing whitespace
//      is trimmed from each line, and trailing empty lines are
//      trimmed from each screen.

#define LINE_BYTES 	64      /* bytes per line */
#define SCR_LINES 	16      /* lines per screen */
#define SCR_BYTES 	(LINE_BYTES*SCR_LINES)  /* bytes per screen */

#define	SEC_BYTES	128
#define TRK_SECTS	52
#define	TRK_BYTES	(TRK_SECTS*SEC_BYTES)

int main(int argc, Cstr *argv)
{
    Cstr                argp;
    char               *arge;
    int                 drv;
    int                 scr_min;
    int                 scr_max;
    pDriveData          drive;
    int                 doff;
    int                 dlen;
    pByte               dbase;
    pByte               dstop;
    int                 scr;
    pByte               sbase;
    pByte               sstop;
    pByte               sscan;
    int                 ch;
    pByte               lnbcs;
    int                 nonascii;
    pByte               lbase;
    pByte               lstop;
    int                 llen;

    assert(argc > 1);

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    drv = strtol(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    scr_min = strtol(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    scr_max = strtol(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);

    drive = map_drive(drv);
    ASSERT(NULL != drive,
           "Unable to access media in drive %c\n"
           "map_drive indicates error %d: %s\n", 'A' + drv, errno, strerror(errno));

    // screen zero is at the start of the SECOND track.
    doff = drive->data_offset + TRK_BYTES;
    dlen = drive->data_length - TRK_BYTES;
    dbase = (pByte)drive + doff;
    dstop = dbase + dlen;

    for (scr = scr_min; scr <= scr_max; ++scr) {
        sbase = dbase + scr * SCR_BYTES;
        sstop = sbase + SCR_BYTES;
        ASSERT(sstop <= dstop, "screen %d is not within this disk storage.", scr);

        printf("\f%d\n", scr);

        // skip unwritten screens.
        if (!*sbase)
            continue;

        lnbcs = 0;
        nonascii = 0;
        for (sscan = sbase; sscan < sstop; sscan++) {
            ch = *sscan;
            if ((ch > ' ') && (ch <= '~'))
                lnbcs = sscan;
            else if (ch != ' ') {
                if (!nonascii) {
                    unsigned            bo = sscan - dbase;
                    fprintf(stderr, "Drive %d Screen %d line %d char %d has bad byte: 0x%02X\n",
                            drv, bo / 1024, (bo / 64) % 16, bo % 64, ch);
                }
                nonascii = 1;
            }
        }
        if (!lnbcs || nonascii)
            continue;

        for (int line = 0; line < SCR_LINES; ++line) {
            lbase = sbase + LINE_BYTES * line;
            if (lbase >= lnbcs)
                break;
            lstop = lbase + LINE_BYTES;
            while (lstop > lbase && ' ' == lstop[-1])
                --lstop;
            llen = lstop - lbase;
            printf("%-*.*s\n", llen, llen, lbase);
        }
    }

    free_drive(drive);
    return 0;
}
