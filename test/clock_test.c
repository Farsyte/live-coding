#include "common/clock.h"
#include "common/edge.h"
#include "simext/support.h"

// clock_post: Power-On Self Test for the Clock code
//
// This function should be called every time the program starts
// before any code relies on Clock not being completely broken.

void clock_post()
{
    Tau                 t0;

    clock_init(CLOCK_STANDARD_HZ);
    clock_invar();
    ASSERT_EQ_integer(1000, clock_elapsed_ns(100, 118));

    // clock_future_tau takes a nominal number of nanonseconds and
    // computes the number of periods, rounding up; so for 18.00 MHz,
    // where period is 55.5… ns, asking for 50 ns would return 2
    // cycles.

    ASSERT_EQ_integer(1, clock_future_tau(50) - TAU);

    // clock_elapsed_ns takes a starting and ending number of
    // period counts, and returns the number of nanoseconds they
    // span, rounding down. As the above sample, if we ask for the
    // elapsed ns for two period times, which is 111.1… ns, then
    // we get 111 ns as the result.

    ASSERT_EQ_integer(111, clock_elapsed_ns(TAU - 2, TAU));
    ASSERT_EQ_integer(1111, clock_elapsed_ns(TAU - 20, TAU));
    ASSERT_EQ_integer(11111, clock_elapsed_ns(TAU - 200, TAU));

    // If we take a number of half-period times, convert to ns, then
    // convert back to a future TAU, the change in TAU we get out is
    // the value we put in.

    ASSERT_EQ_integer(2, clock_future_tau(clock_elapsed_ns(TAU - 2, TAU)) - TAU);
    ASSERT_EQ_integer(20, clock_future_tau(clock_elapsed_ns(TAU - 20, TAU)) - TAU);
    ASSERT_EQ_integer(200, clock_future_tau(clock_elapsed_ns(TAU - 200, TAU)) - TAU);

    t0 = TAU;
    clock_run_one();
    ASSERT_EQ_integer(t0 + 1, TAU);

    clock_run_until(t0 + 36);
    ASSERT_EQ_integer(t0 + 36, TAU);

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
    ASSERT_EQ_integer(1000, clock_elapsed_ns(100, 118));

    // clock_future_tau takes a nominal number of nanonseconds and
    // computes the number of half-period times, rounding up; so
    // for 18.00 MHz, where half-period is 55.5… ns, asking for 50 ns
    // would return 2 cycles.

    ASSERT_EQ_integer(1, clock_future_tau(50) - TAU);

    // clock_elapsed_ns takes a starting and ending number of
    // half-period times, and returns the number of nanoseconds they
    // span, rounding down. As the above sample, if we ask for the
    // elapsed ns for two half-period times, which is 55.5… ns, then
    // we get 55 ns as the result.

    ASSERT_EQ_integer(55, clock_elapsed_ns(TAU - 1, TAU));
    ASSERT_EQ_integer(555, clock_elapsed_ns(TAU - 10, TAU));
    ASSERT_EQ_integer(5555, clock_elapsed_ns(TAU - 100, TAU));

    // If we take a number of half-period times, convert to ns, then
    // convert back to a future TAU, the change in TAU we get out is
    // the value we put in.

    for (Tau hpt = 0; hpt < 1000000; ++hpt)
        ASSERT_EQ_integer(hpt, clock_future_tau(clock_elapsed_ns(TAU - hpt, TAU)) - TAU);

    t0 = TAU;
    clock_run_one();
    ASSERT_EQ_integer(t0 + 1, TAU);

    clock_run_until(t0 + 36);
    ASSERT_EQ_integer(t0 + 36, TAU);

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
    BENCH_VAL(dt * 1.0 / (bench_final_tau - bench_start_tau));

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
