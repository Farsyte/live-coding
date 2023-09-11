#include "step.h"
#include "support.h"

void step_run(Step s)
{
    s->fp(s->ap);
}

double step_elapsed(Step s)
{
    Tau                 maxreps = 1000;
    Tau                 t0, t1, dt;
    double              target_ns = 5.0e7;

    while (1) {

        t0 = rtc_ns();
        for (Tau reps = 0; reps < maxreps; ++reps) {
            s->fp(s->ap);
        }
        t1 = rtc_ns();

        dt = t1 - t0;
        if (dt >= target_ns)
            break;
        else if (dt < target_ns / 10)
            maxreps *= 10;
        else
            maxreps *= target_ns * 2.0 / dt;

        assert(maxreps > 0);
    }

    return dt * 1.0 / maxreps;
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

    ASSERT(*step_post_arg == 1338,
           "step_post FAIL: step_post_fn did not increment the counter\n"
           "  expected: %d\n" "  observed: %d\n", 1338, *step_post_arg);
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

    Step                step_bench = STEP_INIT(step_fn_all, step_arg);

    step_run(step_bench);

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

    Step                step_bench = STEP_INIT(step_fn_all, step_arg);

    double              dt = step_elapsed(step_bench);

    STUB("step_bench: %8.3f ns per step_fn_all call", dt);
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
