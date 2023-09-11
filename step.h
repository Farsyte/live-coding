#pragma once

// A "step" is a function to call, and a pointer to pass it.

// The typedef "StepAp" is the data type that a step uses to
// store the function parameter value.

typedef void       *StepAp;

// The typedef "StepFn" is the data type that a step uses to
// store the pointer to the function. To avoid a small tangle
// of parentheses, the typedef "StepFn" is defined to be the
// type of the function itself.

typedef void        StepFn(StepAp);
typedef StepFn     *StepFp;

// The typedef "pStep" is a pointer to the storage for
// a step; defining a variable of this type allocates
// the storage for the pointer, not for the step.

// The typedef "Step" is an array of one instance of
// a step. defining a variable of this type allocates
// the storage for the step; but where the variable
// is used, it looks like a pointer.

typedef struct {
    StepFp              fp;
    StepAp              ap;
}                  *pStep, Step[1];

// The macro STEP_INIT provides for initialization of a Step
// from a function pointer and parameter pointer, which generally
// will not be of exactly the same type as our storage.

#define STEP_INIT(fp,ap) {{ ((StepFp)(fp)), ((StepAp)(ap)) }}

// The "step_run(Step s)" method executes the step.

extern void         step_run(Step s);

// The "step_elapsed(Step s)" exectues the step many times, and
// returns the average elapsed wall clock time per call as a floating
// point number of nanoseconds. This method will be used by
// benchmarking code within this program.

extern double       step_elapsed(Step s);

// The step_post() function performs a very rapid check to assure
// that the step facility is not obviously broken, and must be
// called by the program before depending on step to work.

extern void         step_post();

// The step_bist() function performans a fairly rapid test of the
// functionality of the step code. The program should call this
// function on request, in a path exercised during a normal build.

extern void         step_bist();

// The step_bench() function measures the performance of the step
// package, and should be executed when desired. The performance
// should be verified to be acceptable before a private branch is
// merged back into a shared branch, whether the branch intended
// to change any step related code or not.

extern void         step_bench();
