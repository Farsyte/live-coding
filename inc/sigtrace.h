#pragma once
#include "addr.h"
#include "data.h"
#include "edge.h"
#include "support.h"
#include <stdio.h>      // Input/output

// sigtrace: record signal trace data

typedef struct sSigSess {
    Cstr                name;
    Cstr                dirname;
}                  *pSigSess, SigSess[1];

typedef struct sSigTrace {
    pSigSess            sess;
    Cstr                name;
    pEdge               edge;
    pAddr               addr;
    pData               data;
    Tau                 next_tau;
    int                 curr_line_len;
    Cstr                filename;
    FILE               *fp;
}                  *pSigTrace, SigTrace[1];

typedef struct sSigPlot {
    Cstr                filename;
    pSigSess            sess;
    FILE               *fp;
}                  *pSigPlot, SigPlot[1];

extern void         sigsess_init(SigSess sess, Cstr name);

extern void         sigtrace_init_edge(SigTrace trc, SigSess sess, pEdge edge);
extern void         sigtrace_init_addr(SigTrace trc, SigSess sess, pAddr addr);
extern void         sigtrace_init_data(SigTrace trc, SigSess sess, pData data);

extern void         sigplot_init(SigPlot plot, SigSess sess, Cstr name,
                                 Cstr title, Cstr caption, Tau tau_min, Tau tau_len);
extern void         sigtrace_log(SigTrace trc, char ch, Cstr data);
extern void         sigplot_sig(SigPlot plot, SigTrace trc);
extern void         sigplot_fini(SigPlot plot);
extern void         sigtrace_fini(SigTrace trc);
extern void         sigsess_fini(SigSess sess);
