#pragma once

// === === === === === === === === === === === === === === === ===
// T-state phase values
// === === === === === === === === === === === === === === === ===

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
