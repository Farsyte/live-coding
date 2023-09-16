#pragma once
#include "edge.h"

// i8228: System Controller and Bus Driver 8080A CPU
//
// This module expects to see a status strobe, during which time
// the Data bus contains status bits. Based the latched value of
// those status bits, the control signals are asserted at the
// correct time (when enabled).
//
// Timing is similar to the 8228 (but quantized).

typedef struct s8228 {
    Cstr                name;

    pData               DATA;

    pEdge               STSTB_;
    pEdge               DBIN;
    pEdge               WR_;
    pEdge               HLDA;

    Edge                MEMR_;
    Edge                MEMW_;
    Edge                IOR_;
    Edge                IOW_;
    Edge                INTA_;

    Byte                status;
    pEdge               activated;

}                  *p8228, i8228[1];

// i8228_invar: verify the invariants for a i8228.

extern void         i8228_invar(i8228);

// i8228_init(s): initialize the given i8228 to have this name
// and an initial state.

extern void         i8228_init(i8228, Cstr name);

// I8228_INIT(s): initialize the given i8228 to have this name
// and an initial state.

#define I8228_INIT(gen) i8228_init(gen, #gen)

// i8228_linked(s): finish initialization, called after pEdge fields are set.

extern void         i8228_linked(i8228);

// i8228_post: Power-On Self Test for the i8228 code
//
// This function should be called every time the program starts
// before any code relies on i8228 not being completely broken.

extern void         i8228_post();

// i8228_bist: Power-On Self Test for the i8228 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the i8228 code is
// not working correctly.

extern void         i8228_bist();

// i8228_bench: performance verification for the i8228 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of i8228.

extern void         i8228_bench();
