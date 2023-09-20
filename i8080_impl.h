#pragma once
#include "i8080.h"

// i8080_impl: shared between parts of the 8080 implementation
//
// The simulation of the 8080 is divided out to a number of
// separate files, containing somewhat independent sections
// of the module which communicate through the internal state
// of the i8080 structure.

// Initialization functions for the sub-modules
// to be called after the CPU is marked as linked:

extern void         i8080_reset_init(i8080 cpu);
extern void         i8080_fetch_init(i8080 cpu);
extern void         i8080_eidihlt_init(i8080 cpu);

// Activity during a T-state is managed by a single
// function, which is called three times.
// - Once in response to Phi_1 rising
// - Once in response to Phi_2 rising
// - Once in response to Phi_3 rising
// Additional calls may be added in the future.
// The value passed is currently set to one of these
// three discrete values.
// The initial notion is that the T-state can be
// viewed as nine oscillator periods; if they are
// called 1 through 9, the values here are the periods
// we are entering at that edge.

#define	PHI1_RISE	1
#define	PHI2_RISE	3
#define	PHI2_FALL	7

// 8080 OP codes

#define	OP_NOP		(0000)
#define	OP_HLT		(0166)
#define	OP_DI		(0363)
#define	OP_EI		(0373)
