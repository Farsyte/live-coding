#include "common/sigtrace_data.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <errno.h>      // Macros reporting error conditions
#include <stdlib.h>     // General utilities
#include <string.h>     // String handling
#include <sys/stat.h>
#include <sys/types.h>

static void         sig_dv(SigTraceData trc);
static void         sig_dz(SigTraceData trc);

void sigtrace_data_init(SigTraceData trc, SigSess sess, pData data)
{
    assert(trc);
    assert(sess);
    data_invar(data);

    sigtrace_init(trc->base, sess, data->name);
    trc->data = data;

    DATA_ON_VALID(data, sig_dv, trc);
    DATA_ON_Z(data, sig_dz, trc);
    sig_dz(trc);
}

static void sig_dv(SigTraceData trc)
{
    sigtrace_log(trc->base, '=', format("%02X", trc->data->value));
}

static void sig_dz(SigTraceData trc)
{
    sigtrace_log(trc->base, 'z', 0);
}
