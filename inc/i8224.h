#pragma once
#include "edge.h"
#include "support.h"

// i8224: Clock Generator and Driver for 8080A CPU
//
// This module expects to see rising edges on its OSC input. Based on
// these edges, it constructs a two-phase clock; it synchronizes RESET
// and READY to a specific phase of the clock; and generates /STSTB, when
// SYNC is active, with appropriate timing.

typedef struct s8224 {
    Cstr                name;

    pEdge               OSC;
    pEdge               SYNC;
    pEdge               RESIN_;
    pEdge               RDYIN;

    Edge                PHI1;
    Edge                PHI2;
    Edge                STSTB_;
    Edge                RESET;
    Edge                READY;

    int                 state;

}                  *p8224, i8224[1];

// i8224_invar: verify the invariants for a i8224.

extern void         i8224_invar(i8224);

// i8224_init(s): initialize the given i8224 to have this name
// and an initial state.

extern void         i8224_init(i8224, Cstr name);

// I8224_INIT(s): initialize the given i8224 to have this name
// and an initial state.

#define I8224_INIT(gen) i8224_init(gen, #gen)

// i8224_linked(s): finish initialization, called after pEdge fields are set.

extern void         i8224_linked(i8224);

// i8224_post: Power-On Self Test for the i8224 code
//
// This function should be called every time the program starts
// before any code relies on i8224 not being completely broken.

extern void         i8224_post();

// i8224_bist: Power-On Self Test for the i8224 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the i8224 code is
// not working correctly.

extern void         i8224_bist();

// i8224_bench: performance verification for the i8224 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of i8224.

extern void         i8224_bench();
