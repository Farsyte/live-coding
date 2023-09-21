#pragma once
#include "addr.h"
#include "edge.h"
#include "support.h"

// decoder: Address Space Decoder for 8080-like microcomputer system
//
// This module expects to see the control signals from the 8228
// indicating what kind of cycle is present, and may also need to
// see some signals from the 8080 itself.
//
// Timing is similar to the 8228 (but quantized).

#define DEC_MEM_AS_BITS		16
#define DEC_MEM_PO_BITS		10
#define DEC_MEM_PAGES		(1u<<(DEC_MEM_AS_BITS - DEC_MEM_PO_BITS))

#define DEC_DEV_AS_BITS		8
#define DEC_IO_PORTS		(1u<<DEC_DEV_AS_BITS)

typedef struct sDecoder {
    Cstr                name;

    // Address bus (probably owned by the CPU)

    pAddr               ADDR;

    // Control signals from 8228 which has been modified
    // to match falling edges of MEMR and IOR with the rising
    // edge of DBIN.

    pEdge               MEMR_;
    pEdge               MEMW_;
    pEdge               IOR_;
    pEdge               IOW_;

    // MUTABLE control signals for each page of memory and each
    // individual I/O port. Receivers of the falling edge of these
    // signals are expected to either request a wait state (pushing
    // RDYIN low), or complete the operation immediately.
    //
    // Providing data at the rising edge, or sampling the data bus
    // at the rising edge, might be "too late" depending on how
    // other modules operate.

    pEdge               mem_rd[DEC_MEM_PAGES];
    pEdge               mem_wr[DEC_MEM_PAGES];

    pEdge               dev_rd[DEC_IO_PORTS];
    pEdge               dev_wr[DEC_IO_PORTS];

    pEdge               shadow;
}                  *pDecoder, Decoder[1];

// decoder_invar: verify the invariants for a decoder.
// - valid pointer, valid name, not empty name.

extern void         decoder_invar(Decoder);

// decoder_init(s): initialize the given decoder to have this name
// and an initial state.

extern void         decoder_init(Decoder, Cstr name);

// DECODER_INIT(s): initialize the given decoder to have this name
// and an initial state.

#define DECODER_INIT(gen) decoder_init(gen, #gen)

// decoder_linked(s): finish initialization, called after pEdge fields are set.

extern void         decoder_linked(Decoder);

// decoder_shadow(s, e): set up (or turn off) the ROM shadow
//
// operation is normal when the shadow is null, but if it is set to
// point to an edge, then all memory read operations will be sent
// to that edge, rather than the mem_rd[page] edge.

extern void         decoder_shadow(Decoder, pEdge);

// decoder_post: Power-On Self Test for the decoder code
//
// This function should be called every time the program starts
// before any code relies on decoder not being completely broken.

extern void         decoder_post();

// decoder_bist: Power-On Self Test for the decoder code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the decoder code is
// not working correctly.

extern void         decoder_bist();

// decoder_bench: performance verification for the decoder code
//
// This function should be called as needed to measure the performance
// of the time critical parts of decoder.

extern void         decoder_bench();
