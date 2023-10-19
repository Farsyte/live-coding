#pragma once
#include "common/data.h"
#include "sigsess.h"
#include "sigtrace.h"
#include <stdio.h>      // Input/output

typedef struct sSigTraceData {
    SigTrace            base;
    pData               data;
}                  *pSigTraceData, SigTraceData[1];

extern void         sigtrace_data_init(SigTraceData trc, SigSess sess, pData data);
