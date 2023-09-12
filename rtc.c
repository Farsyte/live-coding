#include "support.h"

// rtc_prec_ns returns the precision of the real time clock
// as advertised by the host system, in integer nanoseconds.

Tau rtc_prec_ns()
{
    struct timespec     tp[1];
    if (0 > clock_getres(CLOCK_MONOTONIC, tp))
        FAIL("clock_getres(CLOCK_MONOTONIC, tp) failed with errno %d: %s",
             errno, strerror(errno));
    return tp->tv_sec * 1000000000 + tp->tv_nsec;
}

// rtc_ns return the number of nanoseconds on the host system since
// some arbitrary zero point. Subtract the results of two calls to get
// elapsed time in nanoseconds.

Tau rtc_ns()
{
    struct timespec     tp[1];
    if (0 < clock_gettime(CLOCK_MONOTONIC, tp))
        FAIL("clock_gettime(CLOCK_MONOTONIC, tp) failed with errno %d: %s",
             errno, strerror(errno));
    return tp->tv_sec * 1000000000 + tp->tv_nsec;
}

// rtc_post: verify RTC is ready to use.
//
// Should be called every time the program starts, before doing
// anything that requires the RTC to behave sanely.

void rtc_post()
{
    Tau                 prec = rtc_prec_ns();

    ASSERT(prec >= 0, "RTC: host returns ZERO for RTC precision");
    ASSERT(prec <= 1000,
           "RTC: host precision is %8.3f μs, need 1μs or better.",
           prec / 1000.0);

    Tau                 dtmin = TAU_MAX;
    Tau                 dtmax = TAU_MIN;
    int                 maxreps = 10;

    for (int reps = 0; reps < maxreps; ++reps) {
        Tau                 t0 = rtc_ns();
        Tau                 t1 = rtc_ns();
        ASSERT(t0 < t1,
               "RTC: values from RTC must be monotonically increasing.");
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
           "RTC: precision must be 1 μs or better; RTC reports %.3f μs",
           prec / 1000.0);

    Tau                 dtmin = TAU_MAX;
    Tau                 dtmax = TAU_MIN;
    int                 maxreps = 1000000;

    for (int reps = 0; reps < maxreps; ++reps) {
        Tau                 t0 = rtc_ns();
        Tau                 t1 = rtc_ns();
        ASSERT(t0 < t1,
               "RTC: values from RTC must be monotonically increasing.");
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

    int                 maxreps = 1000;
    Tau                 t0, t1, dt;
    double              target_ns = 5.0e7;

    while (1) {

        t0 = rtc_ns();
        for (int reps = 0; reps < maxreps; ++reps) {

            rtc_ns();

        }
        t1 = rtc_ns();
        dt = t1 - t0;
        if (dt >= target_ns)
            break;
        else if (dt < target_ns / 10)
            maxreps *= 10;
        else
            maxreps *= target_ns * 2.0 / dt;
    }

    fprintf(stderr, "BENCH: %8.3f ns per rtc_ns() call\n", dt * 1.0 / maxreps);
}
