#pragma once
#include "common/edge.h"
#include "sigsess.h"
#include "sigtrace.h"

typedef struct sSigTraceEdge {
    SigTrace            base;
    pEdge               edge;
}                  *pSigTraceEdge, SigTraceEdge[1];

extern void         sigtrace_edge_init(SigTraceEdge trc, SigSess sess, pEdge edge);
