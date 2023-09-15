#include "edge.h"
#include "stepat.h"
#include "support.h"

// edge: a signal with callbacks for rising and falling edges
//
// This facility tracks the value of a signal in the simulated system,
// and triggers callbacks on rising and falling edges. It has built-in
// protection against infinite recursion (where the value of the edge is
// changed during a callback), and detection of some hazards.

// edge_invar(e): check invariants for the Edge
// - edge is either high or low
// - subs_invar succeeds on rise and fall lists
// - edge is not currently "busy"
// - last change tau is not in the future

void edge_invar(Edge e)
{
    assert(e);

    Bit                 value = e->value;
    pSubs               rise = e->rise;
    pSubs               fall = e->fall;

    assert(value == 0 || value == 1);
    subs_invar(rise);
    subs_invar(fall);
    assert(!e->busy);
    assert(e->when <= TAU);
}

// edge_init(e): initialise edge
// - initial state is LOW
// - subscriber lists (rise and fall) are empty
// - edge is not busy
// - last value set tau is in the past

void edge_init(Edge e)
{
    e->value = 0;
    subs_init(e->rise);
    subs_init(e->fall);
    e->busy = 0;
    e->when = TAU - 1;
    edge_invar(e);
}

// edge_on_rise(e,fp,ap): call fp(ap) on rising edges.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void edge_on_rise(Edge e, StepFp fp, StepAp ap)
{
    edge_invar(e);
    SUBS_ADD(e->rise, fp, ap);
    edge_invar(e);
}

// edge_on_fall(e,fp,ap): call fp(ap) on falling edges.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void edge_on_fall(Edge e, StepFp fp, StepAp ap)
{
    edge_invar(e);
    SUBS_ADD(e->fall, fp, ap);
    edge_invar(e);
}

// edge_to(e,v): set the edge value to the value v.
// if it changes, notify subscribers on the appropriate list.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
//
// This is a time-critical function.

void edge_to(Edge e, Bit v)
{
    if (e->value != v) {
        // do real work if state changes

        assert(e->when < TAU);  // detect hazard
        assert(!e->busy);       // protect vs infinite recursion
        e->busy = 1;

        e->value = v;

        if (v)
            subs_run(e->rise);
        else
            subs_run(e->fall);

        e->busy = 0;
    }
    // remember tau from every time someone specifies
    // the value, even if the value does not change.
    e->when = TAU;
}

// edge_hi(e): set the edge value to HIGH.
// if it was low, notify subscribers on the "rise" list.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
//
// This is a time-critical function.

void edge_hi(Edge e)
{
    edge_to(e, 1);
}

// edge_lo(e): set the edge value to LOW.
// if it was high, notify subscribers on the "fall" list.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
//
// This is a time-critical function.

void edge_lo(Edge e)
{
    edge_to(e, 0);
}

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
    edge_init(e);
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
    edge_init(e);
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
    edge_init(e);
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
