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
    sched_post();
}

static void main_bist()
{
    rtc_bist();
    step_bist();
    subs_bist();
    stepat_bist();
    sched_bist();
}

static void main_bench()
{
    rtc_bench();
    step_bench();
    subs_bench();
    stepat_bench();
    sched_bench();
}

int main(int argc, char **argv)
{

    (void)argc;
    (void)argv;

    main_post();
    main_bist();
    main_bench();
    return 0;
}
