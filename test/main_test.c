#include "chip/decoder.h"
#include "chip/i8080.h"
#include "chip/i8224.h"
#include "chip/i8228.h"
#include "chip/ram8107x8x4.h"
#include "chip/rom8316.h"
#include "common/bdev.h"
#include "common/bring.h"
#include "common/cdev.h"
#include "common/clock.h"
#include "common/data.h"
#include "common/edge.h"
#include "common/fifo.h"
#include "common/main.h"
#include "common/sched.h"
#include "common/step.h"
#include "common/stepat.h"
#include "common/subs.h"
#include "common/timing.h"
#include "simext/nap.h"
#include "wa16/addr.h"

int main_post()
{
    tau_post();
    bring_post();
    rtc_post();
    nap_post();
    step_post();
    stepat_post();
    subs_post();
    fifo_post();
    sched_post();
    edge_post();
    addr_post();
    data_post();
    clock_post();
    timing_post();
    i8224_post();
    i8228_post();
    decoder_post();
    ram8107x8x4_post();
    rom8316_post();
    i8080_post();
    return 0;
}

int main_bist(int argc, char **argv)
{
    (void)argc;         // not used to select BIST methods (yet)
    (void)argv;         // not used to select BIST methods (yet)

    rtc_bist();
    nap_bist();
    step_bist();
    stepat_bist();
    subs_bist();
    fifo_bist();
    sched_bist();
    edge_bist();
    addr_bist();
    data_bist();
    clock_bist();
    timing_bist();
    i8224_bist();
    i8228_bist();
    decoder_bist();
    ram8107x8x4_bist();
    rom8316_bist();
    i8080_bist();
    cdev_bist();
    bdev_bist();
    return 0;
}

int main_bench(int argc, char **argv)
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
    addr_bench();
    data_bench();
    clock_bench();
    timing_bench();
    i8224_bench();
    i8228_bench();
    decoder_bench();
    ram8107x8x4_bench();
    rom8316_bench();
    i8080_bench();
    return 0;
}
