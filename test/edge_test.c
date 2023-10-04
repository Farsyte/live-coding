#include <stdlib.h>     // General utilities
#include "edge.h"
#include "stepat.h"
#include "support.h"

// edge: a signal with callbacks for rising and falling edges
//
// This facility tracks the value of a signal in the simulated system,
// and triggers callbacks on rising and falling edges. It has built-in
// protection against infinite recursion (where the value of the edge is
// changed during a callback), and detection of some hazards.

typedef struct sEdgeCtx {
    int                 counter;
    Tau                 init_tau;
    Tau                 update_delta;
    Cstr                message;
}                  *pEdgeCtx, EdgeCtx[1];

static void         edgectx_init(EdgeCtx, Cstr);
static void         edgectx_free(EdgeCtx);

static void         edge_saw_rise(EdgeCtx);
static void         edge_saw_fall(EdgeCtx);

// edge_post: Power-On Self Test for the Edge code
//
// This function should be called every time the program starts
// before any code relies on Edge not being completely broken.

void edge_post()
{
    Edge                e;
    EDGE_INIT(e, 0);
    edge_invar(e);

    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(0, e->rise->len);
    ASSERT_EQ_integer(0, e->fall->len);
    ASSERT_EQ_integer(0, e->busy);

    EdgeCtx             rise_ctx;
    edgectx_init(rise_ctx, 0);
    EDGE_ON_RISE(e, edge_saw_rise, rise_ctx);
    edge_invar(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(1, e->rise->len);

    EdgeCtx             fall_ctx;
    edgectx_init(fall_ctx, 0);
    EDGE_ON_FALL(e, edge_saw_fall, fall_ctx);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(1, e->fall->len);
    edge_invar(e);

    EdgeCtx             rise_1st_ctx;
    edgectx_init(rise_1st_ctx, 0);
    EDGE_FIRST_ON_RISE(e, edge_saw_rise, rise_1st_ctx);
    edge_invar(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(2, e->rise->len);

    EdgeCtx             fall_1st_ctx;
    edgectx_init(fall_1st_ctx, 0);
    EDGE_FIRST_ON_FALL(e, edge_saw_fall, fall_1st_ctx);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(2, e->fall->len);
    edge_invar(e);

    edge_lo(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_hi(e);
    ASSERT_EQ_integer(1, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_lo(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_hi(e);
    ASSERT_EQ_integer(1, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_hi(e);
    ASSERT_EQ_integer(1, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_hi(e);
    ASSERT_EQ_integer(1, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_lo(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_lo(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    edgectx_free(fall_1st_ctx);
    edgectx_free(rise_1st_ctx);
    edgectx_free(fall_ctx);
    edgectx_free(rise_ctx);
}

// edge_bist: Power-On Self Test for the Edge code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Edge code is
// not working correctly.

void edge_bist()
{
    Edge                e;
    EDGE_INIT(e, 0);
    edge_invar(e);

    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(0, e->rise->len);
    ASSERT_EQ_integer(0, e->fall->len);
    ASSERT_EQ_integer(0, e->busy);

    const int           subcount = 50;

    EdgeCtx             rise_ctx[subcount];
    for (int sub = 0; sub < subcount; ++sub) {
        edgectx_init(rise_ctx[sub], format("in rise-subscriber %d", sub));
        EDGE_ON_RISE(e, edge_saw_rise, rise_ctx[sub]);
    }

    edge_invar(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(subcount, e->rise->len);

    EdgeCtx             fall_ctx[subcount];
    for (int sub = 0; sub < subcount; ++sub) {
        edgectx_init(fall_ctx[sub], format("in fall-subscriber %d", sub));
        EDGE_ON_FALL(e, edge_saw_fall, fall_ctx[sub]);
    }

    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(subcount, e->fall->len);
    edge_invar(e);

    edge_lo(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_hi(e);
    ASSERT_EQ_integer(1, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_lo(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_hi(e);
    ASSERT_EQ_integer(1, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_hi(e);
    ASSERT_EQ_integer(1, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_hi(e);
    ASSERT_EQ_integer(1, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_lo(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    TAU++;
    edge_lo(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_EQ_integer(TAU, e->when);

    for (int sub = subcount; sub-- > 0;)
        edgectx_free(fall_ctx[sub]);

    for (int sub = subcount; sub-- > 0;)
        edgectx_free(rise_ctx[sub]);
}

static void bench_edge(Edge e)
{
    TAU++;
    edge_hi(e);
    TAU++;
    edge_lo(e);
}

// edge_bench: performance verification for the Edge code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Edge.

void edge_bench()
{
    Edge                e;
    EDGE_INIT(e, 0);
    edge_invar(e);

    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(0, e->rise->len);
    ASSERT_EQ_integer(0, e->fall->len);
    ASSERT_EQ_integer(0, e->busy);

    const int           rise_subs = 1;

    // the +1 allows me to set rise_subs to zero.
    EdgeCtx             rise_ctx[rise_subs + 1];
    for (int sub = 0; sub < rise_subs; ++sub) {
        edgectx_init(rise_ctx[sub], 0);
        EDGE_ON_RISE(e, edge_saw_rise, rise_ctx[sub]);
    }

    edge_invar(e);
    ASSERT_EQ_integer(0, e->value);
    ASSERT_LT_integer(TAU, e->when);
    ASSERT_EQ_integer(rise_subs, e->rise->len);

    const int           fall_subs = 0;

    // the +1 allows me to set fall_subs to zero.
    EdgeCtx             fall_ctx[fall_subs + 1];
    for (int sub = 0; sub < fall_subs; ++sub) {
        edgectx_init(fall_ctx[sub], 0);
        EDGE_ON_FALL(e, edge_saw_fall, fall_ctx[sub]);
    }

    BENCH_TOP("edge");
    double              dt = RTC_ELAPSED(bench_edge, e);
    BENCH_VAL(dt / (rise_subs + fall_subs));
    BENCH_END();

    for (int sub = fall_subs; sub-- > 0;)
        edgectx_free(fall_ctx[sub]);

    for (int sub = rise_subs; sub-- > 0;)
        edgectx_free(rise_ctx[sub]);
}

static void edgectx_init(EdgeCtx ctx, Cstr msg)
{
    ctx->counter = 0;
    ctx->init_tau = TAU;
    ctx->update_delta = 0;
    ctx->message = msg;
}

static void edgectx_free(EdgeCtx ctx)
{
    Cstr                msg = ctx->message;
    if (msg) {
        ctx->message = 0;
        free((char *)msg);
    }
}

static void edge_saw_rise(EdgeCtx ctx)
{
    ctx->counter++;
    ctx->update_delta = TAU - ctx->init_tau;
    if (ctx->message) {
        printf("%s: %lld Δτ, %d count, %s\n", __func__,
               (long long)ctx->update_delta, (int)ctx->counter, ctx->message);
    }
}

static void edge_saw_fall(EdgeCtx ctx)
{
    ctx->counter++;
    ctx->update_delta = TAU - ctx->init_tau;
    if (ctx->message) {
        printf("%s: %lld Δτ, %d count, %s\n", __func__,
               (long long)ctx->update_delta, (int)ctx->counter, ctx->message);
    }
}
