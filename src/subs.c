#include "common/subs.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <stdlib.h>     // General utilities
#include "common/support.h"

// subs: a place for things to do

static int          subs_extend(Subs);

// subs_invar(s): verify the invariants for a Subs.
// - length does not exceed capacity
// - bubble is within the range, or just one outside it
// - if capacity is nonzero, list must not be null
// - each Step on the list must satisfy its invariant
// bubble indicates a step to skip when checking invariants.

void subs_invar(const Subs s)
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

    assert(slot == s->bubble);
    list[slot].fp = fp;
    list[slot].ap = ap;
    s->bubble = -1;

    subs_invar(s);
}

// subs_first(s, fp, ap): add a subsciber to the START of the list (function)
//
// This function extends the list by one, moves all content down the
// list maintaining order, and places the new item at the start of the
// list.
//
// This code should only be used during set-up time,
// and subscription lists should remain unmodified during
// the bulk of a run.
//
// This code is not performance critical.

void subs_first(Subs s, StepFp fp, StepAp ap)
{
    subs_invar(s);

    int                 slot = s->bubble;

    if (slot < 0 || slot >= s->len)
        slot = subs_extend(s);

    subs_invar(s);

    pStep               list = s->list;

    assert(slot == s->bubble);
    while (slot > 0) {
        list[slot].fp = list[slot - 1].fp;
        list[slot].ap = list[slot - 1].ap;
        slot--;
        s->bubble--;
        subs_invar(s);
    }

    list[slot].fp = fp;
    list[slot].ap = ap;

    assert(slot == s->bubble);
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
           "after expansion, cap %d not big enough to expand to %d items", cap, slot);

    s->bubble = slot;
    s->len = slot + 1;

    subs_invar(s);

    return slot;
}
