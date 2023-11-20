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

// bdev-ld: load a drive image from a file
//
// USAGE:
//      bdev-ld DRV destination-file
//
// DESCRIPTION:
//      The bdev-ld command simulates the act of inserting a diskette
//      into a drive, by the simple expedient of copying the data area
//      from the named file into the data area of the drive file,
//      after assuring that the destination is large enough, and updating
//      the header.

int                 debug = 0;
int                 verbose = 0;
int                 dryrun = 0;

int main(int argc, Cstr *argv)
{
    Cstr                program;
    Cstr                argp;
    char               *arge;
    int                 drv;
    pDriveData          drive;
    Cstr                basename;
    Cstr                imagepath;
    int                 filed;
    int                 doff;
    int                 dlen;
    pByte               dst;
    struct sDriveData   sdd;

    assert(argc > 2);

    program = *argv++;

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
    drv = strtol(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);
    if (debug)
        STUB("drv = %d", drv);

    drive = map_drive(drv);
    ASSERT(NULL != drive,
           "Unable to access media in drive %c\n"
           "map_drive indicates error %d: %s\n", 'A' + drv, errno, strerror(errno));

    argp = *argv++;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    basename = argp;
    if (debug)
        STUB("basename = '%s'", basename);

    imagepath = drive_image_path(basename);
    if (debug)
        STUB("imagepath = '%s'", imagepath);

    filed = open(imagepath, O_RDONLY);

    ASSERT(0 <= filed,
           "Unable to access offline storage '%s'\n"
           "  error %d: %s\n", imagepath, errno, strerror(errno));

    doff = drive->data_offset;
    if (debug)
        STUB("doff = %lu", (unsigned long)doff);
    dlen = drive->data_length;
    if (debug)
        STUB("dlen = %lu", (unsigned long)dlen);

    FIN(hrrv, read, filed, &sdd, sizeof(sdd));
    ASSERT(dlen == (int)sdd.data_length,
           "Media size does not match:\n"
           "  %lu bytes of data in %s\n"
           "  %lu capacity in drive %c", sdd.data_length, imagepath, dlen, 'A' + drv);

    FIN(srv, lseek, filed, sdd.data_offset, SEEK_SET);

    dst = (pByte)drive + doff;

    drive->write_protect = 1;
    FIN(drrv, read, filed, dst, dlen);
    drive->write_protect = sdd.write_protect;

    if (debug)
        STUB("drrv = %d", drrv);

    close(filed);
    free_drive(drive);

    ASSERT(drrv == dlen,
           "%s: only %d bytes copied to drive %c\n"
           "  from file %s", program, drrv, 'A' + drv, imagepath);

    if (verbose)
        fprintf(stderr, "%s: Loaded %d bytes of data into drive %c\n"
                "  from file %s\n", program, drrv, 'A' + drv, imagepath);

    return 0;
}
