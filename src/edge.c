#include "common/edge.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "common/stepat.h"
#include "simext/support.h"

// edge: a signal with callbacks for rising and falling edges
//
// This facility tracks the value of a signal in the simulated system,
// and triggers callbacks on rising and falling edges. It has built-in
// protection against infinite recursion (where the value of the edge is
// changed during a callback), and detection of some hazards.

// edge_invar(e): check invariants for the Edge
// - edge is either high or low
// - subs_invar succeeds on rise and fall lists
// - edge is not currently "busy"
// - last change tau is not in the future

void edge_invar(Edge e)
{
    assert(e);
    assert(e->name);
    assert(e->name[0]);

    Bit                 value = e->value;
    pSubs               rise = e->rise;
    pSubs               fall = e->fall;

    assert(value == 0 || value == 1);
    subs_invar(rise);
    subs_invar(fall);
    assert(e->when <= TAU);
}

// edge_init(e): initialise edge
// - initial state is LOW
// - subscriber lists (rise and fall) are empty
// - edge is not busy
// - last value set tau is in the past

void edge_init(Edge e, Cstr name, Bit v)
{
    assert(e);
    assert(name);
    assert(name[0]);

    e->name = name;
    e->value = v;
    subs_init(e->rise);
    subs_init(e->fall);
    e->busy = 0;
    e->when = TAU - 1;
    edge_invar(e);
}

// edge_on_rise(e,fp,ap): call fp(ap) on rising edges.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void edge_on_rise(Edge e, StepFp fp, StepAp ap)
{
    edge_invar(e);
    SUBS_ADD(e->rise, fp, ap);
    edge_invar(e);
}

// edge_on_fall(e,fp,ap): call fp(ap) on falling edges.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void edge_on_fall(Edge e, StepFp fp, StepAp ap)
{
    edge_invar(e);
    SUBS_ADD(e->fall, fp, ap);
    edge_invar(e);
}

// edge_first_on_rise(e,fp,ap): call fp(ap) on rising edges.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void edge_first_on_rise(Edge e, StepFp fp, StepAp ap)
{
    edge_invar(e);
    SUBS_FIRST(e->rise, fp, ap);
    edge_invar(e);
}

// edge_first_on_fall(e,fp,ap): call fp(ap) on falling edges.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void edge_first_on_fall(Edge e, StepFp fp, StepAp ap)
{
    edge_invar(e);
    SUBS_FIRST(e->fall, fp, ap);
    edge_invar(e);
}

// edge_to(e,v): set the edge value to the value v.
// if it changes, notify subscribers on the appropriate list.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
//
// THIS IS A TIME-CRITICAL FUNCTION.

void edge_to(Edge e, Bit v)
{
    if (e->value != v) {
        // do real work if state changes

        assert(e->when < TAU);  // detect hazard
        assert(!e->busy);       // protect vs infinite recursion
        e->busy = 1;

        e->value = v;

        if (v)
            subs_run(e->rise);
        else
            subs_run(e->fall);

        e->busy = 0;
    }
    // remember tau from every time someone specifies
    // the value, even if the value does not change.
    e->when = TAU;
}

// edge_hi(e): set the edge value to HIGH.
// if it was low, notify subscribers on the "rise" list.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
//
// This is a time-critical function.

void edge_hi(Edge e)
{
    edge_to(e, 1);
}

// edge_lo(e): set the edge value to LOW.
// if it was high, notify subscribers on the "fall" list.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
//
// This is a time-critical function.

void edge_lo(Edge e)
{
    edge_to(e, 0);
}
