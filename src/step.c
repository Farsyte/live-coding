#include "step.h"
#include "support.h"

// step: a function to call, and a pointer to pass it.

void step_invar(Step s)
{
    assert(s);
    assert(s->fp);
}

// step_run(s): run the referenced step.
// NOTE: This is in the critical timing path.

void step_run(Step s)
{
    s->fp(s->ap);
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         step_fn_all(Step *args);
static void         step_fn1(int *there);
static void         step_fn2(int *there);
static void         step_fn3(int *there);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void step_post()
{
    int                 step_post_arg[1] = { 1337 };

    Step                post_step = STEP_INIT(step_fn1, step_post_arg);

    step_run(post_step);

    ASSERT_EQ_integer(1338, *step_post_arg);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void step_bist()
{
    int                 start_val = 1337;

    int                 step_args[3] = { start_val, start_val, start_val };

    Step                step_arg[3] = {
        STEP_INIT(step_fn1, step_args + 0),
        STEP_INIT(step_fn2, step_args + 1),
        STEP_INIT(step_fn3, step_args + 2)
    };

    Step                bist_step = STEP_INIT(step_fn_all, step_arg);

    step_run(bist_step);

    ASSERT_EQ_integer(start_val + 1, step_args[0]);
    ASSERT_EQ_integer(start_val + 10, step_args[1]);
    ASSERT_EQ_integer(start_val + 100, step_args[2]);
}

// === === === === === === === === === === === === === === === ===
//                   BENCHMARK THE STEP FACILITY
// === === === === === === === === === === === === === === === ===

void step_bench()
{
    int                 start_val = 1337;

    int                 step_args[3] = { start_val, start_val, start_val };

    Step                step_arg[3] = {
        STEP_INIT(step_fn1, step_args + 0),
        STEP_INIT(step_fn2, step_args + 1),
        STEP_INIT(step_fn3, step_args + 2)
    };

    Step                bench_step = STEP_INIT(step_fn_all, step_arg);

    double              dt = RTC_ELAPSED(step_run, bench_step);

    BENCH_TOP("step");
    BENCH_VAL(dt / 4.0);
    BENCH_END();
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void step_fn_all(Step *args)
{
    step_run(args[0]);
    step_run(args[1]);
    step_run(args[2]);
}

static void step_fn1(int *there)
{
    *there += 1;
}

static void step_fn2(int *there)
{
    *there += 10;
}

static void step_fn3(int *there)
{
    *there += 100;
}
