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

// bdev-st: store a drive image into a file
//
// USAGE:
//      bdev-st DRV destination-file
//
// DESCRIPTION:
//      The bdev-st command simulates the act of removing a diskette
//      from a drive, by the simple expedient of copying all of the
//      information from the drive file into the named file.
//
//      This command will fail if the destinatino file exists and
//      is not writable. This is intentional.
//
//      When the command terminates, the drive will still have the
//      data from the media present; the destination file will have an
//      image of the header and data, but the length of the
//      destination will not have been rounded up for access via MMAP.

int main(int argc, Cstr *argv)
{
    Cstr                argp;
    char               *arge;
    int                 drv;
    pDriveData          drive;
    Cstr                basename;
    Cstr                imagepath;
    int                 filed;
    unsigned            doff;
    unsigned            dlen;
    pByte               src;

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

    filed = open(imagepath, O_RDWR | O_CREAT, 0666);

    if ((filed < 0) && (errno == EACCES)) {
        fprintf(stderr,
                "Disk image file %s\n  is marked as READ-ONLY\n  and has not been updated.\n",
                imagepath);
        return 0;
    }

    ASSERT(0 <= filed,
           "Unable to create/update disk image file '%s'\n"
           "  error %d: %s\n", imagepath, errno, strerror(errno));

    doff = drive->data_offset;
    dlen = drive->data_length;
    src = (pByte)drive;

    FIN(ft, ftruncate, filed, doff + dlen);
    FIN(wr, write, filed, src, doff + dlen);

    close(filed);
    free_drive(drive);

    fprintf(stderr, "Stored %u bytes of header and %u bytes of data\n"
            "  from drive %c to file %s\n", doff, dlen, 'A' + drv, imagepath);

    return 0;
}
