#include <assert.h>
#include <unistd.h>
#include "bdev.h"

// capacity numbers selected to match IBM format 8" diskette,
// which is quoted 2.0 megabits per diskette.
//
// actual capacity is 256256 bytes, or 2,050,048 bits.
//
// IBM is using millions of bits, which became industry standard for
// mass storage. This makes sense as the history before this was paper
// tape and mag tape, where powers of two simply are not something
// that comes to mind.

#define	BPS	128
#define SPT	26
#define TPD	77
#define DPC	4

// Bctx: context for the Bdev Test code,
// a controller containing four IBM-formatted 8-inch diskettes
// containing a specific barber-pole data pattern (each byte
// has the low 8 bits of the sum of the disk, track, sector,
// and byte offset, where the first sector is 1).

typedef struct sBctx {
    pBdev               BDEV;
    Byte                data[DPC][TPD][SPT][BPS];
}                  *pBctx, Bctx[1];

static Data         DATA;
static Bdev         BDEV;
static Bctx         BCTX;

static void         bctx_init(Bctx BCTX, Bdev BDEV);
static void         bctx_seek(Bctx BCTX);

// TODO add comments

void bdev_bist()
{
    Byte                data;
    Byte                exp;

    data_init(DATA, "DATA");
    DATA->value = 0;

    bdev_init(BDEV, "test_bdev");
    BDEV->DATA = DATA;

    bctx_init(BCTX, BDEV);

    bdev_linked(BDEV);

    BDEV_ALL(BDEV);

#define WRR(REG,VAL)                            \
    ++TAU;                                      \
    DATA->value = VAL;                          \
    edge_lo(REG ## _WR_);                       \
    ++TAU;                                      \
    edge_hi(REG ## _WR_ )

#define RDR(REG)                                \
    ++TAU;                                      \
    DATA->value = 0x55;                         \
    edge_lo(REG ## _RD_);                       \
    data = DATA->value;                         \
    ++TAU;                                      \
    edge_hi(REG ## _RD_)

    WRR(SEC, 26);
    ASSERT_EQ_integer(26, SEC->value);
    RDR(SEC);
    ASSERT_EQ_integer(26, data);

    WRR(TRK, 36);
    WRR(DSK, 3);

    ASSERT_EQ_integer(26, SEC->value);
    ASSERT_EQ_integer(36, TRK->value);
    ASSERT_EQ_integer(3, DSK->value);

    RDR(SEC);
    ASSERT_EQ_integer(26, data);

    RDR(TRK);
    ASSERT_EQ_integer(36, data);

    RDR(DSK);
    ASSERT_EQ_integer(3, data);

    WRR(SEC, 1);
    WRR(TRK, 0);
    WRR(DSK, 0);

    ASSERT_EQ_integer(1, SEC->value);
    ASSERT_EQ_integer(0, TRK->value);
    ASSERT_EQ_integer(0, DSK->value);

    RDR(SEC);
    ASSERT_EQ_integer(1, data);

    RDR(TRK);
    ASSERT_EQ_integer(0, data);

    RDR(DSK);
    ASSERT_EQ_integer(0, data);

    // TODO read several sectors.
    // verify we see the blank scratch disks.

    // Read all storage without updating DSK/TRK/SEC
    for (int dsk = 0; dsk < DPC; ++dsk) {
        WRR(DSK, dsk);
        WRR(TRK, 0);
        WRR(SEC, 1);
        for (int trk = 0; trk < TPD; ++trk) {
            for (int sec = 1; sec <= SPT; ++sec) {
                for (int b = 0; b < BPS; ++b) {
                    exp = dsk + trk + sec + b;
                    RDR(DAT);
                    ASSERT_EQ_integer(exp, data);
                }
            }
        }
    }

    // same thing, but reverse order, setting DSK/TRK/SEC
    // as we step from one to the next.
    for (int dsk = DPC - 1; dsk >= 0; --dsk) {
        WRR(DSK, dsk);
        for (int trk = TPD - 1; trk >= 0; --trk) {
            for (int sec = SPT; sec >= 1; --sec) {
                WRR(TRK, trk);
                WRR(SEC, sec);
                for (int b = 0; b < BPS; ++b) {
                    exp = dsk + trk + sec + b;
                    RDR(DAT);
                    ASSERT_EQ_integer(exp, data);
                }
            }
        }
    }

    // write several sectors.
    // verify the BCTX receives the expected update.

    for (int dsk = 1; dsk <= DPC; dsk += 2) {
        WRR(DSK, dsk);
        for (int trk = 13; trk >= 0; trk -= 10) {
            for (int sec = 14; sec <= SPT; sec += 10) {
                WRR(TRK, trk);
                WRR(SEC, sec);
                for (int b = 0; b < BPS; ++b) {
                    exp = dsk + trk + sec + b + 0x55;
                    WRR(DAT, exp);
                    ASSERT_EQ_integer(exp, BCTX->data[dsk][trk][sec - 1][b]);
                }
            }
        }
    }

    // read the sectors we wrote.
    // verify we get back the updated data.

    for (int dsk = 1; dsk <= DPC; dsk += 2) {
        WRR(DSK, dsk);
        for (int trk = 13; trk >= 0; trk -= 10) {
            for (int sec = 14; sec <= SPT; sec += 10) {
                WRR(TRK, trk);
                WRR(SEC, sec);
                for (int b = 0; b < BPS; ++b) {
                    exp = dsk + trk + sec + b + 0x55;
                    RDR(DAT);
                    ASSERT_EQ_integer(exp, data);
                }
            }
        }
    }

    // Start reading an arbitrary sector,
    // and hit RESET in the middle to verify
    // that we get back to the proper state.

    WRR(DSK, 1);
    WRR(TRK, 10);
    WRR(SEC, 10);
    for (int b = 0; b < 32; ++b) {
        RDR(DAT);
    }

    ++TAU;
    edge_lo(RESET_);

    ++TAU;
    edge_hi(RESET_);

    // RESET should select the boot sector of the boot disk,
    // and reading should get the boot data, NOT the remainder
    // of what we were reading above.

    RDR(DSK);
    ASSERT_EQ_integer(0, data);

    RDR(TRK);
    ASSERT_EQ_integer(0, data);

    RDR(SEC);
    ASSERT_EQ_integer(1, data);

    for (int b = 0; b < BPS; ++b) {
        exp = 0 + 0 + 1 + b;
        RDR(DAT);
        ASSERT_EQ_integer(exp, data);
    }

}

static void bctx_init(Bctx BCTX, Bdev BDEV)
{
    BCTX->BDEV = BDEV;

    // Format the "blank" scratch disks to have data we can use
    // to detect access to the wrong area.

    for (int dsk = 0; dsk < DPC; ++dsk)
        for (int trk = 0; trk < TPD; ++trk)
            for (int sec = 1; sec <= SPT; ++sec)
                for (int b = 0; b < BPS; ++b)
                    BCTX->data[dsk][trk][sec - 1][b] = dsk + trk + sec + b;

    BDEV_SET_SEEK(BDEV, bctx_seek, BCTX);
}

static void bctx_seek(Bctx BCTX)
{
    pBdev               BDEV = BCTX->BDEV;

    ASSERT(BDEV, "BCTX has null BDEV");

    BDEV_ALL(BDEV);

    Byte                dsk = DSK->value;
    Byte                trk = TRK->value;
    Byte                sec = SEC->value;

    if ((dsk >= DPC) || (trk >= TPD) || (sec < 1) || (sec > SPT)) {
//        STUB("%c:%02d.%02d is not a valid location", 'A' + dsk, trk, sec);
        return;
    }

    cursor = BCTX->data[dsk][trk][sec - 1];
    cursor_lim = cursor + BPS;

//    STUB("%c:%02d.%02d starts at offset %lu", 'A' + dsk, trk, sec,
//         cursor - BCTX->data[0][0][0]);
//    STUB("%c:%02d.%02d   ends at offset %lu", 'A' + dsk, trk, sec,
//         cursor_lim - BCTX->data[0][0][0]);

    if (sec < SPT) {
        data_to(SEC, sec + 1);
    } else {
        data_to(SEC, 1);
        data_to(TRK, trk + 1);
    }

    BDEV->cursor = cursor;
    BDEV->cursor_lim = cursor_lim;
}
