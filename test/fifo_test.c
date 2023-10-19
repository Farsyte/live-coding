#include "common/fifo.h"
#include "common/support.h"

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

typedef struct sRepCtx {
    int                 counter;
    int                 until;
    pFifo               f;
}                  *pRepCtx, RepCtx[1];

static void         repctx_init(RepCtx ctx, int until, pFifo f);
static void         fifo_inc_once(RepCtx);
static void         fifo_inc_repeat(RepCtx);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void fifo_post()
{
    RepCtx              ctx;

    Fifo                f;
    fifo_init(f);
    fifo_invar(f);

    repctx_init(ctx, 0, f);
    FIFO_ADD(f, fifo_inc_once, ctx);
    ASSERT_EQ_integer(0, ctx->counter);
    fifo_invar(f);

    fifo_run_one(f);
    fifo_invar(f);
    ASSERT_EQ_integer(1, ctx->counter);

    fifo_run_one(f);
    fifo_invar(f);
    ASSERT_EQ_integer(1, ctx->counter);

    repctx_init(ctx, 0, f);
    FIFO_ADD(f, fifo_inc_once, ctx);
    ASSERT_EQ_integer(0, ctx->counter);

    fifo_run_all(f);
    fifo_invar(f);
    ASSERT_EQ_integer(1, ctx->counter);

    repctx_init(ctx, 10, f);
    FIFO_ADD(f, fifo_inc_repeat, ctx);
    ASSERT_EQ_integer(0, ctx->counter);

    fifo_run_one(f);
    fifo_invar(f);
    ASSERT_EQ_integer(1, ctx->counter);

    fifo_run_one(f);
    fifo_invar(f);
    ASSERT_EQ_integer(2, ctx->counter);

    fifo_run_all(f);
    fifo_invar(f);
    ASSERT_EQ_integer(10, ctx->counter);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void fifo_bist()
{
    RepCtx              ctx1;
    RepCtx              ctx2;

    Fifo                f;
    fifo_init(f);
    fifo_invar(f);

    repctx_init(ctx1, 0, f);
    FIFO_ADD(f, fifo_inc_once, ctx1);
    FIFO_ADD(f, fifo_inc_once, ctx1);
    fifo_invar(f);
    ASSERT_EQ_integer(0, ctx1->counter);

    fifo_run_one(f);
    fifo_invar(f);
    ASSERT_EQ_integer(1, ctx1->counter);

    fifo_run_one(f);
    fifo_invar(f);
    ASSERT_EQ_integer(2, ctx1->counter);

    fifo_run_one(f);
    fifo_invar(f);
    ASSERT_EQ_integer(2, ctx1->counter);

    repctx_init(ctx1, 0, f);
    FIFO_ADD(f, fifo_inc_once, ctx1);
    FIFO_ADD(f, fifo_inc_once, ctx1);
    fifo_invar(f);
    ASSERT_EQ_integer(0, ctx1->counter);

    fifo_run_all(f);
    fifo_invar(f);
    ASSERT_EQ_integer(2, ctx1->counter);

    repctx_init(ctx1, 17, f);
    FIFO_ADD(f, fifo_inc_repeat, ctx1);
    fifo_invar(f);

    repctx_init(ctx2, 10, f);
    FIFO_ADD(f, fifo_inc_repeat, ctx2);
    fifo_invar(f);

    ASSERT_EQ_integer(0, ctx1->counter);
    ASSERT_EQ_integer(0, ctx2->counter);

    for (int i = 1; i < 5; ++i) {
        fifo_run_one(f);
        fifo_invar(f);
        ASSERT_EQ_integer(i, ctx1->counter);

        fifo_run_one(f);
        fifo_invar(f);
        ASSERT_EQ_integer(i, ctx2->counter);
    }

    fifo_run_all(f);
    fifo_invar(f);
    ASSERT_EQ_integer(17, ctx1->counter);
    ASSERT_EQ_integer(10, ctx2->counter);
}

// === === === === === === === === === === === === === === === ===
//                   BENCHMARK THE FIFO FACILITY
// === === === === === === === === === === === === === === === ===

static Tau          bench_fifo_runs = 0;
static int          bench_fifo_jobs = 64;

void bench_fifo(Fifo f)
{
    const int           nctr = bench_fifo_jobs;
    RepCtx              ctx[nctr];

    for (int i = 0; i < nctr; ++i) {
        repctx_init(ctx[i], 10000 / (10 + i), f);
        FIFO_ADD(f, fifo_inc_repeat, ctx[i]);
    }

    fifo_run_all(f);

    bench_fifo_runs = 0;
    for (int i = 0; i < nctr; ++i)
        bench_fifo_runs += ctx[i]->counter;
}

void fifo_bench()
{
    Fifo                f;
    fifo_init(f);
    fifo_invar(f);

    double              dt;

    BENCH_TOP("fifo");

    bench_fifo_jobs = 1;
    dt = RTC_ELAPSED(bench_fifo, f) / bench_fifo_runs;
    BENCH_VAL(dt);

    bench_fifo_jobs = 3;
    dt = RTC_ELAPSED(bench_fifo, f) / bench_fifo_runs;
    BENCH_VAL(dt);

    bench_fifo_jobs = 50;
    dt = RTC_ELAPSED(bench_fifo, f) / bench_fifo_runs;
    BENCH_VAL(dt);

    bench_fifo_jobs = 100;
    dt = RTC_ELAPSED(bench_fifo, f) / bench_fifo_runs;
    BENCH_VAL(dt);
    BENCH_END();
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void repctx_init(RepCtx ctx, int until, pFifo f)
{
    ctx->counter = 0;
    ctx->until = until;
    ctx->f = f;
}

static void fifo_inc_once(RepCtx ctx)
{
    ctx->counter++;
}

static void fifo_inc_repeat(RepCtx ctx)
{
    ctx->counter++;
    if (ctx->counter < ctx->until)
        FIFO_ADD(ctx->f, fifo_inc_repeat, ctx);
}
