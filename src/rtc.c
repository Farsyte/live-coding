#include <errno.h>      // Macros reporting error conditions
#include <string.h>     // String handling
#include <time.h>       // Time/date utilities
#include "common/support.h"

// rtc: real time clock (host system wall-clock elapsed time)

// rtc_prec_ns returns the precision of the real time clock
// as advertised by the host system, in integer nanoseconds.

Tau rtc_prec_ns()
{
    struct timespec     tp[1];
    if (0 > clock_getres(CLOCK_MONOTONIC, tp))
        FAIL("clock_getres(CLOCK_MONOTONIC, tp) failed with errno %d: %s",      // no bist coverage
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
        FAIL("clock_gettime(CLOCK_MONOTONIC, tp) failed with errno %d: %s",     // no bist coverage
             errno, strerror(errno));
    return tp->tv_sec * 1000000000 + tp->tv_nsec;
}
