#include "wa16/sigtrace_addr.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <errno.h>      // Macros reporting error conditions
#include <stdlib.h>     // General utilities
#include <string.h>     // String handling
#include <sys/stat.h>
#include <sys/types.h>

void sigtrace_init_addr(SigTrace trc, SigSess sess, pAddr addr)
{
    (void)trc;
    (void)sess;
    (void)addr;
    FAIL("deprectated");
}

#define SigTrace SigTraceAddr

static void         sig_av(SigTraceAddr trc);
static void         sig_az(SigTraceAddr trc);

void sigtrace_addr_init(SigTraceAddr trc, SigSess sess, pAddr addr)
{
    assert(trc);
    assert(sess);
    addr_invar(addr);

    sigtrace_init(trc->base, sess, addr->name);
    trc->addr = addr;

    ADDR_ON_VALID(addr, sig_av, trc);
    ADDR_ON_Z(addr, sig_az, trc);
    sig_az(trc);
}

static void sig_av(SigTraceAddr trc)
{
    sigtrace_log(trc->base, '=', format("%04X", trc->addr->value));
}

static void sig_az(SigTraceAddr trc)
{
    sigtrace_log(trc->base, 'z', 0);
}
