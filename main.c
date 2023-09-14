#include "clock.h"
#include "edge.h"
#include "fifo.h"
#include "sched.h"
#include "step.h"
#include "stepat.h"
#include "subs.h"
#include "support.h"

static int main_post()
{
    tau_post();
    rtc_post();
    step_post();
    stepat_post();
    subs_post();
    fifo_post();
    sched_post();
    edge_post();
    clock_post();
    return 0;
}

static int main_bist(int argc, char **argv)
{
    (void)argc;         // not used to select BIST methods (yet)
    (void)argv;         // not used to select BIST methods (yet)

    rtc_bist();
    step_bist();
    stepat_bist();
    subs_bist();
    fifo_bist();
    sched_bist();
    edge_bist();
    clock_bist();
    return 0;
}

static int main_bench(int argc, char **argv)
{
    (void)argc;         // not used to select BENCH methods (yet)
    (void)argv;         // not used to select BENCH methods (yet)

    rtc_bench();
    step_bench();
    stepat_bench();
    subs_bench();
    fifo_bench();
    sched_bench();
    edge_bench();
    clock_bench();
    return 0;
}

int main(int argc, char **argv)
{
    main_post();

    for (int argi = 1; argi < argc; ++argi) {
        if (!strcmp("bist", argv[argi]))
            return main_bist(argc - argi, argv + argi);
        if (!strcmp("bench", argv[argi]))
            return main_bench(argc - argi, argv + argi);

        FAIL("unrecognized request: '%s'", argv[argi]);
    }

    return 0;
}
