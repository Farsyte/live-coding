#include <assert.h>
#include <unistd.h>
#include "bdev.h"

// Constants determining access times for rotating media
// 1. TAU to load the head onto the media
// 2. TAU per track for seek time
// 3. TAU per sector of disk rotation
// 4. TAU per byte when moving data

#define TAU_US	(18)
#define TAU_MS	(TAU_US * 1000)

// for testing: 10ms to switch drives
#define TAU_NEW_DRIVE	(TAU_MS * 10)

// for testing: 1ms to seek a track
#define TAU_SEEK_ONE	(TAU_MS * 1)

// for testing: 1ms per sector
#define TAU_SECTOR	(TAU_MS * 1)

// for testing: 1us per byte
#define TAU_BYTE        (TAU_US * 1)

typedef struct sBdevCtx {
    Tau                 busy_until;
    int                 last_state;
    int                 curr_dsk;
    int                 need_trk_0;
    int                 curr_trk;
    int                 curr_sec;
    int                 curr_byte;
    Word                curr_dma;
}                  *pBdevCtx, BdevCtx[1];

static Data         DATA;
static Bdev         d;
static BdevCtx      ctx;

static pEdge        CTRL_WR_ = d->wr[0];
static pEdge        STAT_RD_ = d->rd[0];

static pData        DMAH = d->DMAH;
static pEdge        DMAH_RD_ = d->rd[1];
static pEdge        DMAH_WR_ = d->wr[1];

static pData        DMAL = d->DMAL;
static pEdge        DMAL_RD_ = d->rd[2];
static pEdge        DMAL_WR_ = d->wr[2];

static pData        DSK = d->DSK;
static pEdge        DSK_RD_ = d->rd[3];
static pEdge        DSK_WR_ = d->wr[3];

static pData        TRK = d->TRK;
static pEdge        TRK_RD_ = d->rd[4];
static pEdge        TRK_WR_ = d->wr[4];

static pData        SEC = d->SEC;
static pEdge        SEC_RD_ = d->rd[5];
static pEdge        SEC_WR_ = d->wr[5];

static void         bdev_bist_impl(Bdev, BdevCtx);

// TODO add comments

