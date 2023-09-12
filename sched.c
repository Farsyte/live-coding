#include "sched.h"
#include "support.h"

static int          sched_extend(Sched);

static int          sched_push_to_head(Sched s, int slot, Tau at);
static int          sched_push_to_tail(Sched s, int slot, Tau at);

// sched_invar(s): check the invariants on this Sched and the StepAt
// items it contains. Note that this scans the heap, so it is
// O(s->len) in performance, and should not be called from within
// time-critical code.

void sched_invar(Sched s)
{
    assert(s);

    pStepAt             heap = s->heap;
    int                 cap = s->cap;
    int                 len = s->len;
    int                 bubble = s->bubble;

    assert(cap >= len);
    assert(len >= 0);
    assert(bubble >= -1);
    assert(bubble <= len);

    if (cap)
        assert(heap);

    for (int i = 0; i < len; ++i) {
        if (i == bubble)
            continue;

        stepat_invar(heap + i);

        if (i == 0)
            continue;

        int                 p = (i - 1) / 2;
        if (p == bubble)
            continue;

        assert(heap[i].at >= heap[p].at);
    }
}

// macro: places to call sched_invar when debugging.
//
#ifndef	COSTLY_DEBUG_OPTIONS
#define DEBUG_SCHED_INVAR(s) ((void)(s))
#else
#define DEBUG_SCHED_INVAR(s) (sched_invar(s))
#endif

// macro: things to assert while debug.
//
#ifndef	COSTLY_DEBUG_OPTIONS
#define DEBUG_ASSERT(cond, ...)
#else
#define DEBUG_ASSERT(cond, ...)       ASSERT(cond, __VA_ARGS__)
#endif

// sched_init(s): initialize the provided Sched to be empty.

void sched_init(Sched s)
{
    s->heap = 0;
    s->cap = 0;
    s->len = 0;
    s->bubble = -1;
    DEBUG_SCHED_INVAR(s);
}

// sched_add(s,at,fp,ap): Add an entry on the given schedule
// for the given time to call the given function with the
// given parameter. The function declaration does require
// functions and paremeters to be of the given types.
//
// This function is performance critical.

void sched_add(Sched s, Tau at, StepFp fp, StepAp ap)
{
    DEBUG_SCHED_INVAR(s);

    DEBUG_ASSERT(at >= TAU,
                 "adding a scheduled event in the past is not allowed.\n"
                 "  current TAU: %lld\n  request Tau: %lld\n", TAU, at);

    DEBUG_ASSERT(at < TAU_MAX,
                 "adding a scheduled event at TAU_MAX is not allowed.");

    int                 slot = s->bubble;

    if (slot < 0) {
        slot = sched_extend(s);

        DEBUG_ASSERT(slot == s->bubble,
                     "someone broke sched_extend: bubble (%d) is not slot (%d)",
                     s->bubble, slot);

        DEBUG_ASSERT(slot == s->len - 1,
                     "someone broke sched_extend: slot (%d) != tail (%d)",
                     slot, s->len - 1);

        slot = sched_push_to_head(s, slot, at);
        DEBUG_ASSERT(slot == s->bubble);

        DEBUG_SCHED_INVAR(s);
    } else {
        DEBUG_ASSERT(slot == 0,
                     "sched_add: if bubble exists, must be at zero, not at %d",
                     slot);

        slot = sched_push_to_tail(s, slot, at);
        DEBUG_ASSERT(slot == s->bubble);

        DEBUG_SCHED_INVAR(s);
    }

    pStepAt             heap = s->heap;

    heap[slot].at = at;
    heap[slot].fp = fp;
    heap[slot].ap = ap;

    DEBUG_ASSERT(slot == s->bubble);
    s->bubble = -1;

    DEBUG_SCHED_INVAR(s);
}

// sched_run_one(s): run the next StepAt on the schedule.
// if multiple scheduled items have the same tau, the order
// they are called is not defined. The schedule is in its
// stable state when calling the function, which is thus
// free to schedule more items. Does nothing if the schedule
// is empty (or contains one entry which is a bubble).
//
// This function is performance critical.

void sched_run_one(Sched s)
{
    DEBUG_SCHED_INVAR(s);

    pStepAt             heap = s->heap;
    int                 len = s->len;
    int                 bubble = s->bubble;

    if (len < 1)
        return;

    if (bubble != -1) {

        DEBUG_ASSERT(bubble == 0,
                     "sched_run_one: bubble is at %d, must be at head (0)",
                     bubble);

        if (len == 1) {
            s->len = 0;
            s->bubble = -1;
            DEBUG_SCHED_INVAR(s);
            return;
        }
        // capture the FuncAt at the last entry,
        // and decrement the length of the heap.
        // bubble from head toward tail until we
        // find where to store that entry, and
        // store it, popping the bubble.

        int                 tail = len - 1;
        Tau                 at = heap[tail].at;
        StepFp              fp = heap[tail].fp;
        StepAp              ap = heap[tail].ap;

        s->len = tail;
        DEBUG_SCHED_INVAR(s);

        // Starting at bubble, which is zero, work toward
        // the tail to find where to put {at,fp,ap}.

        int                 slot = sched_push_to_tail(s, 0, at);
        DEBUG_SCHED_INVAR(s);

        DEBUG_ASSERT(slot == s->bubble,
                     "sched_run_one: after push, slot (%d) != bubble (%d)",
                     slot, s->bubble);

        heap[slot].at = at;
        heap[slot].fp = fp;
        heap[slot].ap = ap;
        s->bubble = -1;
        DEBUG_SCHED_INVAR(s);
    }

    s->bubble = 0;
    DEBUG_SCHED_INVAR(s);

    // RISKY: we assert that setting bubble and testing
    // invarianat do not change the contents of the heap
    // cell that is the bubble.

    stepat_run(heap + 0);
    DEBUG_SCHED_INVAR(s);
}

