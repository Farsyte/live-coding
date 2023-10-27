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

int main(int argc, Cstr *argv)
{
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

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    basename = argp;

    imagepath = drive_image_path(basename);

    filed = open(imagepath, O_RDONLY);

    ASSERT(0 <= filed,
           "Unable to access offline storage '%s'\n"
           "  error %d: %s\n", imagepath, errno, strerror(errno));

    doff = drive->data_offset;
    dlen = drive->data_length;

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

    ASSERT(drrv == dlen,
           "Transfer error: only %d bytes copied to drive %c\n"
           "  from media file %s", drrv, 'A' + drv, imagepath);

    close(filed);
    free_drive(drive);

    fprintf(stderr, "Loaded %d bytes of data\n"
            "  into drive %c from file %s\n", drrv, 'A' + drv, imagepath);

    return 0;
}
