#pragma once
#include "data.h"
#include "edge.h"
#include "support.h"
#include "target.h"

// Block Device does the following:
// - Read a block of data from storage to memory
// - Write a block of data from memory to storage
//
// To support this, we need a lot of data
// - control/status register
// - DMA base address (2 bytes)
// - which disk (1 byte)
// - which track (1 byte)
// - which sector (1 byte)

// Control Byte Values:
#define BDEV_CTRL_NOP	0x00
#define BDEV_CTRL_RD	0x01
#define BDEV_CTRL_WR	0x02
// Behavior is UNDEFINED if any other value is stored to CTRL.

// Status Byte Values:
#define BDEV_STAT_IDLE	0x00
#define BDEV_STAT_BUSY	0xFF

#define BDEV_DSK_MIN	0
#define BDEV_DSK_MAX	25
// TRK: 1 is first track
// SEC: 0 is first sector
//   NOTE: the CP/M 1.4 manual shows a diskette allocation
//   showing Track 1 sector 1 follows Track 0 sector 26,
//   so it is likely that any disk image files I have on hand
//   will be 26 sectors per track.

// bdev state machine states
#define	BDEV_STATE_IDLE	0
#define	BDEV_STATE_RD	1
#define	BDEV_STATE_WR	2

typedef void bdev_impl_fn (void *bdev, void *ctx);

typedef struct sBdev {
    Cstr                name;

    Data                DMAH;
    Data                DMAL;

    Data                DSK;
    Data                TRK;
    Data                SEC;

    Edge                rd[6];                  // read strobes, active low
    Edge                wr[6];                  // write strobes, active low

    pData               DATA;                   // system data bus

    // storage for the mass storage state machine

    int                 requested_op;
    int                 current_state;

    void               *impl_ctx;
    bdev_impl_fn       *impl_fn;

}                  *pBdev, Bdev[1];

extern void         bdev_init(Bdev, Cstr name);
extern void         bdev_set_impl(Bdev, bdev_impl_fn *fn, void *ctx);

#define BDEV_SET_IMPL(d,fn,ctx) bdev_set_impl(d, (bdev_impl_fn *)(fn), (void *)ctx);

extern void         bdev_linked(Bdev);
extern void         bdev_close(Bdev);

extern void         bdev_bist();
