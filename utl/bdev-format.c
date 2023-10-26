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

// bdev-format: format a BDEV drive volume file
//
// USAGE:
//      bdev-format DRV ss spt [s3 [s4 ...]] [hexfile [hexfile ...]]
//
// DESCRIPTION:
//      The bdev-format command locates the storage to be used for the
//      specified drive for the BDEV simulation module, and arranges
//      it to represent mass storage media of capacity equal to the
//      products of the values on the command line, whose data content
//      is all set to 00h bytes.
//
//      size parameters may be hex (starting with 0x), octal (starting
//      with 0), or decimal (starting with 1 to 9).
//
//      The s1 value indicates the sector size, and is used when aligning
//      data from files. The s2 value indicates sectors per track and is
//      used in debug printing.
//
//      If a parameter is encountered (while scanning for the size) that
//      does not start with a digit, then it and subsequent parameters
//      are the names of files containing Intel HEX format data to be
//      loaded into the drive.
//
//      The first hexfile generally contains a boot sector, which will
//      be loaded into the first sector of the drive; the first data
//      record is loaded at the start of the sector, and the offset of
//      all other data is relative to the first record's address.
//
//      Each subsequent file is loaded starting in the next sector of
//      the drive not yet loaded.
//      

typedef struct sDiskStoreCtx {
    size_t              sector_size;            // bytes per sector (s1 parameter)
    size_t              sectors_per_track;      // sectors per track (s2 parameter)
    Byte               *disk_base;              // where disk is mapped in
    size_t              disk_size;              // size of disk (maybe 256256)
    long                disk_hwm;               // highest byte offset written to disk
    long                ffb_addr;               // first file byte's file address
    long                ffb_off;                // first file byte's disk offset
    long                file_hwm;               // highest ADDR from the current file
}                  *pDiskStoreCtx, DiskStoreCtx[1];

static void         disk_store_from_hex(DiskStoreCtx ctx, unsigned addr, unsigned data);

int main(int argc, Cstr *argv)
{
    Cstr                argp;
    char               *arge;

    assert(argc > 3);

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    int                 drv = strtol(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    int                 sector_size = strtol(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);

    assert(0 < sector_size);
    assert(sector_size < 65536);

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    int                 sectors_per_track = strtol(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);

    assert(0 < sectors_per_track);
    assert(sectors_per_track < 256);

    int                 sz = sector_size * sectors_per_track;
    while (NULL != (argp = *++argv)) {
        size_t              fac = strtol(argp, &arge, 0);
        if ('\0' != *arge)
            break;
        assert(0 < fac);
        assert(fac < 65536);
        sz *= fac;
        assert(sz > 1024);
        assert(sz < 8 * 1024 * 1024);
    }
    --argv;

    printf
      ("\nFormatting drive '%c'\n\twith %d bytes (%.1f KiB, %.1f MiB) of ZERO\n",
       'A' + drv, sz, sz / 1024.0, sz / (1024.0 * 1024.0));

    pDriveData          dd = new_drive(drv, sz);

    ASSERT(NULL != dd, "new_drive failed");

    free_drive(dd);

    dd = map_drive(drv);
    assert(dd->data_length >= (unsigned)sz);

    DiskStoreCtx        ctx;

    ctx->sector_size = sector_size;
    ctx->sectors_per_track = sectors_per_track;
    ctx->disk_base = dd->data_offset + (Byte *)dd;
    ctx->disk_size = dd->data_length;
    ctx->disk_hwm = -1;

    while (NULL != (argp = *++argv)) {
        fprintf(stderr, "%s\n", argp);

        ctx->ffb_addr = -1;
        ctx->ffb_off = -1;
        ctx->file_hwm = -1;

        hex_parse(argp, (HexStoreFn *)disk_store_from_hex, ctx);

        {
            unsigned            hwma = ctx->file_hwm;
            unsigned            hwmb = ctx->disk_hwm;
            unsigned            hwms = hwmb / sector_size;
            unsigned            hwmt = hwms / sectors_per_track;
            hwms -= hwmt * sectors_per_track;
            fprintf(stderr, "\t%04Xh -> Track %02d, Sector %02d\n", hwma, hwmt, hwms + 1);
        }
    }

    free_drive(dd);

    return 0;
}

void disk_store_from_hex(DiskStoreCtx ctx, unsigned addr, unsigned data)
{
    size_t              sector_size = ctx->sector_size;
    size_t              sectors_per_track = ctx->sectors_per_track;
    Byte               *disk_base = ctx->disk_base;
    size_t              disk_size = ctx->disk_size;

    long                file_hwm = ctx->file_hwm;
    long                disk_hwm = ctx->disk_hwm;
    long                ffb_addr = ctx->ffb_addr;
    long                ffb_off = ctx->ffb_off;

    if (ffb_addr < 0 || ffb_off < 0) {
        ffb_addr = addr;
        ffb_off = disk_hwm;
        ffb_off += sector_size - 1;
        ffb_off /= sector_size;
        ffb_off *= sector_size;
        ctx->ffb_addr = ffb_addr;
        ctx->ffb_off = ffb_off;

        {
            unsigned            hwmb = ffb_off;
            unsigned            hwms = hwmb / sector_size;
            unsigned            hwmt = hwms / sectors_per_track;
            hwms -= hwmt * sectors_per_track;

            fprintf(stderr, "\t%04Xh -> Track %02d, Sector %02d\n",
                    (unsigned)ffb_addr, hwmt, hwms + 1);
        }
    } else {
        assert(addr >= ffb_addr);
    }

    // fprintf(stderr, "data: %04Xh %02Xh\n", addr, data);

    if (file_hwm < addr)
        ctx->file_hwm = addr;

    addr -= ffb_addr;
    addr += ffb_off;
    assert(addr < disk_size);
    disk_base[addr] = data;
    if (disk_hwm < addr)
        ctx->disk_hwm = addr;
}
