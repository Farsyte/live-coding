#pragma once
#include "simext/support.h"

// timing: check simulated signal timing
//
// This facility verifies that the elapsed time from a start call to a
// final call is within a specified range. The duration of the interval
// is checked when the end of an interval is noted, and the start of the
// interval is present. The timing checker will terminate the run if it
// sees an interval with a bad duration.
//
// A summary can be printed for each checker, and a final check can be
// made that the range of values, converted to nanoseconds, is within the
// originally requested range, as the runtime checks are done in terms of
// TAU counts with limits computed from the requested durations.

typedef struct {
    Cstr                name;

    // lower acceptable bound in nanoseconds
    // set to 0 to not check lower bound.
    Tau                 lb_ns;

    // upper acceptable bound in nanoseconds
    // set to 0 to not check upper bound.
    Tau                 ub_ns;

    // lower acceptable bound in TAU counts
    // set to 0 to not check lower bound.
    // if lb > 0, then timing_check will
    //   ASSERGE_integer(lb, min);
    Tau                 lb;

    // upper acceptable bound in nanoseconds
    // set to 0 to not check upper bound.
    // if ub > 0, then timing_check will
    //   ASSERLE_integer(ub, max);
    Tau                 ub;

    // observed minimum and maximum elapsed times
    // in TAU units, and a count of the number of
    // intervals observed.
    Tau                 min;
    Tau                 max;
    Tau                 count;

    // set to TAU by timing_start
    // set to 0 if not in an interval.
    Tau                 t0;

}                  *pTiming, Timing[1];

// timing_invar(t): verify the invariants for a Timing.

extern void         timing_invar(Timing);

// timing_init(t, name, lb_ns, ub_ns): establish a Timing.
// - name must be given and must not be blank.
// - lb_ns is the lower bound, in nanoseconds, or zero for unchecked.
// - ub_ns is the upper bound, in nanoseconds, or zero for unchecked.

extern void         timing_init(Timing, Cstr name, Tau lb_ns, Tau ub_ns);

// timing_start(t): mark the start of a possible interval.
// This function is timing critical.

extern void         timing_start(Timing);

// timing_cancel(t): cancel the timing interval.
// This function is timing critical.

extern void         timing_cancel(Timing);

// timing_final(t): mark the endof a possible interval.
// if a start was marked and not cancelled, then the checks
// are applied to it.
// This function is timing critical.

extern void         timing_final(Timing);

// timing_print(t): print a summary of the timing.
// prints the observed range of durations of the interval
// in TAU counts and in microseconds (to the nanosecond);
// prints the allowed range, substituting "unchecked" for
// bounds that were set to zero initially.

extern void         timing_print(Timing);

// timing_check(t): final timing check.
// Will throw an assertion failure if the range of durations
// in nanoseconds exceeds the original limit values.
// If this occurs, then the code converting the bounds to TAU
// count units has a bug.

extern void         timing_check(Timing);

// timing_post: Power-On Self Test for the Timing code
//
// This function should be called every time the program starts
// before any code relies on Timing not being completely broken.

extern void         timing_post();

// timing_bist: Power-On Self Test for the Timing code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Timing code is
// not working correctly.

extern void         timing_bist();

// timing_bench: performance verification for the Timing code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Timing.

extern void         timing_bench();
