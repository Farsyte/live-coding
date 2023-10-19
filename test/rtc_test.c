#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "common/support.h"

// rtc_post: verify RTC is ready to use.
//
// Should be called every time the program starts, before doing
// anything that requires the RTC to behave sanely.

void rtc_post()
{
    Tau                 prec = rtc_prec_ns();

    ASSERT(prec >= 0, "RTC: host returns ZERO for RTC precision");
    ASSERT(prec <= 1000,
           "RTC: host precision is %8.3f μs, need 1μs or better.", prec / 1000.0);

    Tau                 dtmin = TAU_MAX;
    Tau                 dtmax = TAU_MIN;
    int                 maxreps = 10;

    for (int reps = 0; reps < maxreps; ++reps) {
        Tau                 t0 = rtc_ns();
        Tau                 t1 = rtc_ns();
        ASSERT(t0 < t1, "RTC: values from RTC must be monotonically increasing.");
        Tau                 dt = t1 - t0;
        if (dtmin > dt)
            dtmin = dt;
        if (dtmax < dt)
            dtmax = dt;
    }

    ASSERT(dtmin <= 1000, "RTC: minimum Δτ should be 1 μs or less.");
    ASSERT(dtmax <= 1000000, "RTC: maximum Δτ should be 1 ms or less.");
}

// rtc_bist: test the RTC facility.
//
// Need not be called every time the program starts, but should be
// called on every successful build to assure that the RTC facility is
// fundamentally working.

void rtc_bist()
{
    Tau                 prec = rtc_prec_ns();

    ASSERT(prec > 0, "RTC: precision must be a positive number of ns");
    ASSERT(prec <= 1000,
           "RTC: precision must be 1 μs or better; RTC reports %.3f μs", prec / 1000.0);

    Tau                 dtmin = TAU_MAX;
    Tau                 dtmax = TAU_MIN;
    int                 maxreps = 1000000;

    for (int reps = 0; reps < maxreps; ++reps) {
        Tau                 t0 = rtc_ns();
        Tau                 t1 = rtc_ns();
        ASSERT(t0 < t1, "RTC: values from RTC must be monotonically increasing.");
        Tau                 dt = t1 - t0;
        if (dtmin > dt)
            dtmin = dt;
        if (dtmax < dt)
            dtmax = dt;
    }

    ASSERT(dtmin <= 1000, "RTC: minimum Δτ should be 1 μs or less.");
    ASSERT(dtmax <= 1000000, "RTC: maximum Δτ should be 1 ms or less.");
}

// rtc_bench: benchmark the RTC facility.

void rtc_bench()
{
    Tau                 maxreps = 100;
    Tau                 t0, t1, dt;
    double              target_ns = 1.0e7;

    while (1) {

        t0 = rtc_ns();
        for (Tau reps = 0; reps < maxreps; ++reps)
            (void)rtc_ns();
        t1 = rtc_ns();
        assert(t0 > 0);
        assert(t1 > t0);

        dt = t1 - t0;
        if (dt >= target_ns)
            break;
        else if (dt < target_ns / 10)
            maxreps *= 10;
        else
            maxreps *= target_ns * 2.0 / dt;

        assert(maxreps > 0);
    }

    double              avg = dt * 1.0 / maxreps;

    BENCH_TOP("rtc");
    BENCH_VAL(avg);
    BENCH_END();
}

double rtc_elapsed(void (*func)(void *), void *arg)
{
    Tau                 maxreps = 100;
    Tau                 t0, t1, dt;
    double              target_ns = 1.0e7;

    while (1) {

        t0 = rtc_ns();
        for (Tau reps = 0; reps < maxreps; ++reps)
            func(arg);
        t1 = rtc_ns();

        dt = t1 - t0;
        if (dt >= target_ns)
            break;
        else if (dt < target_ns / 10)
            maxreps *= 10;
        else
            maxreps *= target_ns * 2.0 / dt;

        assert(maxreps > 0);
    }

    return dt * 1.0 / maxreps;
}
