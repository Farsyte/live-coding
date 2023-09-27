#pragma once
#include "addr.h"
#include "data.h"
#include "edge.h"

// i8080: Single Chip 8-bit Microprocessor
//
// This module simulates the behavior of the 8080 CPU, matching
// the external signals (within simulation quantization) and modeling
// some but not all known internal state.

// forward declare s8080, so we can forward declare
// the type of our functions that manage each T-state.
struct s8080;

// T-state management functions are given the cpu and the phase.

typedef void        f8080State(struct s8080 *cpu, int phase);
typedef f8080State *p8080State;

typedef struct s8080 {
    Cstr                name;

    pEdge               PHI1;                   // owned by i8224
    pEdge               PHI2;                   // owned by i8224

    Addr                ADDR;                   // output from 8080 except during DMA
    Data                DATA;                   // in while DBIN; out while WR_

    Edge                SYNC;                   // output from 8080
    Edge                DBIN;                   // output from 8080
    Edge                WR_;                    // output from 8080

    pEdge               READY;                  // owned by i8224
    Edge                WAIT;                   // output from 8080
    Edge                HOLD;                   // input to 8080
    Edge                HLDA;                   // output from 8080

    Edge                INT;                    // input to 8080
    Edge                INTE;                   // output from 8080

    pEdge               RESET;                  // owned by i8224

    // programmer visible state
    Addr                PC;                     // program counter
    Addr                SP;                     // stack pointer

    Data                A;

    Data                B;
    Data                C;
    Data                D;
    Data                E;
    Data                H;
    Data                L;

    // internal architectural state

    Data                IR;                     // instruction register
    Data                TMP;                    // TEMP REG feeding ALU upper operand
    Data                ACT;                    // Accumulator Latch for ALU operations
    Data                ALU;                    // Result of ALU operation

    Addr                IDAL;                   // Incrementer/Decrementer Address Latch

    Data                FLAGS;                  // flag bits placed for push/pop PSW

    Edge                RESET_INT;              // officially "INTERNAL RESET"
    Edge                RETM1_INT;              // officially "RETURN M1"
    Edge                INH_PC_INC;             // officially "INHIBIT STORE OF PC+1"

    // T-state management functions
    p8080State          state_reset;            // entry T-state for reset
    p8080State          state_fetch;            // entry T-state for STATUS_FETCH cycles
    p8080State          state_2bops;            // T4 handler for 2-byte ops
    p8080State          state_2bops_t1;         // T1 handler for 2-byte ops

    p8080State          state;                  // function for the current T-state
    p8080State          state_next;             // function for the next T-state
    p8080State          state_m1t1;             // function for the next T1 state

    // opcode decode table giving the T-state function to use
    // for M1 T2, last bit of work for prior instruction.
    p8080State          m1t2[256];

    // opcode decode table giving the T-state function to use
    // for M1 T4 (op decode is in M1 T3).
    p8080State          m1t4[256];

    // in some cases, the m1t4 needs to decode IR again
    // to pick the m1t5.
    p8080State          m1t5[256];

    // opcode decode table giving the T-state function to use
    // for M2 T3 (picked during M2 T1)
    p8080State          m2t3[256];

}                  *p8080, i8080[1];

// i8080_invar: verify the invariants for a i8080.

extern void         i8080_invar(i8080);

// i8080_init(s): initialize the given i8080 to have this name
// and an initial state.

extern void         i8080_init(i8080, Cstr name);

// I8080_INIT(s): initialize the given i8080 to have this name
// and an initial state.

#define I8080_INIT(gen) i8080_init(gen, #gen)

// i8080_linked(s): finish initialization, called after pEdge fields are set.

extern void         i8080_linked(i8080);

// i8080_post: Power-On Self Test for the i8080 code
//
// This function should be called every time the program starts
// before any code relies on i8080 not being completely broken.

extern void         i8080_post();

// i8080_bist: Power-On Self Test for the i8080 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the i8080 code is
// not working correctly.

extern void         i8080_bist();

// i8080_bench: performance verification for the i8080 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of i8080.

extern void         i8080_bench();