// sched_run_all(s): repeatedly run items from the schedule
// until the schedule is empty.
//
// This function is performance critical.

void sched_run_all(Sched s)
{
    DEBUG_SCHED_INVAR(s);
    while (s->len > 0) {
        sched_run_one(s);
        DEBUG_SCHED_INVAR(s);
    }
    DEBUG_SCHED_INVAR(s);
}

// sched_extend(s): expand the heap by one entry, returning
// the index of the added entry. re-allocate the memory used
// to store it, if the capacity would be exceeded. Place the
// bubble on the new location.
//
// This function checks heap invariants before and after the
// re-allocation process.
//
// This function is called by performance critical code, but
// the block containing the "realloc" call is not crirtical.
// Code outside that condition is critical.
//
// Most of this function is performance critical.

static int sched_extend(Sched s)
{
    DEBUG_ASSERT(s->bubble == -1);

    int                 slot = s->len;
    int                 cap = s->cap;

    if (slot == cap) {
        pStepAt             heap = s->heap;
        void               *copy;

        // This block executes only when needing to double the size of
        // the memory storing the heap; thus, for a heap with maximum
        // length N, this block executes O(log N) times in total over
        // the whole life of the schedule; this block is not
        // considered to be time critical.

        sched_invar(s);
        assert(s->bubble == -1);

        cap = (cap < 8) ? 8 : 2 * cap;
        copy = realloc(heap, cap * sizeof(heap[0]));
        ASSERT(copy, "unable to realloc %p to %d entries\n", (void *)heap, cap);
        s->heap = copy;
        s->cap = cap;

        sched_invar(s);
    }

    s->bubble = slot;
    s->len = slot + 1;

    return slot;
}

// Caller wants to write data into the heap, with "at" giving the
// scheduled time. Start at "slot" (usually if not always the very
// last cell of the heap), and, if "at" is before the scheduled time
// of slot's parent, move the bubble toward the head of the heap.
// Return the selected location where an entry at "at" would statisfy
// the invariants of the sched structure.
//
// This function is performance critical.

static int sched_push_to_head(Sched s, int slot, Tau at)
{
    DEBUG_SCHED_INVAR(s);

    pStepAt             heap = s->heap;

    while (slot > 0) {

        DEBUG_ASSERT(slot == s->bubble,
                     "loop invariant failed: slot (%d) != bubble (%d)",
                     slot, s->bubble);

        int                 p = (slot - 1) / 2;
        if (at >= heap[p].at)
            break;

        heap[slot] = heap[p];
        s->bubble = slot = p;

        DEBUG_SCHED_INVAR(s);
    }

    DEBUG_SCHED_INVAR(s);
    return slot;
}

// Caller wants to write data into the heap, with "at" giving the
// scheduled time. Start at "slot" (usually if not always the very
// first cell of the heap). Grab the child of slot that has the lower
// "at" -- if it is lower than the given "at" then move it into the
// slot, step down to that child, and repeat until adding the entry
// for "at" would statisfy the invariants of the sched structure.
//
// This function is performance critical.

static int sched_push_to_tail(Sched s, int slot, Tau at)
{
    DEBUG_SCHED_INVAR(s);

    pStepAt             heap = s->heap;
    int                 len = s->len;

    while (1) {

        DEBUG_ASSERT(slot == s->bubble,
                     "loop invariant failed: slot (%d) != bubble (%d)",
                     slot, s->bubble);

        // consider the first child.
        int                 c = 2 * slot + 1;
        if (c >= len)
            break;

        int                 ca = heap[c].at;

        // or maybe the second child,
        // if it should run before the first.
        int                 c2 = c + 1;
        if (c2 < len) {
            int                 c2a = heap[c2].at;
            if (c2a < ca) {
                c = c2;
                ca = c2a;
            }
        }
        // if requested "at" is not after either child,
        // we have found our slot.
        if (at <= ca)
            break;

        heap[slot] = heap[c];
        s->bubble = slot = c;

        DEBUG_SCHED_INVAR(s);
    }

    DEBUG_SCHED_INVAR(s);
    return slot;
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

typedef struct {
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
    PRINT_TOP();

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

    PRINT_END();
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void sched_bist()
{
    PRINT_TOP();

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

    PRINT_END();
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
    PRINT_TOP();

    Sched               s;
    sched_init(s);
    sched_invar(s);

    double              dt;

    bench_sched_jobs = 1;
    dt = RTC_ELAPSED(bench_sched, s);
    fprintf(stderr, "BENCH: %8.3f ns per scheduled stepat (heap size %d)\n",
            dt * 1.0 / bench_sched_runs, bench_sched_jobs);

    bench_sched_jobs = 3;
    dt = RTC_ELAPSED(bench_sched, s);
    fprintf(stderr, "BENCH: %8.3f ns per scheduled stepat (heap size %d)\n",
            dt * 1.0 / bench_sched_runs, bench_sched_jobs);

    bench_sched_jobs = 50;
    dt = RTC_ELAPSED(bench_sched, s);
    fprintf(stderr, "BENCH: %8.3f ns per scheduled stepat (heap size %d)\n",
            dt * 1.0 / bench_sched_runs, bench_sched_jobs);

    bench_sched_jobs = 100;
    dt = RTC_ELAPSED(bench_sched, s);
    fprintf(stderr, "BENCH: %8.3f ns per scheduled stepat (heap size %d)\n",
            dt * 1.0 / bench_sched_runs, bench_sched_jobs);

    PRINT_END();
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
