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

// FIN macro: handle the most common error reporting mechanism. Used
// for functions that report errors by returning a negative value
// (with errno set), while zero and positive are success. This macro
// reports errors and aborts, and should be used when recovery from
// the function failing is not appropriate.

#define FIN(var, fn, ...)                                               \
    int var = fn(__VA_ARGS__);                                          \
    if (var >= 0) { } else { perror(#fn); abort(); }

typedef uint8_t     byte;
typedef const char *Cstr;

typedef void HexStoreFn (void *ctx, unsigned addr, unsigned data);

typedef struct sDiskStoreCtx {
    size_t              sector_size;            // bytes per sector (s1 parameter)
    size_t              sectors_per_track;      // sectors per track (s2 parameter)
    byte               *disk_base;              // where disk is mapped in
    size_t              disk_size;              // size of disk (maybe 256256)
    long                disk_hwm;               // highest byte offset written to disk
    long                ffb_addr;               // first file byte's file address
    long                ffb_off;                // first file byte's disk offset
    long                file_hwm;               // highest ADDR from the current file
}                  *pDiskStoreCtx, DiskStoreCtx[1];

static void         disk_store_from_hex(DiskStoreCtx ctx, unsigned addr, unsigned data);
static int          hex_parse(Cstr filename, HexStoreFn *store, void *ctx);
static int          open_drive(Cstr drv);
static void         size_drive(int fd, size_t sz, Cstr drv);
static size_t       pagesize();
static size_t       roundpage(size_t sz, size_t psz);
static byte        *map(int fd, Cstr drv, size_t msz);

int main(int argc, Cstr * argv)
{
    Cstr                argp;
    char               *arge;

    assert(argc > 3);

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    Cstr                drv = argp;

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    size_t              sector_size = strtoul(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);

    assert(0 < sector_size);
    assert(sector_size < 65536);

    argp = *++argv;
    assert(NULL != argp);
    assert('\0' != argp[0]);
    size_t              sectors_per_track = strtoul(argp, &arge, 0);
    assert(argp < arge);
    assert('\0' == *arge);

    assert(0 < sectors_per_track);
    assert(sectors_per_track < 256);

    size_t              sz = sector_size * sectors_per_track;
    while (NULL != (argp = *++argv)) {
        size_t              fac = strtoul(argp, &arge, 0);
        if ('\0' != *arge)
            break;
        assert(0 < fac);
        assert(fac < 65536);
        sz *= fac;
        assert(sz > 1024);
        assert(sz < 8 * 1024 * 1024);
    }
    --argv;

    printf("\nFormatting %s\n\twith %lu bytes (%.1f KiB, %.1f MiB) of ZERO\n",
           drv, sz, sz / 1024.0, sz / (1024.0 * 1024.0));

    int                 fd = open_drive(drv);
    size_drive(fd, sz, drv);

    size_t              psz = pagesize();
    size_t              msz = roundpage(sz, psz);

    byte               *mp = map(fd, drv, msz);

    close(fd);          // remains mapped after closing.

    memset(mp, 0x00, msz);

    DiskStoreCtx        ctx;

    ctx->sector_size = sector_size;
    ctx->sectors_per_track = sectors_per_track;
    ctx->disk_base = mp;
    ctx->disk_size = sz;
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

    return 0;
}

void disk_store_from_hex(DiskStoreCtx ctx, unsigned addr, unsigned data)
{
    size_t              sector_size = ctx->sector_size;
    size_t              sectors_per_track = ctx->sectors_per_track;
    byte               *disk_base = ctx->disk_base;
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

#define HEX_START_CODE	':'

static int          hex_digit_value(int ch);
static int          fgetb(FILE *fp);

// hex_parse: parse a named file in intel HEX format.
//
// Returns -1 with errno set for some errors.
// Returns -2 if the file is truncated.
// Returns -3 if a character should be a hex digit but is not.

static int hex_parse(Cstr filename, HexStoreFn *store, void *ctx)
{
    FILE               *fp;
    int                 ch;
    int                 ret;

    unsigned            base = 0;
    int                 line = 0;
    int                 alldone = 0;

    unsigned            sum = 0;
    unsigned            size = 0;
    unsigned            addr = 0;
    unsigned            type = 0;
    unsigned            data = 0;
    unsigned            sum_expected = 0;

    unsigned            cs_addr = 0;
    unsigned            ip_addr = 0;
    unsigned            exec_addr = 0;

    fp = fopen(filename, "r");
    if (!fp)
        return -1;      // no bist coverage

    while (!alldone) {
        line++;

        // ignore input until we hit a START marker,
        // which is an ASCII COLON.

        while (':' != (ch = fgetc(fp)))
            if (ch == EOF) {
                ret = -2;       // no bist coverage
                goto fail;      // no bist coverage
            }
#define MERGE(var)                                                      \
        ret = fgetb(fp);                                                \
        if (ret < 0)                                                    \
            return ret;                                                 \
        sum += ret;                                                     \
        var <<= 8;                                                      \
        var |= ret & 255;

#define GETB(var)                                                       \
        var = 0;                                                        \
        MERGE(var);

#define GETW(var)                                                       \
        var = 0;                                                        \
        MERGE(var);                                                     \
        MERGE(var);

#define GETD(var)                                                       \
        var = 0;                                                        \
        MERGE(var);                                                     \
        MERGE(var);                                                     \
        MERGE(var);                                                     \
        MERGE(var);

        GETB(size);
        GETW(addr);

        GETB(type);

        switch (type) {

          case 0x00:   // DATA record
              for (unsigned i = 0; i < size; ++i) {
                  GETB(data);

                  store(ctx, addr, data);
                  addr++;
              }
              break;

          case 0x01:   // END OF FILE record
              alldone = 1;
              break;

              // NO BIST COVERAGE for cases 0x02, 0x03, 0x04, 0x05

          case 0x02:   // extended segment address              
              GETW(base);
              base *= 16;
              break;

          case 0x03:   // start segment address                 
              GETW(cs_addr);
              GETW(ip_addr);
              break;

          case 0x04:   // extendended linear address            
              GETD(base);
              break;

          case 0x05:   // start linear address                  
              GETD(exec_addr);
              break;
        }

        unsigned            sum_computed = ((sum ^ 0xFF) + 1) & 0xFF;
        GETB(sum_expected);

        if (sum_expected != sum_computed) {
            // NO BIST COVERAGE for checksum error path
            fprintf(stderr, "%s:%d: checksum error\n", filename, line);
            fprintf(stderr, "  expected: 0x%02X\n", sum_expected);
            fprintf(stderr, "  observed: 0x%02X\n", sum_computed);
            abort();
        }

        assert(0 == (sum & 0xFF));
    }
    ret = 0;

    (void)cs_addr;
    (void)ip_addr;
    (void)exec_addr;

fail:
    fclose(fp);
    return ret;
}

// hex_digit_value(ch): convert hex digit to value.
// returns the value of the digit, or 10..15 for A..F or a..f;
// returns -3 if the character is not a hex digit.

static int hex_digit_value(int ch)
{
    if ('0' <= ch && ch <= '9')
        return ch - '0';
    if ('A' <= ch && ch <= 'F')
        return ch - 'A' + 10;
    // NO BIST COVERAGE for anything but 0..9 and A..F
    if ('a' <= ch && ch <= 'f')
        return ch - 'a' + 10;
    return -3;
}

// fgetb(fp): get two hex digits from the file.
// returns the eight bit value represented by the digits;
// returns -2 if an end-of-file is encountered;
// returns -3 if an input is not a hex digit.
//
// NO BIST COVERAGE for error paths

static int fgetb(FILE *fp)
{
    int                 c1 = fgetc(fp);
    if (c1 == EOF)
        return -2;
    int                 c2 = fgetc(fp);
    if (c2 == EOF)
        return -2;
    c1 = hex_digit_value(c1);
    if (c1 < 0)
        return -3;
    c2 = hex_digit_value(c2);
    if (c1 < 0)
        return -3;
    return c1 * 16 + c2;
}
static int open_drive(Cstr drv)
{
    int                 fd = open(drv, O_RDWR | O_CREAT, 0640);
    if (fd < 0) {
        perror(drv);
        abort();
    }
    return fd;
}
static void size_drive(int fd, size_t sz, Cstr drv)
{
    int                 rv = ftruncate(fd, sz);
    if (rv < 0) {
        perror(drv);
        abort();
    }
}
static size_t pagesize()
{
    int                 psz = sysconf(_SC_PAGESIZE);
    if (psz < 0) {
        perror("sysconf(_SC_PAGESIZE)");
        abort();
    }
    return psz;
}
static size_t roundpage(size_t sz, size_t psz)
{
    size_t              pages = (sz + psz - 1) / psz;
    size_t              msz = pages * psz;
    return msz;
}
static byte        *map(int fd, Cstr drv, size_t msz)
{
    void               *vp;

    vp = mmap(NULL, msz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (((void *)-1) == vp) {
        perror(drv);
        abort();
    }
    return (byte *) vp;

}
