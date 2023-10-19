#include "common/hex.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <stdlib.h>     // General utilities

// hex: parse the content of a hex file.

#define HEX_START_CODE	':'

static int          hex_digit_value(int ch);
static int          fgetb(FILE *fp);

// hex_parse: parse a named file in intel HEX format.
//
// Returns -1 with errno set for some errors.
// Returns -2 if the file is truncated.
// Returns -3 if a character should be a hex digit but is not.

int hex_parse(Cstr filename, HexStoreFn * store, void *ctx)
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

        ASSERT_EQ_integer(sum_expected, sum_computed);
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
