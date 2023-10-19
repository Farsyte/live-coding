#pragma once
#include "common/support.h"
#include "common/sigsess.h"
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
