#include "subs.h"
#include "support.h"

// subs: a place for things to do

static int          subs_extend(Subs);

// subs_invar(s): verify the invariants for a Subs.
// - length does not exceed capacity
// - bubble is within the range, or just one outside it
// - if capacity is nonzero, list must not be null
// - each Step on the list must satisfy its invariant
// bubble indicates a step to skip when checking invariants.

void subs_invar(Subs s)
{
    assert(s);
    assert(s->cap >= s->len);
    assert(s->len >= 0);
    assert(s->bubble >= -1);
    assert(s->bubble <= s->len);

    if (s->cap)
        assert(s->list);

    for (int i = 0; i < s->len; ++i) {
        if (i == s->bubble)
            continue;

        step_invar(s->list + i);
    }
}

// subs_init(s): initialize the given Subs to be empty.

void subs_init(Subs s)
{
    s->list = 0;
    s->cap = 0;
    s->len = 0;
    s->bubble = -1;
    subs_invar(s);
}

// subs_add(s, fp, ap): add a subsciber to the list.
//
// This code should only be used during set-up time,
// and subscription lists should remain unmodified during
// the bulk of a run.
//
// This code is not performance critical.

void subs_add(Subs s, StepFp fp, StepAp ap)
{
    subs_invar(s);

    int                 slot = s->bubble;

    if (slot < 0 || slot >= s->len)
        slot = subs_extend(s);

    subs_invar(s);

    pStep               list = s->list;

    list[slot].fp = fp;
    list[slot].ap = ap;

    if (slot == s->bubble)
        s->bubble = -1;

    subs_invar(s);
}

// subs_run(s): run all steps in the subs list.
// NOTE: This is in the critical timing path.

void subs_run(Subs s)
{
    for (int i = 0; i < s->len; ++i) {
        step_run(s->list + i);
    }
}

// subs_extend: increase the length of the active list.
// allocates additional memory as needed.

static int subs_extend(Subs s)
{
    subs_invar(s);
    assert(s->bubble == -1);

    int                 slot = s->len;
    int                 cap = s->cap;
    pStep               list = s->list;

    if (slot == cap) {
        void               *copy;
        cap = (cap < 8) ? 8 : 2 * cap;
        copy = realloc(list, cap * sizeof(list[0]));
        ASSERT(copy, "unable to realloc %p to %d entries\n", (void *)list, cap);
        s->list = copy;
        s->cap = cap;
        subs_invar(s);
    }

    ASSERT(slot < cap,
           "after expansion, cap %d not big enough to expand to %d items", cap,
           slot);

    s->bubble = slot;
    s->len = slot + 1;

    subs_invar(s);

    return slot;
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         subs_fn1(int *);
static void         subs_fn2(int *);
static void         subs_fn3(int *);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

// subs_post: Power-On Self Test for the Subs code
//
// This function should be called every time the program starts
// before any code relies on Subs not being completely broken.

void subs_post()
{
    Subs                s;
    subs_init(s);
    ASSERT_EQ_integer(0, s->len);

    const int           start_val = 1337;

    int                 subs_args[1] = { start_val };

    SUBS_ADD(s, subs_fn1, subs_args + 0);

    subs_run(s);

    ASSERT_EQ_integer(start_val + 1, subs_args[0]);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

// subs_bist: Power-On Self Test for the Subs code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Subs code is
// not working correctly.

void subs_bist()
{
    Subs                s;
    subs_init(s);
    ASSERT_EQ_integer(0, s->len);

    const int           start_val = 1337;

    int                 subs_args[3] = { start_val, start_val, start_val };

    SUBS_ADD(s, subs_fn1, subs_args + 0);
    SUBS_ADD(s, subs_fn2, subs_args + 1);
    SUBS_ADD(s, subs_fn3, subs_args + 2);

    subs_run(s);
    subs_run(s);
    subs_run(s);

    ASSERT_EQ_integer(start_val + 3, subs_args[0]);
    ASSERT_EQ_integer(start_val + 30, subs_args[1]);
    ASSERT_EQ_integer(start_val + 300, subs_args[2]);
}

// === === === === === === === === === === === === === === === ===
//                   BENCHMARK THE SUBS FACILITY
// === === === === === === === === === === === === === === === ===

// subs_bench: performance verification for the Subs code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Subs.

void subs_bench()
{
    Subs                s;
    subs_init(s);

#define TEST_LEN	16

    unsigned           *subs_args = calloc(TEST_LEN, sizeof(unsigned));
    assert(subs_args);

    for (int i = 0; i < TEST_LEN; ++i)
        SUBS_ADD(s, subs_fn1, subs_args + i);

    double              dt = RTC_ELAPSED(subs_run, s);

    BENCH_TOP("ns per subs iter");
    BENCH_VAL(dt / TEST_LEN);
    BENCH_END();
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void subs_fn1(int *arg)
{
    *arg += 1;
}
static void subs_fn2(int *arg)
{
    *arg += 10;
}
static void subs_fn3(int *arg)
{
    *arg += 100;
}
