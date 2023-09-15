#pragma once
#include "subs.h"
#include "support.h"
#include "target.h"

// edge: a signal with callbacks for rising and falling edges
//
// This facility tracks the value of a signal in the simulated system,
// and triggers callbacks on rising and falling edges. It has built-in
// protection against infinite recursion (where the value of the edge is
// changed during a callback), and detection of some hazards.

typedef struct sEdge {
    Cstr                name;

    Bit                 value;
    Subs                rise;
    Subs                fall;
    int                 busy;
    Tau                 when;
}                  *pEdge, Edge[1];

// edge_invar(e): check invariants for the Edge
// - edge is either high or low
// - subs_invar succeeds on rise and fall lists
// - edge is not currently "busy"
// - last value set tau is not in the future

extern void         edge_invar(Edge);

// edge_init(e): initialise edge, function call entry
// - name is the given string
// - initial state is LOW
// - subscriber lists (rise and fall) are empty
// - edge is not busy
// - last value set tau is in the past

extern void         edge_init(Edge, Cstr name);

// edge_init(e): initialise edge, macro entry
// - uses string form of e for the name.

#define EDGE_INIT(e)	edge_init(e, #e)

// edge_on_rise(e,fp,ap): call fp(ap) on rising edges.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

extern void         edge_on_rise(Edge, StepFp, StepAp);

// edge_on_fall(e,fp,ap): call fp(ap) on falling edges.
// This is the function entry point, which demands the
// parameters are of the data type being stored.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

extern void         edge_on_fall(Edge, StepFp, StepAp);

// EDGE_ON_RISE(e,fp,ap): call fp(ap) on rising edges.
// This is the macro entry point, which converts the
// function and argument pointer to the storage type.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

#define EDGE_ON_RISE(e,fp,ap) \
    edge_on_rise(e, ((StepFp)(fp)), ((StepAp)(ap)))

// EDGE_ON_FALL(e,fp,ap): call fp(ap) on falling edges.
// This is the macro entry point, which converts the
// function and argument pointer to the storage type.
//
// Intended to be used during initialization.
// Do not use in any any time-critical path.

#define EDGE_ON_FALL(e,fp,ap) \
    edge_on_fall(e, ((StepFp)(fp)), ((StepAp)(ap)))

// edge_hi(e): set the edge value to HIGH.
// if it was low, notify subscribers on the "rise" list.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
//
// This is a time-critical function.

extern void         edge_hi(Edge);

// edge_lo(e): set the edge value to LOW.
// if it was high, notify subscribers on the "fall" list.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
//
// This is a time-critical function.

extern void         edge_lo(Edge);

// edge_to(e,v): set the edge value to the value v.
// if it changes, notify subscribers on the appropriate list.
// recursion protection: assert busy is not set, then
// set busy during the subscriber notification.
// hazard detect: if the value is changing, assert that
// the "when" value is in the past; whether changing or
// not, set "when" to the current time.
//
// This is a time-critical function.

extern void         edge_to(Edge, Bit);

// edge_post: Power-On Self Test for the Edge code
//
// This function should be called every time the program starts
// before any code relies on Edge not being completely broken.

extern void         edge_post();

// edge_bist: Power-On Self Test for the Edge code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Edge code is
// not working correctly.

extern void         edge_bist();

// edge_bench: performance verification for the Edge code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Edge.

extern void         edge_bench();
