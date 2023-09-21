#pragma once
#include "subs.h"
#include "support.h"
#include "target.h"

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
// timing of signals. Callbacks on bus valid and bus "Z" should not
// be used to drive simulation logic.

typedef struct sData {
    Cstr                name;

    Byte                value;
    Subs                valid;
    Subs                z;
    int                 busy;
    Tau                 when;
    Tau                 when_z;
}                  *pData, Data[1];

// data_invar(a): check invariants for the Data
// - subs_invar succeeds on valid and z lists
// - data is not currently "busy"
// - last value set tau is not in the future
// - transition to Z is not in the future

extern void         data_invar(Data);

// data_init(a): initialise data, function call entry
// - name is the given string
// - initial state is high impedence
// - subscriber lists (valid and z) are empty
// - data is not busy
// - last value set tau is far in the past
// - transition to Z is in the recent past

extern void         data_init(Data, Cstr name);

// data_init(a): initialise data, macro entry
// - uses string form of a for the name.

#define DATA_INIT(d)	data_init(d, #d)

// data_on_valid(d,fp,ap): call fp(ap) on new value.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

extern void         data_on_valid(Data, StepFp, StepAp);

// data_on_z(d,fp,ap): call fp(ap) on Z transition.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

extern void         data_on_z(Data, StepFp, StepAp);

// DATA_ON_VALID(d,fp,ap): call fp(ap) on new value.
// This is the macro entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

#define DATA_ON_VALID(d,fp,ap) \
    data_on_valid(d, ((StepFp)(fp)), ((StepAp)(ap)))

// DATA_ON_Z(d,fp,ap): call fp(ap) on zing datas.
// This is the macro entry point, which converts the
// function and argument pointer to the storage type.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

#define DATA_ON_Z(d,fp,ap) \
    data_on_z(d, ((StepFp)(fp)), ((StepAp)(ap)))

// data_to(d,v): set the data value to the value v.
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

extern void         data_to(Data, Byte);

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

extern void         data_z(Data);

// data_is_z(Data): return true if Data is not driven

extern Bit          data_is_z(Data);

// data_post: Power-On Self Test for the Data code
//
// This function should be called every time the program starts
// before any code relies on Data not being completely broken.

extern void         data_post();

// data_bist: Power-On Self Test for the Data code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Data code is
// not working correctly.

extern void         data_bist();

// data_bench: performance verification for the Data code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Data.

extern void         data_bench();
