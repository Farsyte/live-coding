#include "common/step.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "simext/support.h"

// step: a function to call, and a pointer to pass it.

void step_invar(Step s)
{
    assert(s);
    assert(s->fp);
}

// step_run(s): run the referenced step.
// NOTE: This is in the critical timing path.

void step_run(Step s)
{
    s->fp(s->ap);
}
