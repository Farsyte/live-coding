#pragma once
#include "step.h"

// subs is a list of steps that can be extended, and executed.

typedef struct {
    pStep               list;
    int                 cap;
    int                 len;
}                  *pSubs, Subs[1];

extern void         subs_invar(Subs);
extern void         subs_init(Subs);

#define SUBS_ADD(s, fp, ap)	subs_add(s, ((StepFp)(fp)), ((StepAp)(ap)))
extern void         subs_add(Subs, StepFp, StepAp);

extern void         subs_run(Subs);

extern void         subs_post();
extern void         subs_bist();
extern void         subs_bench();
