#pragma once
#include "common/subs.h"
#include "simext/support.h"
#include "wa16/target.h"

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

typedef struct sAddr {
    Cstr                name;

    Word                value;
    Tau                 when;
    Tau                 when_z;
    Subs                valid;
    Subs                z;
    int                 busy;
}                  *pAddr, Addr[1];

// addr_invar(a): check invariants for the Addr
// - subs_invar succeeds on valid and z lists
// - addr is not currently "busy"
// - last value set tau is not in the future
// - transition to Z is not in the future

extern void         addr_invar(Addr);

// addr_init(a): initialise addr, function call entry
// - name is the given string
// - initial state is high impedence
// - subscriber lists (valid and z) are empty
// - addr is not busy
// - last value set tau is far in the past
// - transition to Z is in the recent past

extern void         addr_init(Addr, Cstr name);

// addr_init(a): initialise addr, macro entry
// - uses string form of a for the name.

#define ADDR_INIT(a)	addr_init(a, #a)

// addr_on_valid(a,fp,ap): call fp(ap) on new value.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

extern void         addr_on_valid(Addr, StepFp, StepAp);

// addr_on_z(a,fp,ap): call fp(ap) on Z transition.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

extern void         addr_on_z(Addr, StepFp, StepAp);

// ADDR_ON_VALID(a,fp,ap): call fp(ap) on new value.
// This is the macro entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

#define ADDR_ON_VALID(a,fp,ap) \
    addr_on_valid(a, ((StepFp)(fp)), ((StepAp)(ap)))

// ADDR_ON_Z(a,fp,ap): call fp(ap) on zing addrs.
// This is the macro entry point, which converts the
// function and argument pointer to the storage type.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

#define ADDR_ON_Z(a,fp,ap) \
    addr_on_z(a, ((StepFp)(fp)), ((StepAp)(ap)))

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

extern void         addr_to(Addr, Word);

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

extern void         addr_z(Addr);

// addr_is_z(Addr): return true if Addr is not driven

extern Bit          addr_is_z(Addr);

// addr_post: Power-On Self Test for the Addr code
//
// This function should be called every time the program starts
// before any code relies on Addr not being completely broken.

extern void         addr_post();

// addr_bist: Power-On Self Test for the Addr code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Addr code is
// not working correctly.

extern void         addr_bist();

// addr_bench: performance verification for the Addr code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Addr.

extern void         addr_bench();
