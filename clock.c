#include "clock.h"
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

    edge_init(CLOCK);
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
    return (end - start) * NS_PER_SEC / (2 * clock_freq_hz);
}

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

Tau clock_future_tau(Tau delta_ns)
{
    return TAU + (delta_ns * 2 * clock_freq_hz + NS_PER_SEC - 1) / NS_PER_SEC;
}

// clock_run_one(): cycle the clock signal high then low, updating TAU.
//
// Note that this increments TAU by 2.
//
// This is time critical.

void clock_run_one()
{
    TAU++;
    edge_hi(CLOCK);
    TAU++;
    edge_lo(CLOCK);
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

// clock_post: Power-On Self Test for the Clock code
//
// This function should be called every time the program starts
// before any code relies on Clock not being completely broken.

void clock_post()
{
    Tau                 t0;

    clock_init(CLOCK_STANDARD_HZ);
    clock_invar();
    ASSERT_EQ_integer(1000, clock_elapsed_ns(100, 136));

    // clock_future_tau takes a nominal number of nanonseconds and
    // computes the number of half-period times, rounding up; so
    // for 18.00 MHz, where half-period is 27.7… ns, asking for 50 ns
    // would return 2 cycles.

    ASSERT_EQ_integer(2, clock_future_tau(50) - TAU);

    // clock_elapsed_ns takes a starting and ending number of
    // half-period times, and returns the number of nanoseconds they
    // span, rounding down. As the above sample, if we ask for the
    // elapsed ns for two half-period times, which is 55.5… ns, then
    // we get 55 ns as the result.

    ASSERT_EQ_integer(55, clock_elapsed_ns(TAU - 2, TAU));
    ASSERT_EQ_integer(555, clock_elapsed_ns(TAU - 20, TAU));
    ASSERT_EQ_integer(5555, clock_elapsed_ns(TAU - 200, TAU));

    // If we take a number of half-period times, convert to ns, then
    // convert back to a future TAU, the change in TAU we get out is
    // the value we put in.

    ASSERT_EQ_integer(2, clock_future_tau(clock_elapsed_ns(TAU - 2, TAU)) - TAU);
    ASSERT_EQ_integer(20, clock_future_tau(clock_elapsed_ns(TAU - 20, TAU)) - TAU);
    ASSERT_EQ_integer(200, clock_future_tau(clock_elapsed_ns(TAU - 200, TAU)) - TAU);

    t0 = TAU;
    clock_run_one();
    ASSERT_EQ_integer(t0 + 2, TAU);

    clock_run_until(t0 + 72);
    ASSERT_EQ_integer(t0 + 72, TAU);

}

// clock_bist: Power-On Self Test for the Clock code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Clock code is
// not working correctly.

void clock_bist()
{
    clock_init(CLOCK_STANDARD_HZ);
    clock_invar();

    Tau                 t0;

    clock_init(CLOCK_STANDARD_HZ);
    clock_invar();
    ASSERT_EQ_integer(1000, clock_elapsed_ns(100, 136));

    // clock_future_tau takes a nominal number of nanonseconds and
    // computes the number of half-period times, rounding up; so
    // for 18.00 MHz, where half-period is 27.7… ns, asking for 50 ns
    // would return 2 cycles.

    ASSERT_EQ_integer(2, clock_future_tau(50) - TAU);

    // clock_elapsed_ns takes a starting and ending number of
    // half-period times, and returns the number of nanoseconds they
    // span, rounding down. As the above sample, if we ask for the
    // elapsed ns for two half-period times, which is 55.5… ns, then
    // we get 55 ns as the result.

    ASSERT_EQ_integer(55, clock_elapsed_ns(TAU - 2, TAU));
    ASSERT_EQ_integer(555, clock_elapsed_ns(TAU - 20, TAU));
    ASSERT_EQ_integer(5555, clock_elapsed_ns(TAU - 200, TAU));

    // If we take a number of half-period times, convert to ns, then
    // convert back to a future TAU, the change in TAU we get out is
    // the value we put in.

    for (Tau hpt = 0; hpt < 1000000; ++hpt)
        ASSERT_EQ_integer(hpt, clock_future_tau(clock_elapsed_ns(TAU - hpt, TAU)) - TAU);

    t0 = TAU;
    clock_run_one();
    ASSERT_EQ_integer(t0 + 2, TAU);

    clock_run_until(t0 + 72);
    ASSERT_EQ_integer(t0 + 72, TAU);

    // NOTE: for this to work, long_run_ns must be a multiple
    // of the actual real period of the clock (not a multiple
    // of the rounded-down period).
    Tau                 long_run_ns = 1000000;
    t0 = TAU;
    clock_run_until(clock_future_tau(long_run_ns));
    ASSERT_EQ_integer(long_run_ns, clock_elapsed_ns(t0, TAU));
}

static Tau          bench_start_tau = 0;
static Tau          bench_final_tau = 0;

static Tau          tick_count[1];
static Tau          tock_count[1];

void bench_clock(void *arg)
{
    (void)arg;

    Tau                 long_run_ns = 10000;
    bench_start_tau = TAU;
    *tick_count = 0;
    *tock_count = 0;
    clock_run_until(clock_future_tau(long_run_ns));
    bench_final_tau = TAU;
}

static void inc(Tau *counter)
{
    ++*counter;
}

// clock_bench: performance verification for the Clock code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Clock.

void clock_bench()
{
    double              dt;
    clock_init(CLOCK_STANDARD_HZ);

    BENCH_TOP("clock");

    dt = RTC_ELAPSED(bench_clock, 0);
    BENCH_VAL(dt * 2.0 / (bench_final_tau - bench_start_tau));

    EDGE_ON_RISE(CLOCK, inc, tick_count);
    dt = RTC_ELAPSED(bench_clock, 0);
    BENCH_VAL(dt / (*tick_count + *tock_count));

    EDGE_ON_FALL(CLOCK, inc, tock_count);
    dt = RTC_ELAPSED(bench_clock, 0);
    BENCH_VAL(dt / (*tick_count + *tock_count));

    while (CLOCK->rise->len < 10)
        EDGE_ON_RISE(CLOCK, inc, tick_count);
    dt = RTC_ELAPSED(bench_clock, 0);
    BENCH_VAL(dt / (*tick_count + *tock_count));

    while (CLOCK->rise->len < 100)
        EDGE_ON_RISE(CLOCK, inc, tick_count);
    dt = RTC_ELAPSED(bench_clock, 0);
    BENCH_VAL(dt / (*tick_count + *tock_count));

    while (CLOCK->fall->len < 100)
        EDGE_ON_FALL(CLOCK, inc, tick_count);
    dt = RTC_ELAPSED(bench_clock, 0);
    BENCH_VAL(dt / (*tick_count + *tock_count));

    BENCH_END();

}
