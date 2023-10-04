#include "data.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "stepat.h"
#include "support.h"

// data: a signal bus with callbacks for changes in state.
//
// This facility tracks the value of an dataess bus in the simulated
// system, and triggers callbacks on change in state. It has built-in
// protection against infinite recursion (where the value of the data
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

// data_invar(d): check invariants for the Data
// - subs_invar succeeds on valid and z lists
// - data is not currently "busy"
// - transition to Z is not in the future

void data_invar(Data d)
{
    assert(d);
    assert(d->name);
    assert(d->name[0]);

    subs_invar(d->valid);
    subs_invar(d->z);
    assert(!d->busy);
    assert(d->when <= TAU);
    assert(d->when_z <= TAU);
}

// data_init(d): initialise data
// - initial state is LOW
// - subscriber lists (valid and z) are empty
// - data is not busy
// - last value set tau is in the past

void data_init(Data d, Cstr name)
{
    assert(d);
    assert(name);
    assert(name[0]);

    d->name = name;
    d->value = 0;
    subs_init(d->valid);
    subs_init(d->z);
    d->busy = 0;
    d->when = TAU - 2;
    d->when_z = TAU - 1;
    data_invar(d);
}

// data_on_valid(a,fp,ap): call fp(ap) on new value.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void data_on_valid(Data d, StepFp fp, StepAp ap)
{
    data_invar(d);
    SUBS_ADD(d->valid, fp, ap);
    data_invar(d);
}

// data_on_z(a,fp,ap): call fp(ap) on Z transition.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

void data_on_z(Data d, StepFp fp, StepAp ap)
{
    data_invar(d);
    SUBS_ADD(d->z, fp, ap);
    data_invar(d);
}

// data_to(a,v): set the data value to the value v.
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

void data_to(Data d, Byte v)
{
    if (data_is_z(d) || (d->value != v)) {
        // do real work if state changes

        // NOTE: periodically monitor the performance hit
        // from these asserts and the supporting code,
        // and make sure the benefit outweighs the cost.

        assert(d->when < TAU);  // detect hazard
        assert(!d->busy);       // protect vs infinite recursion
        d->busy = 1;
        d->value = v;
        d->when = TAU;
        subs_run(d->valid);
        d->busy = 0;
    } else {
        // If there is not state change, just remember
        // that someone asserts the value on the bus
        // at this time is this value, for hazard detection.
        d->when = TAU;
    }
}

// data_z(d): set the bus to the Z state.
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

void data_z(Data d)
{
    if (!data_is_z(d)) {
        // do real work if state changes

        // NOTE: periodically monitor the performance hit
        // from these asserts and the supporting code,
        // and make sure the benefit outweighs the cost.

        assert(d->when < TAU);  // detect hazard
        assert(!d->busy);       // protect vs infinite recursion
        d->busy = 1;
        d->value = ~0;
        d->when_z = TAU;
        subs_run(d->z);
        d->busy = 0;
    }
    // NOTE: if we are already Z, we do not even care about recording
    // the time for additional "I'm releasing this bus" events.
}

// data_is_z(Data): return true if Data is not driven

Bit data_is_z(Data d)
{
    return d->when_z > d->when;
}
