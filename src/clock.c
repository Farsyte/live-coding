#include "clock.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "edge.h"
#include "support.h"

// The CLOCK signal itself. Make use of the clock by attaching a
// subsciber to the rise or fall list of this Edge.

Edge                CLOCK;

// Helper macro so nobody has to count zeros, or wonder
// why we have a multiply-by-a-billion somewhere.

#define NS_PER_SEC	1000000000

// Configured clock frequency in integer Hz.

static Tau          clock_freq_hz = 0;

void clock_invar()
{
    edge_invar(CLOCK);
    ASSERT_GT_integer(0, clock_freq_hz);
}

void clock_init(Tau clock_hz)
{
    assert(clock_hz > 0);

    clock_freq_hz = clock_hz;

    EDGE_INIT(CLOCK, 0);
}

// clock_ns_to_tau(ns): exact tau counts for given nanoseconds
// floating point so caller can round up or down as desiredc
double clock_ns_to_tau(double ns)
{
    double              raw = (ns * clock_freq_hz) / NS_PER_SEC;
    // somehow deal with tiny tiny rounding errors:
    // if we are within a very tiny bit of an integer,
    // then round to that integer.
    return raw;
}

// clock_tau_to_ns(tau): exact nanoseconds for given tau counts
// floating point so caller can round up or down as desired
double clock_tau_to_ns(double tau)
{
    return (tau * NS_PER_SEC) / clock_freq_hz;
}

// clock_elapsed_ns(Tau, Tau): compute elapsed time
// Converts the difference in the tau values to nanoseconds using
// the configured clock frequency, rounding DOWN.
//
// For example, for an 18.00 MHz clock, which has a true period of
// 55.555… ns, if start and end are TAU values bracketing a ten
// clock_run_one() calls, this function will return 555 ns (not 550
// and not 556).

Tau clock_elapsed_ns(Tau start, Tau end)
{
    return (end - start) * NS_PER_SEC / (clock_freq_hz);
}

// clock_future_tau(Tau ns): compute TAU for some ns in the future
// Using the clock frequency properly, compute the next TAU value
// that is at or after a point at a given ns in the future.
// The round-up implied here exactly matches the round-down used
// in conversion of delta-TAU values to nanoseconds elapsed.
//
// For example, for an 18.00 MHz clock, which has a true period of
// 55.555… ns, the ending TAU for something 55 ns in the future is
// TAU+1 (one period of the clock), matching the conversion of
// two TAU increments back to 55 ns. The ending TAU for something
// that is 100 ns in the future is TAU+4, as it is rounded up, which
// is actually a span of 111.11… ns.

Tau clock_future_tau(Tau delta_ns)
{
    return TAU + (delta_ns * clock_freq_hz + NS_PER_SEC - 1) / NS_PER_SEC;
}

// clock_run_one(): cycle the clock signal high then low, updating TAU.
//
// Note that this increments TAU by 1.
//
// This is time critical.

void clock_run_one()
{
    TAU++;
    CLOCK->value = 0;
    edge_hi(CLOCK);
}

// clock_run_until(Tau): cycle until the given TAU
//
// Note that as TAU increments by two for each full CLOCK cycle,
// the ending value of TAU may exceed the passed value.
//
// This is time critical.

void clock_run_until(Tau last)
{
    while (TAU < last)
        clock_run_one();
}
