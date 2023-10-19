#pragma once
#include "common/step.h"

// subs: a place for things to do

typedef struct sSubs {
    pStep               list;
    int                 cap;
    int                 len;
    int                 bubble;
}                  *pSubs, Subs[1];

// subs_invar: verify the invariants for a Subs.
// - length does not exceed capacity
// - bubble is within the range, or just one outside it
// - if capacity is nonzero, list must not be null
// - each Step on the list must satisfy its invariant
// bubble indicates a step to skip when checking invariants.

extern void         subs_invar(const Subs);

// subs_init(s): initialize the given Subs to be empty.

extern void         subs_init(Subs);

// SUBS_ADD(s, fp, ap): add a subsciber to the list (macro with casting)
// subs_add(s, fp, ap): add a subsciber to the list (function)
//
// This code should only be used during set-up time,
// and subscription lists should remain unmodified during
// the bulk of a run.

#define SUBS_ADD(s, fp, ap)                     \
    subs_add(s, ((StepFp)(fp)), ((StepAp)(ap)))

extern void         subs_add(Subs, StepFp, StepAp);

// SUBS_FIRST(s, fp, ap): add a subsciber to the START of the list (macro with casting)
// subs_first(s, fp, ap): add a subsciber to the START of the list (function)
//
// This function extends the list by one, moves all content down the
// list maintaining order, and places the new item at the start of the
// list.
//
// This code should only be used during set-up time,
// and subscription lists should remain unmodified during
// the bulk of a run.

#define SUBS_FIRST(s, fp, ap)                     \
    subs_first(s, ((StepFp)(fp)), ((StepAp)(ap)))

extern void         subs_first(Subs, StepFp, StepAp);

// subs_run(s): run all steps in the subs list.
// NOTE: This is in the critical timing path.

extern void         subs_run(Subs);

// subs_post: Power-On Self Test for the Subs code
//
// This function should be called every time the program starts
// before any code relies on Subs not being completely broken.

extern void         subs_post();

// subs_bist: Power-On Self Test for the Subs code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Subs code is
// not working correctly.

extern void         subs_bist();

// subs_bench: performance verification for the Subs code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Subs.

extern void         subs_bench();
