#pragma once
#include "common/sigsess.h"
#include "common/sigtrace.h"
#include "wa16/addr.h"

typedef struct sSigTraceAddr {
    SigTrace            base;
    pAddr               addr;
}                  *pSigTraceAddr, SigTraceAddr[1];

extern void         sigtrace_addr_init(SigTraceAddr trc, SigSess sess, pAddr addr);
