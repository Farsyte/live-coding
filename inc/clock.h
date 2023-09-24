#pragma once
#include "edge.h"
#include "support.h"

// clock: provide an Edge that oscillates
//
// This facility provides an Edge, a function that cause it to oscillate
// between high and low, and functions that relate cycles of this CLOCK
// to the simulation time.

// The most common clock speed for the initial 8080 based simulation
// is expected to be 18.00 MHz, giving an 8080 T-state time of 500 ns
// and a 2.0 μs minimum instruction time.

#define CLOCK_STANDARD_HZ	(18000000)

// The CLOCK signal itself. Make use of the clock by attaching a
// subsciber to the rise or fall list of this Edge.

extern Edge         CLOCK;

// clock_invar(): Check invariants for the Clock.

extern void         clock_invar();

// clock_init(Tau): Initialize the Clock to run at this frequency.
// The frequency is an integer number of cycles per second.

extern void         clock_init(Tau clock_Hz);

// clock_ns_to_tau(ns): exact tau counts for given nanoseconds
// floating point so caller can round up or down as desiredc
extern double       clock_ns_to_tau(double ns);

// clock_tau_to_ns(tau): exact nanoseconds for given tau counts
// floating point so caller can round up or down as desired
extern double       clock_tau_to_ns(double tau);

// clock_elapsed_ns(Tau, Tau): compute elapsed time
// Converts the difference in the tau values to nanoseconds using
// the configured clock frequency, rounding DOWN.
//
// For example, for an 18.00 MHz clock, which has a true period of
// 55.555… ns, if start and end are TAU values bracketing a ten
// clock_run_one() calls, this function will return 555 ns (not 550
// and not 556).

extern Tau          clock_elapsed_ns(Tau start, Tau end);

// clock_future_tau(Tau ns): compute TAU for some ns in the future
// Using the clock frequency properly, compute the next TAU value
// that is at or after a point at a given ns in the future.
// The round-up implied here exactly matches the round-down used
// in conversion of delta-TAU values to nanoseconds elapsed.
//
// For example, for an 18.00 MHz clock, which has a true period of
// 55.555… ns, the ending TAU for something 55 ns in the future is
// TAU+2 (two half-periods of the clock), matching the conversion of
// two TAU increments back to 55 ns. The ending TAU for something
// that is 100 ns in the future is TAU+4, as it is rounded up, which
// is actually a span of 111.11… ns.

extern Tau          clock_future_tau(Tau delta_ns);

// clock_run_one(): cycle the clock signal high then low, updating TAU.
//
// Note that this increments TAU by 2.
//
// This is time critical.

extern void         clock_run_one();

// clock_run_until(Tau): cycle until the given TAU
//
// Note that as TAU increments by two for each full CLOCK cycle,
// the ending value of TAU may exceed the passed value.
//
// This is time critical.

extern void         clock_run_until(Tau last);

// clock_post: Power-On Self Test for the Clock code
//
// This function should be called every time the program starts
// before any code relies on Clock not being completely broken.

extern void         clock_post();

// clock_bist: Power-On Self Test for the Clock code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Clock code is
// not working correctly.

extern void         clock_bist();

// clock_bench: performance verification for the Clock code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Clock.

extern void         clock_bench();
