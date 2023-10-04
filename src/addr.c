#include "addr.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "stepat.h"
#include "support.h"

// addr: a signal bus with callbacks for changes in state.
//
// This facility tracks the value of an address bus in the simulated
// system, and triggers callbacks on change in state. It has built-in
// protection against infinite recursion (where the value of the addr
// is changed during a callback), and detection of some hazards.
//
// The state of the bus includes an implicit "Z" (high impedence)
// state where a transition to "Z" can be followed by a valid value
// within the same TAU.
//
// This facility exists to support display and validation of the
// timing of signals. Callbacks on bus valid and bus "Z" can be used
// to record transitions into a trace, but should not be used to drive
// simulation logic.

// addr_invar(a): check invariants for the Addr
// - subs_invar succeeds on valid and z lists
// - addr is not currently "busy"
// - transition to Z is not in the future

void addr_invar(Addr a)
{
    assert(a);
    assert(a->name);
    assert(a->name[0]);

    subs_invar(a->valid);
    subs_invar(a->z);
    assert(!a->busy);
    assert(a->when <= TAU);
    assert(a->when_z <= TAU);
}

// addr_init(a): initialise addr
// - initial state is LOW
// - subscriber lists (valid and z) are empty
// - addr is not busy
// - last value set tau is in the past

void addr_init(Addr a, Cstr name)
{
    assert(a);
    assert(name);
    assert(name[0]);

    a->name = name;
    a->value = 0;
    subs_init(a->valid);
    subs_init(a->z);
    a->busy = 0;
    a->when = TAU - 2;
    a->when_z = TAU - 1;
    addr_invar(a);
}

// addr_on_valid(a,fp,ap): call fp(ap) on new value.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void addr_on_valid(Addr a, StepFp fp, StepAp ap)
{
    addr_invar(a);
    SUBS_ADD(a->valid, fp, ap);
    addr_invar(a);
}

// addr_on_z(a,fp,ap): call fp(ap) on Z transition.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void addr_on_z(Addr a, StepFp fp, StepAp ap)
{
    addr_invar(a);
    SUBS_ADD(a->z, fp, ap);
    addr_invar(a);
}

// addr_to(a,v): set the addr value to the value v.
// If the state changed (or the bus was in Z state), then
// the "valid" subscribers are notified after the state is
// updated.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
// Note that "when_z" at the current time does not indicate
// a hazard.
//
// This is a time-critical function.

void addr_to(Addr a, Word v)
{
    if (addr_is_z(a) || (a->value != v)) {
        // do real work if state changes

        // NOTE: periodically monitor the performance hit
        // from these asserts and the supporting code,
        // and make sure the benefit outweighs the cost.

        assert(a->when < TAU);  // detect hazard
        assert(!a->busy);       // protect vs infinite recursion
        a->busy = 1;
        a->value = v;
        a->when = TAU;
        subs_run(a->valid);
        a->busy = 0;
    } else {
        // If there is not state change, just remember
        // that someone asserts the value on the bus
        // at this time is this value, for hazard detection.
        a->when = TAU;
    }
}

// addr_z(a): set the bus to the Z state.
// The bus is marked to be in the Z state (the content of
// the value storage is not modified).
// If the bus was not in the Z state, the "z" subscribers
// are notified that the bus state has changed.

// hazard detect: assert that the "when" value is in the past,
// as it is not valid for the state of the bus to be set,
// then transition to "Z", in a single TAU.
// However, a bus transition to "Z" followed by new data in
// the same TAU is a common and allowed pattern.
// Always set the "when_z" value to the current TAU.
// changing or not, set "when_z" to the current time.
//
// This is a time-critical function.

void addr_z(Addr a)
{
    if (!addr_is_z(a)) {
        // do real work if state changes

        // NOTE: periodically monitor the performance hit
        // from these asserts and the supporting code,
        // and make sure the benefit outweighs the cost.

        assert(a->when < TAU);  // detect hazard
        assert(!a->busy);       // protect vs infinite recursion
        a->busy = 1;
        a->value = ~0;
        a->when_z = TAU;
        subs_run(a->z);
        a->busy = 0;
    }
    // NOTE: if we are already Z, we do not even care about recording
    // the time for additional "I'm releasing this bus" events.
}

// addr_is_z(Addr): return true if Addr is not driven

Bit addr_is_z(Addr a)
{
    return a->when_z > a->when;
}
