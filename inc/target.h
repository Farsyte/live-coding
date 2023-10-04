#pragma once
#include <stdint.h>     // Fixed-width integer types

// Definitions for data types to use in the current simulation target.
//
// Currently configured to be similar to a typical 8080 based
// microcomputer that would have been sane in the late 80s through
// early 90s.

typedef uint8_t    *pBit, Bit;
typedef uint8_t    *pByte, Byte;
typedef uint16_t   *pWord, Word;
