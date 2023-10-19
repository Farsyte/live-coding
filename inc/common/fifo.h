#pragma once
#include "common/step.h"
#include "common/support.h"

// fifo: a place for things to do next

typedef struct sFifo {
    pStep               ring;
    Tau                 mask;
    Tau                 prod;
    Tau                 cons;
}                  *pFifo, Fifo[1];

// fifo_invar(s): check the invariants on this Fifo and the Step items
// it contains. Note that this scans the ring, so it is O(s->prod -
// s->cons) in performance, and should not be called from within
// time-critical code.

extern void         fifo_invar(Fifo);

// fifo_init(s): initialize the provided Fifo to be empty,
// and has some reasonable initial capacity.

extern void         fifo_init(Fifo);

// FIFO_ADD(s,fp,ap): Add an entry on the given fifo to call the
// given function with the given parameter when all entries added
// before it have been serviced. Macro-ized to cast the pointers to
// the appropriate generic types.

#define FIFO_ADD(s, fp, ap)                     \
    fifo_add(s, ((StepFp)(fp)), ((StepAp)(ap)))

// fifo_add(s,fp,ap): Add an entry on the given fifo to call the given
// function with the given parameter when all entries added before it
// have been serviced. The function declaration does require functions
// and paremeters to be of the given types.

extern void         fifo_add(Fifo, StepFp, StepAp);

// fifo_run_one(s): run the next Step on the fifo. The fifo is in its
// stable state when calling the function, which is thus free to fifo
// more items. Does nothing if the fifo is empty.

extern void         fifo_run_one(Fifo);

// fifo_run_all(s): repeatedly run items from the fifo until the fifo
// is empty.

extern void         fifo_run_all(Fifo);

// fifo_post(): power-on self-test for fifo.
// the program should call this function before doing anything
// that relies on fifo not being horribly broken.

extern void         fifo_post();

// fifo_bist(): built-in self-test for fifo.
// the program should call this function during the
// automated build process, so that the build will fail
// if the fifor is not running correctly.

extern void         fifo_bist();

// fifo_bench(): benchmark the fifor.
// characterize performance under a modest selection of
// fifor workloads.

extern void         fifo_bench();
