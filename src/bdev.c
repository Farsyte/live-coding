#include "bdev.h"
#include <assert.h>
#include <ctype.h>
#include <unistd.h>

#define NAME_RD(REG)	bdev_rd_ ## REG
#define	SIG_RD(REG)	NAME_RD(REG) (Bdev d)
#define DECL_RD(REG)	static void SIG_RD(REG)
#define NAME_WR(REG)	bdev_wr_ ## REG
#define	SIG_WR(REG)	NAME_WR(REG) (Bdev d)
#define DECL_WR(REG)	static void SIG_WR(REG)

DECL_RD(STAT);
DECL_RD(DMAH);
DECL_RD(DMAL);
DECL_RD(DSK);
DECL_RD(TRK);
DECL_RD(SEC);

DECL_WR(CTRL);
DECL_WR(DMAH);
DECL_WR(DMAL);
DECL_WR(DSK);
DECL_WR(TRK);
DECL_WR(SEC);

void bdev_init(Bdev d, Cstr name)
{
    ASSERT(d, "dev ptr must not be NULL");
    ASSERT(name, "dev name must not be NULL");
    ASSERT(name[0], "dev name must not be empty");

    d->name = name;

    edge_init(d->wr[0], format("%s:/CTRL_WR", name), 1);
    edge_init(d->rd[0], format("%s:/STAT_RD", name), 1);

    data_init(d->DMAH, format("%s:DMAH", name));
    edge_init(d->rd[1], format("%s:/DMAH_RD", name), 1);
    edge_init(d->wr[1], format("%s:/DMAH_WR", name), 1);

    data_init(d->DMAL, format("%s:DMAL", name));
    edge_init(d->rd[2], format("%s:/DMAL_RD", name), 1);
    edge_init(d->wr[2], format("%s:/DMAL_WR", name), 1);

    data_init(d->DSK, format("%s:DSK", name));
    edge_init(d->rd[3], format("%s:/DSK_RD", name), 1);
    edge_init(d->wr[3], format("%s:/DSK_WR", name), 1);

    data_init(d->TRK, format("%s:TRK", name));
    edge_init(d->rd[4], format("%s:/TRK_RD", name), 1);
    edge_init(d->wr[4], format("%s:/TRK_WR", name), 1);

    data_init(d->SEC, format("%s:SEC", name));
    edge_init(d->rd[5], format("%s:/SEC_RD", name), 1);
    edge_init(d->wr[5], format("%s:/SEC_WR", name), 1);

    d->DATA = NULL;

    d->requested_op = BDEV_CTRL_NOP;
    d->current_state = BDEV_STATE_IDLE;

    d->impl_ctx = NULL;
    d->impl_fn = NULL;
}

void bdev_set_impl(Bdev d, bdev_impl_fn *fn, void *ctx)
{
    d->impl_ctx = ctx;
    d->impl_fn = fn;
}

void bdev_invar(Bdev d)
{
    ASSERT(d, "dev ptr must not be NULL");
    ASSERT(d->name, "dev name must not be NULL");
    ASSERT(d->name[0], "dev name must not be empty");

    data_invar(d->DMAH);
    data_invar(d->DMAL);
    data_invar(d->DSK);
    data_invar(d->TRK);
    data_invar(d->SEC);

    edge_invar(d->rd[0]);
    edge_invar(d->wr[0]);
    edge_invar(d->rd[1]);
    edge_invar(d->wr[1]);
    edge_invar(d->rd[2]);
    edge_invar(d->wr[2]);
    edge_invar(d->rd[3]);
    edge_invar(d->wr[3]);
    edge_invar(d->rd[4]);
    edge_invar(d->wr[4]);
    edge_invar(d->rd[5]);
    edge_invar(d->wr[5]);

    data_invar(d->DATA);

    ASSERT(NULL != d->impl_fn, "bdev needs an impl_fn");
}

void bdev_linked(Bdev d)
{
    bdev_invar(d);

    EDGE_ON_FALL(d->wr[0], bdev_wr_CTRL, d);
    EDGE_ON_FALL(d->rd[0], bdev_rd_STAT, d);

    EDGE_ON_FALL(d->rd[1], bdev_rd_DMAH, d);
    EDGE_ON_FALL(d->wr[1], bdev_wr_DMAH, d);

    EDGE_ON_FALL(d->rd[2], bdev_rd_DMAL, d);
    EDGE_ON_FALL(d->wr[2], bdev_wr_DMAL, d);

    EDGE_ON_FALL(d->rd[3], bdev_rd_DSK, d);
    EDGE_ON_FALL(d->wr[3], bdev_wr_DSK, d);

    EDGE_ON_FALL(d->rd[4], bdev_rd_TRK, d);
    EDGE_ON_FALL(d->wr[4], bdev_wr_TRK, d);

    EDGE_ON_FALL(d->rd[5], bdev_rd_SEC, d);
    EDGE_ON_FALL(d->wr[5], bdev_wr_SEC, d);
}

#define IMPL_WR(REG)                            \
    DECL_WR(REG) {                              \
        data_to(d->REG, d->DATA->value);        \
    } DECL_WR(REG)

IMPL_WR(DMAH);
IMPL_WR(DMAL);
IMPL_WR(DSK);
IMPL_WR(TRK);
IMPL_WR(SEC);

#define IMPL_RD(REG)                            \
    DECL_RD(REG) {                              \
        data_to(d->DATA, d->REG->value);        \
    } DECL_RD(REG)

IMPL_RD(DMAH);
IMPL_RD(DMAL);
IMPL_RD(DSK);
IMPL_RD(TRK);
IMPL_RD(SEC);

// The actual work of the mass storage unit is performed
// as the simulated CPU reads the STAT register.

static void bdev_wr_CTRL(Bdev d)
{
    d->requested_op = d->DATA->value;

    if (d->current_state == BDEV_STATE_IDLE) {
        switch (d->requested_op) {
          case BDEV_CTRL_NOP:
              d->current_state = BDEV_STATE_IDLE;
              d->requested_op = BDEV_CTRL_NOP;
              break;
          case BDEV_CTRL_RD:
              d->current_state = BDEV_STATE_RD;
              d->requested_op = BDEV_CTRL_NOP;
              break;
          case BDEV_CTRL_WR:
              d->current_state = BDEV_STATE_WR;
              d->requested_op = BDEV_CTRL_NOP;
              break;
          default:
              STUB("bdev %s: requested op %02X not recognized", d->name, d->requested_op);
        }
    } else {
        STUB("bdev %s: requested op %02X but state is %02X",
             d->name, d->requested_op, d->current_state);
    }

    d->impl_fn(d, d->impl_ctx);
}

static void bdev_rd_STAT(Bdev d)
{
    d->impl_fn(d, d->impl_ctx);

    if (d->current_state == BDEV_STATE_IDLE)
        data_to(d->DATA, BDEV_STAT_IDLE);
    else
        data_to(d->DATA, BDEV_STAT_BUSY);
}
