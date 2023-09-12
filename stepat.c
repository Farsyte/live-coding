#include "stepat.h"
#include "support.h"

Tau                 TAU = 0;

void stepat_invar(StepAt s)
{
    assert(s);
    assert(s->at >= TAU);
    assert(s->fp);
}

void stepat_run(StepAt s)
{
    TAU = s->at;
    s->fp(s->ap);
}

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

static void         stepat_post_fn(int *there);

void stepat_post()
{
    int                 stepat_post_arg[1] = { 1337 };

    Tau                 TAU_expected = TAU + 24;

    StepAt              s =
      STEPAT_INIT(TAU_expected, stepat_post_fn, stepat_post_arg);

    stepat_run(s);

    ASSERT_EQ_integer(1338, *stepat_post_arg);
    ASSERT_EQ_integer(TAU_expected, TAU);
}

static void stepat_post_fn(int *there)
{
    ++*there;
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

    StepAt              bench_stepat =
      STEPAT_INIT(0, stepat_fn_all, stepat_arg);

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

    fprintf(stderr, "BENCH: %8.3f ns per stepat_run() call\n", dt / 4.0);
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
