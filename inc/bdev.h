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

// To read a sector:
//
// - secrd:
//   - DSK := disk number
//   - TRK := track number
//   - SEC := sector number
// - secrd1:
//   - *(HL)++ = DAT
//   - back to secrd1 until all data moved
// - done:
//
// Reading data after the last byte of a sector will get you the first
// byte of the next sector on the track. Reading data after the last
// byte of a track will get you the first byte of the first sector on
// the next track. Reading data after the last byte of the last track
// will get you TBD.
//
// This is done by selecting the next sector (which may be the first
// sector of the next track) in the SEC and TRK registers when reading
// the first byte of the sector. Due to this behavior, always set TRK
// when setting SEC.

// To write a sector:
//
// - secwr:
//   - DSK := disk number
//   - TRK := track number
//   - SEC := sector number
//   - CTRL := BDEV_CTRL_SEEK
// - secwr1:
//   - DAT := *(HL)++
//   - back to secwr1 until all data moved
// - error:
//   - report error to console
// - done:
//
// Writing data after the last byte of a sector will write to the
// first byte of the next sector on the track. Writing data after the
// last byte of a track will write to the first byte of the first
// sector on the next track. Writing data after the last byte of the
// last track will discard the data.
//
// This is done by selecting the next sector (which may be the first
// sector of the next track) in the SEC and TRK registers when writing
// the first byte of the sector. Due to this behavior, always set TRK
// when setting SEC.

typedef void bdev_seek_fn (void *ctx);

typedef struct sBdev {
    Cstr                name;

    // falling edge of RESET_ resets the controller.
    // TODO reset should reset controller state to:
    //   DSK=0 TRK=0 SEC=1
    //   cursor = NULL
    //   cursor_lim = NULL
    Edge                RESET_;

    Data                DSK;                    // drive number, 0..Ndisk-1
    Edge                DSK_RD_;
    Edge                DSK_WR_;

    Data                TRK;                    // track number, 00..Ntrk-1
    Edge                TRK_RD_;
    Edge                TRK_WR_;

    Data                SEC;                    // sector number, 01..Nsec
    Edge                SEC_RD_;
    Edge                SEC_WR_;

    Edge                DAT_RD_;
    Edge                DAT_WR_;

    pData               DATA;                   // system data bus

    bdev_seek_fn       *seek;
    void               *seek_ctx;
    Byte               *cursor;
    Byte               *cursor_lim;

}                  *pBdev, Bdev[1];

#define BDEV_D(BDEV,REG)	pData REG = BDEV->REG; (void) REG
#define BDEV_E(BDEV,SIG)	pEdge SIG = BDEV->SIG; (void) SIG

#define BDEV_ALL(BDEV)                                                  \
    BDEV_E(BDEV,RESET_);                                                \
                                                                        \
    BDEV_D(BDEV,DSK);                                                   \
    BDEV_E(BDEV,DSK_RD_);                                               \
    BDEV_E(BDEV,DSK_WR_);                                               \
                                                                        \
    BDEV_D(BDEV,TRK);                                                   \
    BDEV_E(BDEV,TRK_RD_);                                               \
    BDEV_E(BDEV,TRK_WR_);                                               \
                                                                        \
    BDEV_D(BDEV,SEC);                                                   \
    BDEV_E(BDEV,SEC_RD_);                                               \
    BDEV_E(BDEV,SEC_WR_);                                               \
                                                                        \
    BDEV_E(BDEV,DAT_RD_);                                               \
    BDEV_E(BDEV,DAT_WR_);                                               \
                                                                        \
    BDEV_D(BDEV,DATA);                                                  \
                                                                        \
    bdev_seek_fn *seek = BDEV->seek; (void) seek;                       \
    void *seek_ctx = BDEV->seek_ctx; (void) seek_ctx;                   \
    Byte *cursor = BDEV->cursor; (void) cursor;                         \
    Byte *cursor_lim = BDEV->cursor_lim; (void) cursor_lim

extern void         bdev_init(Bdev, Cstr name);
extern void         bdev_set_seek(Bdev, bdev_seek_fn *fn, void *ctx);
extern void         bdev_invar(Bdev);

#define BDEV_SET_SEEK(d,fn,ctx) bdev_set_seek(d, (bdev_seek_fn *)(fn), (void *)ctx);

extern void         bdev_linked(Bdev);
extern void         bdev_close(Bdev);

extern void         bdev_bist();
