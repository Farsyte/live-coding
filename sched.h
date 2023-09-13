#pragma once
#include "stepat.h"

// sched: a place for things to do in order

typedef struct {
    pStepAt             heap;
    int                 cap;
    int                 len;
    int                 bubble;
}                  *pSched, Sched[1];

// sched_invar(s): check the invariants on this Sched and the StepAt
// items it contains. Note that this scans the heap, so it is
// O(s->len) in performance, and should not be called from within
// time-critical code.

extern void         sched_invar(Sched);

// sched_init(s): initialize the provided Sched to be empty.
extern void         sched_init(Sched);

// SCHED_ADD(s,at,fp,ap): Add an entry on the given schedule
// for the given time to call the given function with the
// given parameter. Macro-ized to cast the pointers to the
// appropriate generic types.

#define SCHED_ADD(s, at, fp, ap)                        \
    sched_add(s, at, ((StepFp)(fp)), ((StepAp)(ap)))

// sched_add(s,at,fp,ap): Add an entry on the given schedule
// for the given time to call the given function with the
// given parameter. The function declaration does require
// functions and paremeters to be of the given types.

extern void         sched_add(Sched, Tau, StepFp, StepAp);

// sched_run_one(s): run the next StepAt on the schedule.
// if multiple scheduled items have the same tau, the order
// they are called is not defined. The schedule is in its
// stable state when calling the function, which is thus
// free to schedule more items. Does nothing if the schedule
// is empty (or contains one entry which is a bubble).

extern void         sched_run_one(Sched);

// sched_run_all(s): repeatedly run items from the schedule
// until the schedule is empty.

extern void         sched_run_all(Sched);

// sched_post(): power-on self-test for sched.
// the program should call this function before doing anything
// that relies on sched not being horribly broken.

extern void         sched_post();

// sched_bist(): built-in self-test for sched.
// the program should call this function during the
// automated build process, so that the build will fail
// if the scheduler is not running correctly.

extern void         sched_bist();

// sched_bench(): benchmark the scheduler.
// characterize performance under a modest selection of
// scheduler workloads.

extern void         sched_bench();
