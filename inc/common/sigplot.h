#pragma once
#include "common/sigsess.h"
#include "simext/support.h"
#include <stdio.h>      // Input/output

// sigtrace: record signal trace data

typedef struct sSigPlot {
    Cstr                filename;
    pSigSess            sess;
    FILE               *fp;
}                  *pSigPlot, SigPlot[1];

extern void         sigplot_init(SigPlot plot, SigSess sess, Cstr name,
                                 Cstr title, Cstr caption, Tau tau_min, Tau tau_len);
extern void         sigplot_fini(SigPlot plot);
