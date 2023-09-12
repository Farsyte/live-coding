#include "step.h"
#include "support.h"

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

static void         step_post_fn(int *there);

void step_post()
{
    int                 step_post_arg[1] = { 1337 };

    Step                s = STEP_INIT(step_post_fn, step_post_arg);

    step_run(s);

    ASSERT_EQ_integer(1338, *step_post_arg);
}

static void step_post_fn(int *there)
{
    ++*there;
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

    Step                bench_step = STEP_INIT(step_fn_all, step_arg);

    step_run(bench_step);

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

    double              dt = RTC_ELAPSED(step_fn_all, step_arg);

    fprintf(stderr, "BENCH: %8.3f ns per step_run() call\n", dt / 4.0);
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
