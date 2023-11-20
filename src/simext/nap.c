#include "simext/nap.h"
#include <sys/select.h>
#include <unistd.h>
#include "simext/support.h"

Tau                 nap_total = 0;
Tau                 nap_pend = 0;

void nap_init()
{
    nap_pend = 0;
    nap_total = 0;
}

//           struct timeval {
//               time_t      tv_sec;         /* seconds */
//               suseconds_t tv_usec;        /* microseconds */
//           };

void nap()
{
    nap_pend += 1000000;

    Tau                 nap_us = nap_pend / 1000;
    if (nap_us < 1)
        return;

    Tau                 t0 = rtc_ns();
    usleep(nap_us);
    Tau                 dt = rtc_ns() - t0;

    nap_pend -= dt;
    nap_total += dt;
}
