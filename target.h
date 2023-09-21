#pragma once
#include "headers.h"

// Definitions for data types to use in the current simulation target,
// some kind of 8080 based microcomputer that would have been sane in
// the late 80s through early 90s.

typedef uint8_t    *pBit, Bit;
typedef uint8_t    *pByte, Byte;
typedef uint16_t   *pWord, Word;

// Even with --std=gnu99, asking for -Wpedantic causes any use
// of these macros to generate
//     warning: binary constants are a C2X feature or GCC extension
//
// and all discussions I can find on how to eliminate that warning
// come down to "stop using that extension" because the answering
// idiot thinks that, just because they don't see value in the binary
// constant, nobody else should want to use it.
//
// sigh.
//
// To use this, I would have to bracket all USES of these macros
// with
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wpedantic"
// (use STATUS_whatever)
// #pragma GCC diagnostic pop

// Position of each status bit in the status byte
#define STATUS_INTA     0x01    // Interrupt Acknowledge
#define STATUS_WO       0x02    // Write Output (active low)
#define STATUS_STACK    0x04    // Stack Memory Access
#define STATUS_HLTA     0x08    // Halt Acknowledge
#define STATUS_OUT      0x10    // Output Write
#define STATUS_M1       0x20    // Instruction Fetch (incl INTA)
#define STATUS_INP      0x40    // Input Read
#define STATUS_MEMR     0x80    // Memory Read

// Aggregate Status Byte values for each machine cycle type
#define STATUS_RESET    0x02    // [     ]  (0) can be used during RESET
#define STATUS_FETCH    0xA2    // [/MEMR]  (1) MEMR M1
#define STATUS_MREAD    0x82    // [/MEMR]  (2) MEMR
#define STATUS_MWRITE   0x00    // [/MEMW]  (3)              /WO
#define STATUS_SREAD    0x86    // [/MEMR]  (4) MEMR STACK
#define STATUS_SWRITE   0x04    // [/MEMW]  (5)      STACK   /WO
#define STATUS_INPUTRD  0x42    // [/IOR ]  (6) INP
#define STATUS_OUTPUTWR 0x10    // [/IOW ]  (7) OUT          /WO
#define STATUS_INTACK   0x23    // [/INTA]  (8)      M1        INTA
#define STATUS_HALTACK  0x8A    // [     ]  (9) MEMR    HLTA
#define STATUS_INTACKW  0x2B    // [/INTA]  (10)     M1 HLTA   INTA
