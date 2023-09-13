#pragma once
#include "subs.h"
#include "support.h"
#include "target.h"

// turn both of these off to reduce the
// edge overhead by about 10% when there
// are 50 clients per subs.

typedef struct sEdge {
    Bit                 value;
    Subs                rise;
    Subs                fall;
    int                 busy;
    Tau                 when;
}                  *pEdge, Edge[1];

extern void         edge_invar(Edge);
extern void         edge_init(Edge);

#define EDGE_ON_RISE(e,fp,ap) \
    edge_on_rise(e, ((StepFp)(fp)), ((StepAp)(ap)))

#define EDGE_ON_FALL(e,fp,ap) \
    edge_on_fall(e, ((StepFp)(fp)), ((StepAp)(ap)))

extern void         edge_on_rise(Edge, StepFp, StepAp);
extern void         edge_on_fall(Edge, StepFp, StepAp);

extern void         edge_hi(Edge);
extern void         edge_lo(Edge);

extern void         edge_post();
extern void         edge_bist();
extern void         edge_bench();
