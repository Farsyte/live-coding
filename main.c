#include "fifo.h"
#include "sched.h"
#include "step.h"
#include "stepat.h"
#include "subs.h"
#include "support.h"

static void main_post()
{
    tau_post();
    rtc_post();
    step_post();
    subs_post();
    stepat_post();
    fifo_post();
    sched_post();
}

static void main_bist()
{
    rtc_bist();
    step_bist();
    subs_bist();
    stepat_bist();
    fifo_bist();
    sched_bist();
}

static void main_bench()
{
    rtc_bench();
    step_bench();
    subs_bench();
    stepat_bench();
    fifo_bench();
    sched_bench();
}

int main(int argc, char **argv)
{
    main_post();

    for (int argi = 1; argi < argc; ++argi) {
        if (!strcmp("bist", argv[argi]))
            main_bist();
        else if (!strcmp("bench", argv[argi]))
            main_bench();
        else
            FAIL("unrecoginzed request: '%s'", argv[argi]);
    }

    return 0;
}
