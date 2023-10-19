#include "common/sigtrace_edge.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <errno.h>      // Macros reporting error conditions
#include <stdlib.h>     // General utilities
#include <string.h>     // String handling
#include <sys/stat.h>
#include <sys/types.h>

static void         sig_rise(SigTraceEdge trc);
static void         sig_fall(SigTraceEdge trc);

void sigtrace_edge_init(SigTraceEdge trc, SigSess sess, pEdge edge)
{
    assert(trc);
    assert(sess);
    edge_invar(edge);

    sigtrace_init(trc->base, sess, edge->name);
    trc->edge = edge;

    EDGE_ON_RISE(edge, sig_rise, trc);
    EDGE_ON_FALL(edge, sig_fall, trc);

    if (edge->value)
        sig_rise(trc);
    else
        sig_fall(trc);
}

static void sig_rise(SigTraceEdge trc)
{
    sigtrace_log(trc->base, '1', 0);
}

static void sig_fall(SigTraceEdge trc)
{
    sigtrace_log(trc->base, '0', 0);
}
