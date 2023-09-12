#include "sched.h"
#include "support.h"

static int          sched_extend(Sched);

void sched_invar(Sched s)
{
    assert(s);
    assert(s->cap >= s->len);
    assert(s->len >= 0);
    assert(s->bubble >= -1);
    assert(s->bubble <= s->len);

    if (s->cap)
        assert(s->heap);

    for (int i = 0; i < s->len; ++i) {
        if (i == s->bubble)
            continue;

        stepat_invar(s->heap + i);

        // TODO check heap order invariants.

    }
}

void sched_init(Sched s)
{
    s->heap = 0;
    s->cap = 0;
    s->len = 0;
    s->bubble = -1;
    sched_invar(s);
}

void sched_add(Sched s, Tau at, StepFp fp, StepAp ap)
{
    sched_invar(s);

    int                 slot = s->bubble;

    if ((slot >= 0) && (slot < s->len)) {
        s->bubble = -1;
    } else {
        slot = sched_extend(s);
    }

    sched_invar(s);

    pStepAt             heap = s->heap;

    // TODO bubble the new thing from the tail toward the head.

    heap[slot].at = at;
    heap[slot].fp = fp;
    heap[slot].ap = ap;

    if (slot == s->bubble)
        s->bubble = -1;

    sched_invar(s);
}

void sched_run_one(Sched s)
{
    sched_invar(s);
    if (s->len < 1)
        return;
    stepat_run(s->heap + 0);
    s->bubble = 0;
    sched_invar(s);
    // TODO move the bubble from head to tail
    // TODO with bubble at tail, reduce len, cancel bubble.
}

void sched_run_all(Sched s)
{
    sched_invar(s);
    while (s->len > 0)
        sched_run_one(s);
    sched_invar(s);
}

static int sched_extend(Sched s)
{
    sched_invar(s);
    assert(s->bubble == -1);

    int                 slot = s->len;
    int                 cap = s->cap;
    pStepAt             heap = s->heap;

    if (slot == cap) {
        void               *copy;
        cap = (cap < 8) ? 8 : 2 * cap;
        copy = realloc(heap, cap * sizeof(heap[0]));
        ASSERT(copy, "unable to realloc %p to %d entries\n", (void *)heap, cap);
        s->heap = copy;
        s->cap = cap;
        sched_invar(s);
    }

    ASSERT(slot < cap,
           "after expansion, cap %d not big enough to expand to %d items", cap,
           slot);

    s->bubble = slot;
    s->len = slot + 1;

    sched_invar(s);

    return slot;
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void sched_post()
{
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void sched_bist()
{
}

// === === === === === === === === === === === === === === === ===
//                   BENCHMARK THE SCHED FACILITY
// === === === === === === === === === === === === === === === ===

void sched_bench()
{
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===
