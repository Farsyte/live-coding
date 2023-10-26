#include "common/fifo.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <stdlib.h>     // General utilities
#include <string.h>     // String handling
#include "simext/support.h"

static void         fifo_double(Fifo);

#define MINIMUM_RING_BITS	3
#define MINIMUM_RING_SIZE	(1u << MINIMUM_RING_BITS)
#define MINIMUM_RING_MASK	(MINIMUM_RING_SIZE - 1)

// fifo_invar(f): check the invariants on this Fifo and the Step items
// it contains. Note that this scans the ring, so it is O(f->prod -
// f->cons) in performance, and should not be called from within
// time-critical code.

void fifo_invar(Fifo f)
{
    assert(f);

    pStep               ring = f->ring;
    Tau                 mask = f->mask;
    Tau                 prod = f->prod;
    Tau                 cons = f->cons;

    assert(ring);
    assert(mask >= MINIMUM_RING_MASK);
    assert(prod >= cons);
    assert(prod - cons <= mask);

    for (Tau i = cons; i != prod; i++)
        step_invar(ring + (mask & i));
}

// #define COSTLY_DEBUG_OPTIONS

// macro: places to call fifo_invar when debugging.
//
#ifndef	COSTLY_DEBUG_OPTIONS
#define DEBUG_FIFO_INVAR(f) ((void)(f))
#else
#define DEBUG_FIFO_INVAR(f) (fifo_invar(f))
#endif

// macro: things to assert while debug.
//
#ifndef	COSTLY_DEBUG_OPTIONS
#define DEBUG_ASSERT(cond, ...)
#else
#define DEBUG_ASSERT(cond, ...)       ASSERT(cond, __VA_ARGS__)
#endif

// fifo_init(f): initialize the provided Fifo to be empty.

void fifo_init(Fifo f)
{
    f->mask = MINIMUM_RING_MASK;
    f->prod = 0;
    f->cons = 0;

    f->ring = calloc(f->mask + 1, sizeof(f->ring[0]));

    ASSERT(f->ring,
           "calloc(%lu, %lu) failed in fifo_init",
           (unsigned long)f->mask + 1, (unsigned long)sizeof(f->ring[0]));

    fifo_invar(f);
}

// fifo_add(s,fp,ap): Add an entry on the given fifo to call the given
// function with the given parameter when all entries added before it
// have been serviced. The function declaration does require functions
// and paremeters to be of the given types.
//
// This function is performance critical.

void fifo_add(Fifo f, StepFp fp, StepAp ap)
{
    DEBUG_FIFO_INVAR(f);

    Tau                 mask = f->mask;
    Tau                 prod = f->prod;
    Tau                 cons = f->cons;
    if (prod - cons == mask) {
        // this specific block is NOT performance critical.
        fifo_double(f); // no bist coverage
        mask = f->mask; // no bist coverage
    }

    pStep               ring = f->ring;

    ring[mask & prod].fp = fp;
    ring[mask & prod].ap = ap;

    f->prod = prod + 1;

    DEBUG_FIFO_INVAR(f);
}

// fifo_run_one(s): run the next Step on the fifo. The fifo is in its
// stable state when calling the function, which is thus free to fifo
// more items. Does nothing if the fifo is empty.
//
// This function is performance critical.

void fifo_run_one(Fifo f)
{
    DEBUG_FIFO_INVAR(f);

    Tau                 prod = f->prod;
    Tau                 cons = f->cons;

    if (prod > cons) {
        Tau                 mask = f->mask;
        pStep               ring = f->ring;
        f->cons = cons + 1;
        DEBUG_FIFO_INVAR(f);
        step_run(ring + (mask & cons));
    }

    DEBUG_FIFO_INVAR(f);
}

// fifo_run_all(s): repeatedly run items from the fifo until the fifo
// is empty.
//
// This function is performance critical.

void fifo_run_all(Fifo f)
{
    DEBUG_FIFO_INVAR(f);
    while (f->prod > f->cons) {
        fifo_run_one(f);
        DEBUG_FIFO_INVAR(f);
    }
    DEBUG_FIFO_INVAR(f);
}

// fifo_double(f): double the storage for the ring, then
// copy the original data into both halves of the new storage,
// so that the same prod and cons, with the new mask, point at
// the same semantic content.
//
// Most of this function is performance critical.
//
// NO BIST COVERAGE for this function.

static void fifo_double(Fifo f)
{
    fifo_invar(f);

    Tau                 mask = f->mask;
    pStep               ring = f->ring;
    size_t              old_size = mask + 1;
    size_t              old_bytes = old_size * sizeof(ring[0]);

    Tau                 new_mask = (mask << 1) | 1;

    pStep               copy;

    copy = calloc(new_mask + 1, sizeof(copy[0]));

    memcpy(copy, ring, old_bytes);
    memcpy(copy + old_size, ring, old_bytes);

    f->ring = copy;
    f->mask = new_mask;

    fifo_invar(f);
}
