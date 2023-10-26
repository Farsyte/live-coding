#pragma once
#include "simext/support.h"

// hex: parse the content of a hex file.

// HexStoreFn: callback function receiving data from a HEX file.
//
// A function of this type expects to be called with the address of a
// byte of data represented by the hex file (including adjustments for
// segmented or 32-bit linear address spaces), and the byte of data
// for that location.

typedef void HexStoreFn (void *ctx, unsigned addr, unsigned data);

// hex_parse: parse a named file in intel HEX format.
//
// Returns -1 with errno set if unable to open the file.
// Returns -2 if the file ends early.
// Returns -3 if a character should be a hex digit but is not.

extern int          hex_parse(Cstr filename, HexStoreFn *store, void *ctx);
