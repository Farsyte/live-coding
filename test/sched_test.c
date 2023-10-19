#include "common/sched.h"
#include "common/support.h"

// sched: a place for things to do in order

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

typedef struct sRepCtx {
    int                 counter;
    int                 period;
    int                 until;
    pSched              s;
}                  *pRepCtx, RepCtx[1];

static void         repctx_init(RepCtx ctx, int period, int until, pSched s);
static void         sched_inc_once(RepCtx);
static void         sched_inc_repeat(RepCtx);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void sched_post()
{
    RepCtx              ctx;
    Tau                 t0 = TAU;

    Sched               s;
    sched_init(s);
    sched_invar(s);

    repctx_init(ctx, 0, 0, s);
    t0 = TAU;
    SCHED_ADD(s, t0 + 10, sched_inc_once, ctx);
    ASSERT_EQ_integer(0, ctx->counter);
    sched_invar(s);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 10, TAU);
    ASSERT_EQ_integer(1, ctx->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 10, TAU);
    ASSERT_EQ_integer(1, ctx->counter);

    repctx_init(ctx, 0, 0, s);
    t0 = TAU;
    SCHED_ADD(s, t0 + 10, sched_inc_once, ctx);
    ASSERT_EQ_integer(0, ctx->counter);

    sched_run_all(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 10, TAU);
    ASSERT_EQ_integer(1, ctx->counter);

    repctx_init(ctx, 10, 10, s);
    t0 = TAU;
    SCHED_ADD(s, t0 + 10, sched_inc_repeat, ctx);
    ASSERT_EQ_integer(0, ctx->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 10, TAU);
    ASSERT_EQ_integer(1, ctx->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 20, TAU);
    ASSERT_EQ_integer(2, ctx->counter);

    sched_run_all(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 100, TAU);
    ASSERT_EQ_integer(10, ctx->counter);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void sched_bist()
{
    RepCtx              ctx1;
    RepCtx              ctx2;
    Tau                 t0 = TAU;

    Sched               s;
    sched_init(s);
    sched_invar(s);

    repctx_init(ctx1, 0, 0, s);
    SCHED_ADD(s, t0 + 10, sched_inc_once, ctx1);
    SCHED_ADD(s, t0 + 12, sched_inc_once, ctx1);
    sched_invar(s);
    ASSERT_EQ_integer(0, ctx1->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 10, TAU);
    ASSERT_EQ_integer(1, ctx1->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 12, TAU);
    ASSERT_EQ_integer(2, ctx1->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 12, TAU);
    ASSERT_EQ_integer(2, ctx1->counter);

    repctx_init(ctx1, 0, 0, s);
    t0 = TAU;
    SCHED_ADD(s, t0 + 10, sched_inc_once, ctx1);
    SCHED_ADD(s, t0 + 12, sched_inc_once, ctx1);
    sched_invar(s);
    ASSERT_EQ_integer(0, ctx1->counter);

    sched_run_all(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 12, TAU);
    ASSERT_EQ_integer(2, ctx1->counter);

    t0 = TAU;

    repctx_init(ctx1, 10, 17, s);
    SCHED_ADD(s, t0 + 20, sched_inc_repeat, ctx1);
    sched_invar(s);

    repctx_init(ctx2, 17, 10, s);
    SCHED_ADD(s, t0 + 27, sched_inc_repeat, ctx2);
    sched_invar(s);

    ASSERT_EQ_integer(0, ctx1->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 20, TAU);
    ASSERT_EQ_integer(1, ctx1->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 27, TAU);
    ASSERT_EQ_integer(1, ctx2->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 30, TAU);
    ASSERT_EQ_integer(2, ctx1->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 40, TAU);
    ASSERT_EQ_integer(3, ctx1->counter);

    sched_run_one(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 44, TAU);
    ASSERT_EQ_integer(2, ctx2->counter);

    sched_run_all(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 180, TAU);
    ASSERT_EQ_integer(17, ctx1->counter);
    ASSERT_EQ_integer(10, ctx2->counter);

    repctx_init(ctx1, 10, 17, s);
    repctx_init(ctx2, 17, 10, s);

    t0 = TAU;

    SCHED_ADD(s, t0 + 10, sched_inc_once, ctx1);
    SCHED_ADD(s, t0 + 27, sched_inc_repeat, ctx2);
    SCHED_ADD(s, t0 + 20, sched_inc_repeat, ctx1);
    SCHED_ADD(s, t0 + 12, sched_inc_once, ctx1);
    sched_invar(s);

    ASSERT_EQ_integer(0, ctx1->counter);
    ASSERT_EQ_integer(0, ctx2->counter);

    sched_run_all(s);
    sched_invar(s);
    ASSERT_EQ_integer(t0 + 180, TAU);
    ASSERT_EQ_integer(17, ctx1->counter);
    ASSERT_EQ_integer(10, ctx2->counter);
}

// === === === === === === === === === === === === === === === ===
//                   BENCHMARK THE SCHED FACILITY
// === === === === === === === === === === === === === === === ===

static Tau          bench_sched_runs = 0;
static int          bench_sched_jobs = 64;

void bench_sched(Sched s)
{
    const int           nctr = bench_sched_jobs;
    RepCtx              ctx[nctr];

    const Tau           t0 = TAU;
    for (int i = 0; i < nctr; ++i) {
        repctx_init(ctx[i], 10 + i, 10000 / (10 + i), s);
        SCHED_ADD(s, t0 + 10 + i, sched_inc_repeat, ctx[i]);
    }

    sched_run_all(s);

    bench_sched_runs = 0;
    for (int i = 0; i < nctr; ++i)
        bench_sched_runs += ctx[i]->counter;
}

void sched_bench()
{
    Sched               s;
    sched_init(s);
    sched_invar(s);

    double              dt;

    BENCH_TOP("sched");

    bench_sched_jobs = 1;
    dt = RTC_ELAPSED(bench_sched, s) / bench_sched_runs;
    BENCH_VAL(dt);

    bench_sched_jobs = 3;
    dt = RTC_ELAPSED(bench_sched, s) / bench_sched_runs;
    BENCH_VAL(dt);

    bench_sched_jobs = 50;
    dt = RTC_ELAPSED(bench_sched, s) / bench_sched_runs;
    BENCH_VAL(dt);

    bench_sched_jobs = 100;
    dt = RTC_ELAPSED(bench_sched, s) / bench_sched_runs;
    BENCH_VAL(dt);
    BENCH_END();
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void repctx_init(RepCtx ctx, int period, int until, pSched s)
{
    ctx->counter = 0;
    ctx->period = period;
    ctx->until = until;
    ctx->s = s;
}

static void sched_inc_once(RepCtx ctx)
{
    ctx->counter++;
}

static void sched_inc_repeat(RepCtx ctx)
{
    ctx->counter++;
    if (ctx->counter < ctx->until)
        SCHED_ADD(ctx->s, TAU + ctx->period, sched_inc_repeat, ctx);
}
