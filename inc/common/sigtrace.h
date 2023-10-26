#pragma once
#include "common/sigplot.h"
#include "common/sigsess.h"
#include "simext/support.h"
#include <stdio.h>      // Input/output

// sigtrace: record signal trace data

typedef struct sSigTrace {
    pSigSess            sess;
    Cstr                name;
    Tau                 next_tau;
    int                 curr_line_len;
    Cstr                filename;
    FILE               *fp;
}                  *pSigTrace, SigTrace[1];

extern void         sigtrace_init(SigTrace trc, SigSess sess, Cstr name);
extern void         sigtrace_log(SigTrace trc, char ch, Cstr data);
extern void         sigtrace_plot(SigTrace trc, SigPlot plot);
extern void         sigtrace_fini(SigTrace trc);
