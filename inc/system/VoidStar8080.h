#pragma once
#include "common/bdev.h"
#include "common/cdev.h"
#include "chip/decoder.h"
#include "chip/i8080.h"
#include "chip/i8224.h"
#include "chip/i8228.h"
#include "chip/ram8107x8x4.h"
#include "chip/rom8316.h"
#include "common/support.h"

typedef struct sVoidStar8080 {
    Cstr                name;

    i8080               cpu;
    i8224               gen;
    i8228               ctl;
    Decoder             dec;

    Cdev                tty;
    Cdev                crt;
    Cdev                lpt;

    Bdev                bdev;
    struct sBctx       *bctx;

    // each Rom8316 represents 2 KiB of memory.
    // rom[0] goes in the top 2 KiB (at F800H)
    // rom[1] goes in the next 2 KiB (at F000H)
    // and so on down.
#define ROM_CHIPS	1
    Rom8316             rom[ROM_CHIPS];

    // each Ram8107x8x4 represents 16 KiB of memory.
    // ram[0] goes in the bottom 16 KiB of memorty (at 0000H)
    // ram[1] goes in the next 16 KiB of memorty (at 4000H)
    // and so on, if we had more than two.
#define RAM_BOARDS	4
    Ram8107x8x4         ram[RAM_BOARDS];

    Edge                RESIN_;                 // owned by system
    Edge                RDYIN;                  // owned by system

}                  *pVoidStar8080, VoidStar8080[1];

extern int          VoidStar8080_main(int argc, char **argv);

extern void         VoidStar8080_init(VoidStar8080, Cstr name);
extern void         VoidStar8080_invar(VoidStar8080);
extern void         VoidStar8080_linked(VoidStar8080);

#define VOIDSTAR8080_INIT(sys)	VoidStar8080_init(sys, #sys)
