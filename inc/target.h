#pragma once
#include "headers.h"

// Definitions for data types to use in the current simulation target,
// some kind of 8080 based microcomputer that would have been sane in
// the late 80s through early 90s.

typedef uint8_t    *pBit, Bit;
typedef uint8_t    *pByte, Byte;
typedef uint16_t   *pWord, Word;

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

// Position of flag bits in PSW when pushed and popped

#define FLAGS_CY	0x01
#define FLAGS_P		0x04
#define FLAGS_AC	0x10
#define FLAGS_Z		0x40
#define FLAGS_S		0x80
