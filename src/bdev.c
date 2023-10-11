#include "bdev.h"
#include <assert.h>
#include <ctype.h>
#include <unistd.h>

#define NAME_RD(REG)	bdev_rd_ ## REG
#define	SIG_RD(REG)	NAME_RD(REG) (Bdev BDEV)
#define DECL_RD(REG)	static void SIG_RD(REG)
#define NAME_WR(REG)	bdev_wr_ ## REG
#define	SIG_WR(REG)	NAME_WR(REG) (Bdev BDEV)
#define DECL_WR(REG)	static void SIG_WR(REG)

static void         bdev_reset(Bdev BDEV);

DECL_RD(DSK);
DECL_RD(TRK);
DECL_RD(SEC);
DECL_RD(DAT);

DECL_WR(DSK);
DECL_WR(TRK);
DECL_WR(SEC);
DECL_WR(DAT);

// TODO add comments

void bdev_init(Bdev BDEV, Cstr name)
{
    ASSERT(BDEV, "dev ptr must not be NULL");
    ASSERT(name, "dev name must not be NULL");
    ASSERT(name[0], "dev name must not be empty");

    BDEV->name = name;

    BDEV->DATA = NULL;
    BDEV->seek = NULL;
    BDEV->seek_ctx = NULL;
    BDEV->cursor = NULL;
    BDEV->cursor_lim = NULL;

    BDEV_ALL(BDEV);

    data_init(DSK, format("%s:DSK", name));
    data_init(TRK, format("%s:TRK", name));
    data_init(SEC, format("%s:SEC", name));

    edge_init(RESET_, format("%s:/RESET", name), 1);

    edge_init(DSK_RD_, format("%s:/DSK_RD", name), 1);
    edge_init(DSK_WR_, format("%s:/DSK_WR", name), 1);

    edge_init(TRK_RD_, format("%s:/TRK_RD", name), 1);
    edge_init(TRK_WR_, format("%s:/TRK_WR", name), 1);

    edge_init(SEC_RD_, format("%s:/SEC_RD", name), 1);
    edge_init(SEC_WR_, format("%s:/SEC_WR", name), 1);

    edge_init(DAT_RD_, format("%s:/DAT_RD", name), 1);
    edge_init(DAT_WR_, format("%s:/DAT_WR", name), 1);
}

void bdev_set_seek(Bdev BDEV, bdev_seek_fn *fn, void *ctx)
{
    BDEV->seek = fn;
    BDEV->seek_ctx = ctx;
    BDEV->cursor = NULL;
    BDEV->cursor_lim = NULL;
}

void bdev_invar(Bdev BDEV)
{

    ASSERT(BDEV, "dev ptr must not be NULL");
    ASSERT(BDEV->name, "dev name must not be NULL");
    ASSERT(BDEV->name[0], "dev name must not be empty");

    BDEV_ALL(BDEV);

    edge_invar(RESET_);

    data_invar(DSK);
    edge_invar(DSK_RD_);
    edge_invar(DSK_WR_);

    data_invar(TRK);
    edge_invar(TRK_RD_);
    edge_invar(TRK_WR_);

    data_invar(SEC);
    edge_invar(SEC_RD_);
    edge_invar(SEC_WR_);

    edge_invar(DAT_RD_);
    edge_invar(DAT_WR_);

    data_invar(DATA);

    ASSERT(seek, "bdev: no 'seek' function registered");
}

void bdev_linked(Bdev BDEV)
{
    bdev_invar(BDEV);

    BDEV_ALL(BDEV);

    EDGE_ON_FALL(RESET_, bdev_reset, BDEV);

    EDGE_ON_FALL(DSK_RD_, bdev_rd_DSK, BDEV);
    EDGE_ON_FALL(DSK_WR_, bdev_wr_DSK, BDEV);

    EDGE_ON_FALL(TRK_RD_, bdev_rd_TRK, BDEV);
    EDGE_ON_FALL(TRK_WR_, bdev_wr_TRK, BDEV);

    EDGE_ON_FALL(SEC_RD_, bdev_rd_SEC, BDEV);
    EDGE_ON_FALL(SEC_WR_, bdev_wr_SEC, BDEV);

    EDGE_ON_FALL(DAT_RD_, bdev_rd_DAT, BDEV);
    EDGE_ON_FALL(DAT_WR_, bdev_wr_DAT, BDEV);
}

static void bdev_reset(Bdev BDEV)
{
    BDEV_ALL(BDEV);

    data_to(DSK, 0);
    data_to(TRK, 0);
    data_to(SEC, 1);

    BDEV->cursor = NULL;
    BDEV->cursor_lim = NULL;
}

#define IMPL_WR(REG)                                  \
    DECL_WR(REG) {                                    \
        data_to(BDEV->REG, BDEV->DATA->value);        \
    } DECL_WR(REG)

IMPL_WR(DSK);
IMPL_WR(TRK);
IMPL_WR(SEC);

#define IMPL_RD(REG)                                  \
    DECL_RD(REG) {                                    \
        data_to(BDEV->DATA, BDEV->REG->value);        \
    } DECL_RD(REG)

IMPL_RD(DSK);
IMPL_RD(TRK);
IMPL_RD(SEC);

static void bdev_rd_DAT(Bdev BDEV)
{
    bdev_invar(BDEV);

    BDEV_ALL(BDEV);

    if (cursor == cursor_lim) {

        seek(seek_ctx);

        cursor = BDEV->cursor;
        cursor_lim = BDEV->cursor_lim;

        // reading from "nowhere" gives us a zero.
        if (cursor == cursor_lim) {
            DATA->value = 0x00;
            return;
        }
    }

    DATA->value = *cursor++;
    BDEV->cursor = cursor;
}

static void bdev_wr_DAT(Bdev BDEV)
{
    bdev_invar(BDEV);

    BDEV_ALL(BDEV);

    if (cursor == cursor_lim) {

        seek(seek_ctx);

        cursor = BDEV->cursor;
        cursor_lim = BDEV->cursor_lim;

        // writing to "nowhere" discards the data.
        if (cursor == cursor_lim) {
            return;
        }
    }

    *cursor++ = DATA->value;
    BDEV->cursor = cursor;
}
