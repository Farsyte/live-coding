#pragma once
#include "stepat.h"

// sched is 

typedef struct {
    pStepAt             heap;
    int                 cap;
    int                 len;
    int                 bubble;
}                  *pSched, Sched[1];

extern void         sched_invar(Sched);
extern void         sched_init(Sched);

#define SCHED_ADD(s, at, fp, ap)                        \
    sched_add(s, at, ((StepFp)(fp)), ((StepAp)(ap)))

extern void         sched_add(Sched, Tau, StepFp, StepAp);

extern void         sched_run_one(Sched);
extern void         sched_run_all(Sched);

extern void         sched_post();
extern void         sched_bist();
extern void         sched_bench();
