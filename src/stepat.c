#include "stepat.h"
#include "support.h"

// stepat: a function to call, a pointer to pass it, and a Tau.

// The "stepat_invar(StepAt s)" method checks invariants on the stepat.

void stepat_invar(StepAt s)
{
    assert(s);
    assert(s->at >= TAU);
    assert(s->fp);
}

// The "stepat_run(StepAt s)" method executes the stepat.

void stepat_run(StepAt s)
{
    TAU = s->at;
    s->fp(s->ap);
}
