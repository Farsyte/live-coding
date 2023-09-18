#pragma once
#include "edge.h"
#include "support.h"
#include "target.h"

// ram8107x8x4: 16 KiB memory for 8080-like microcomputer system
//
// This module expects to connect to the Address and Data bus,
// and attach /RD and /WR signals to the decoder, to provide
// 16 KiB of memory to the simulated system.
//
// It is notionally modelled after a memory board making use of eight
// 8107B-4 (4096x1 bit) Dynamic Memory chips wide, by four rows, for a
// total of 16 KiB of memory.
//
// This module does not (yet) model the effects of failure to refresh
// each row of the memory within 

#define RAM8107x8x4_ADDR_BITS	(14)
#define	RAM8107x8x4_SIZE	(1u<<RAM8107x8x4_ADDR_BITS)
#define	RAM8107x8x4_MASK	(RAM8107x8x4_SIZE - 1)

typedef struct sRam8107x8x4 {
    Cstr                name;

    pAddr               ADDR;
    pData               DATA;

    Edge                RD_;
    Edge                WR_;

    Byte                cells[RAM8107x8x4_SIZE];

}                  *pRam8107x8x4, Ram8107x8x4[1];

// ram8107x8x4_invar: verify the invariants for a ram8107x8x4.
// - valid pointer, valid name, not empty name.
// - valid signals attached, in a non-invalid configuration.

extern void         ram8107x8x4_invar(Ram8107x8x4);

// ram8107x8x4_init(s): initialize the given ram8107x8x4 to have this
// name and an initial state.

extern void         ram8107x8x4_init(Ram8107x8x4, Cstr name);

// RAM8107X8X4_INIT(s): initialize the given ram8107x8x4 to have this
// name and an initial state.

#define RAM8107X8X4_INIT(gen) ram8107x8x4_init(gen, #gen)

// ram8107x8x4_linked(s): finish initialization, called after pEdge
// fields are set.

extern void         ram8107x8x4_linked(Ram8107x8x4);

// ram8107x8x4_post: Power-On Self Test for the ram8107x8x4 code
//
// This function should be called every time the program starts
// before any code relies on ram8107x8x4 not being completely broken.

extern void         ram8107x8x4_post();

// ram8107x8x4_bist: Power-On Self Test for the ram8107x8x4 code

//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the ram8107x8x4 code is
// not working correctly.

extern void         ram8107x8x4_bist();

// ram8107x8x4_bench: performance verification for the ram8107x8x4 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of ram8107x8x4.

extern void         ram8107x8x4_bench();
