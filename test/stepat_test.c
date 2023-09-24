#include "stepat.h"
#include "support.h"

// stepat: a function to call, a pointer to pass it, and a Tau.

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         stepat_fn_all(StepAt *args);
static void         stepat_fn1(int *there);
static void         stepat_fn2(int *there);
static void         stepat_fn3(int *there);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

// The stepat_post() function performs a very rapid check to assure
// that the stepat facility is not obviously broken, and must be
// called by the program before depending on stepat to work.

void stepat_post()
{
    int                 stepat_post_arg[1] = { 1337 };

    Tau                 TAU_expected = TAU + 24;

    StepAt              s = STEPAT_INIT(TAU_expected, stepat_fn1, stepat_post_arg);

    stepat_run(s);

    ASSERT_EQ_integer(1338, *stepat_post_arg);
    ASSERT_EQ_integer(TAU_expected, TAU);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void stepat_bist()
{
    int                 start_val = 1337;

    int                 stepat_args[3] = { start_val, start_val, start_val };

    Tau                 t1 = TAU + 10;
    Tau                 t2 = TAU + 20;
    Tau                 t3 = TAU + 30;

    StepAt              stepat_arg[3] = {
        STEPAT_INIT(t1, stepat_fn1, stepat_args + 0),
        STEPAT_INIT(t2, stepat_fn2, stepat_args + 1),
        STEPAT_INIT(t3, stepat_fn3, stepat_args + 2)
    };

    StepAt              bench_stepat = STEPAT_INIT(0, stepat_fn_all, stepat_arg);

    stepat_run(bench_stepat);

    ASSERT_EQ_integer(start_val + 1, stepat_args[0]);
    ASSERT_EQ_integer(start_val + 10, stepat_args[1]);
    ASSERT_EQ_integer(start_val + 100, stepat_args[2]);

    ASSERT_EQ_integer(t3, TAU);
}

// === === === === === === === === === === === === === === === ===
//                   BENCHMARK THE STEPAT FACILITY
// === === === === === === === === === === === === === === === ===

void stepat_bench()
{
    int                 start_val = 1337;

    int                 stepat_args[3] = { start_val, start_val, start_val };

    Tau                 t1 = TAU + 10;
    Tau                 t2 = TAU + 20;
    Tau                 t3 = TAU + 30;

    StepAt              stepat_arg[3] = {
        STEPAT_INIT(t1, stepat_fn1, stepat_args + 0),
        STEPAT_INIT(t2, stepat_fn2, stepat_args + 1),
        STEPAT_INIT(t3, stepat_fn3, stepat_args + 2)
    };

    double              dt = RTC_ELAPSED(stepat_fn_all, stepat_arg);

    BENCH_TOP("stepat");
    BENCH_VAL(dt / 4.0);
    BENCH_END();
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void stepat_fn_all(StepAt *args)
{
    stepat_run(args[0]);
    stepat_run(args[1]);
    stepat_run(args[2]);
}

static void stepat_fn1(int *there)
{
    *there += 1;
}

static void stepat_fn2(int *there)
{
    *there += 10;
}

static void stepat_fn3(int *there)
{
    *there += 100;
}