void bdev_bist()
{
    Byte                b;

    data_init(DATA, "DATA");
    DATA->value = 0;

    bdev_init(d, "test_bdev");
    d->DATA = DATA;

    ctx->busy_until = TAU;
    ctx->curr_dsk = -1;
    ctx->need_trk_0 = -1;
    ctx->curr_trk = -1;
    ctx->curr_sec = -1;
    ctx->curr_byte = -1;
    ctx->curr_dma = 0xFFFF;

    BDEV_SET_IMPL(d, bdev_bist_impl, ctx);

    bdev_linked(d);

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
    b = DATA->value;                            \
    ++TAU;                                      \
    edge_hi(REG ## _RD_)

    WRR(SEC, 26);
    ASSERT_EQ_integer(26, SEC->value);
    RDR(SEC);
    ASSERT_EQ_integer(26, b);

    WRR(TRK, 36);
    WRR(DSK, 3);
    WRR(DMAL, 32);
    WRR(DMAH, 2);

    ASSERT_EQ_integer(26, SEC->value);
    ASSERT_EQ_integer(36, TRK->value);
    ASSERT_EQ_integer(3, DSK->value);
    ASSERT_EQ_integer(32, DMAL->value);
    ASSERT_EQ_integer(2, DMAH->value);

    RDR(SEC);
    ASSERT_EQ_integer(26, b);

    RDR(TRK);
    ASSERT_EQ_integer(36, b);

    RDR(DSK);
    ASSERT_EQ_integer(3, b);

    RDR(DMAL);
    ASSERT_EQ_integer(32, b);

    RDR(DMAH);
    ASSERT_EQ_integer(2, b);

    WRR(SEC, 1);
    WRR(TRK, 0);
    WRR(DSK, 0);
    WRR(DMAL, 128);
    WRR(DMAH, 0);

    ASSERT_EQ_integer(1, SEC->value);
    ASSERT_EQ_integer(0, TRK->value);
    ASSERT_EQ_integer(0, DSK->value);
    ASSERT_EQ_integer(128, DMAL->value);
    ASSERT_EQ_integer(0, DMAH->value);

    RDR(SEC);
    ASSERT_EQ_integer(1, b);

    RDR(TRK);
    ASSERT_EQ_integer(0, b);

    RDR(DSK);
    ASSERT_EQ_integer(0, b);

    RDR(DMAL);
    ASSERT_EQ_integer(128, b);

    RDR(DMAH);
    ASSERT_EQ_integer(0, b);

    WRR(CTRL, BDEV_CTRL_NOP);

    RDR(STAT);
    ASSERT_EQ_integer(BDEV_STAT_IDLE, b);

    WRR(CTRL, BDEV_CTRL_RD);

    // TODO read status up to N times until we get IDLE
    //   error if we read anything but BUSY or IDLE
    //   error if we do not read IDLE before the limit

    for (int i = 0; i < 1000000000; ++i) {
        RDR(STAT);
        if (b == BDEV_STAT_IDLE) {
            STUB("STAT IDLE at i=%d", i);
            break;
        }
        ASSERT_EQ_integer(BDEV_STAT_BUSY, b);
    }
    RDR(STAT);
    ASSERT_EQ_integer(BDEV_STAT_IDLE, b);

    WRR(CTRL, BDEV_CTRL_WR);

    // TODO read status up to N times until we get IDLE
    //   error if we read anything but BUSY or IDLE
    //   error if we do not read IDLE before the limit

    for (int i = 0; i < 1000000000; ++i) {
        RDR(STAT);
        if (b == BDEV_STAT_IDLE) {
            STUB("STAT IDLE at i=%d", i);
            break;
        }
        ASSERT_EQ_integer(BDEV_STAT_BUSY, b);
    }
    RDR(STAT);
    ASSERT_EQ_integer(BDEV_STAT_IDLE, b);

    (void)CTRL_WR_;     // TODO impl
    (void)STAT_RD_;     // TODO impl

    PRINT_END();
}

static void bdev_bist_impl(Bdev d, BdevCtx c)
{
    Byte                b = 0x55;

    for (;;) {
        if (TAU < c->busy_until)
            return;

        if (d->current_state == BDEV_STATE_IDLE) {
            c->last_state = d->current_state;
            c->busy_until = TAU;
            return;
        }

        if (c->last_state != d->current_state) {
            c->last_state = d->current_state;
            c->curr_byte = 0;
            c->curr_dma = (d->DMAH->value << 8) | d->DMAL->value;
            c->busy_until = TAU;
            return;
        }

        if (c->curr_dsk != d->DSK->value) {
            c->curr_dsk = d->DSK->value;
            c->need_trk_0 = 1;
            c->curr_trk = 34;
            c->busy_until += TAU_NEW_DRIVE;
            STUB("switch to drive %c", c->curr_dsk + 'A');
            continue;
        }

        if (c->need_trk_0 && c->curr_trk > 0) {
            STUB("recal, seek in");
            c->curr_trk--;
            c->busy_until += TAU_SEEK_ONE;
            if (0 == c->curr_trk) {
                STUB("recal found track 0");
                c->need_trk_0 = 0;
            }
            continue;
        }

        if (c->curr_trk < d->TRK->value) {
            STUB("seek out");
            c->curr_trk++;
            c->busy_until += TAU_SEEK_ONE;
            continue;
        }

        if (c->curr_trk > d->TRK->value) {
            STUB("seek in");
            c->curr_trk--;
            c->busy_until += TAU_SEEK_ONE;
            continue;
        }

        if (c->curr_sec != d->SEC->value) {
            STUB("sector wait");
            c->curr_sec = (c->curr_sec % 26) + 1;
            c->busy_until += TAU_SECTOR;
            continue;
        }

        if (c->curr_byte < 128) {
            switch (d->current_state) {
              case BDEV_STATE_RD:
                  // TODO get mass storage data into "b"
                  // TODO put "b" into memory at curr_dma
                  //
                  // THINK. Would it be better to avoid DMA entirely
                  // and have the CPU use PIO to receive each byte?
                  //
                  STUB("%s RD %04XH %c:%02d.%02d[%03d] ==> %02X",
                       d->name, c->curr_dma,
                       d->DSK->value + 'A', d->TRK->value, d->SEC->value, c->curr_byte, b);
                  break;
              case BDEV_STATE_WR:
                  // TODO get "b" from memory at curr_dma
                  // TODO put "b" into mass storage
                  //
                  // THINK. Would it be better to avoid DMA entirely
                  // and have the CPU use PIO to provide each byte?
                  //
                  STUB("%s WR %04XH %c:%02d.%02d[%03d] <== %02X",
                       d->name, c->curr_dma,
                       d->DSK->value + 'A', d->TRK->value, d->SEC->value, c->curr_byte, b);
                  break;
            }
            c->curr_byte++;
            c->curr_dma++;
            c->busy_until += TAU_BYTE;
            continue;
        }

        STUB("%s operation complete", d->name);
        d->current_state = BDEV_STATE_IDLE;
    }
}
