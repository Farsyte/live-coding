#pragma once
#include "common/step.h"
#include "simext/support.h"

// stepat: a function to call, a pointer to pass it, and a Tau.

// A "stepat" is a function to call, a pointer to pass it,
// and a scheduled SIMULATION time at which it is called.

// The typedef "pStepAt" is a pointer to the storage for
// a stepat; defining a variable of this type allocates
// the storage for the pointer, not for the stepat.

// The typedef "StepAt" is an array of one instance of
// a stepat. defining a variable of this type allocates
// the storage for the stepat; but where the variable
// is used, it looks like a pointer.

typedef struct sStepAt {
    Tau                 at;
    StepFp              fp;
    StepAp              ap;
}                  *pStepAt, StepAt[1];

// The macro STEPAT_INIT provides for initialization of a StepAt
// from a function pointer and parameter pointer, which generally
// will not be of exactly the same type as our storage.

#define STEPAT_INIT(at,fp,ap) {{ at, ((StepFp)(fp)), ((StepAp)(ap)) }}

// The "stepat_invar(StepAt s)" method checks invariants on the stepat.

extern void         stepat_invar(StepAt s);

// The "stepat_run(StepAt s)" method executes the stepat.

extern void         stepat_run(StepAt s);

// The stepat_post() function performs a very rapid check to assure
// that the stepat facility is not obviously broken, and must be
// called by the program before depending on stepat to work.

extern void         stepat_post();

// The stepat_bist() function performans a fairly rapid test of the
// functionality of the stepat code. The program should call this
// function on request, in a path exercised during a normal build.

extern void         stepat_bist();

// The stepat_bench() function measures the performance of the stepat
// package, and should be executed when desired. The performance
// should be verified to be acceptable before a private branch is
// merged back into a shared branch, whether the branch intended
// to change any stepat related code or not.

extern void         stepat_bench();
