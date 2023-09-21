#include "addr.h"
#include "stepat.h"
#include "support.h"

// This compile time option disables the code that runs the subscriber
// lists, and can be used for an unmeasured but potentially modest
// performance boost when the simulation involves frequent changes
// to "Addr" items, which includes not only the Addr bus but all of
// the sixteen-bit registers and latches.
//
// If this is on, timing diagrams will not show updates to these values.

#undef	ADDR_NO_SUBS

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
// timing of signals. Callbacks on bus valid and bus "Z" should not
// be used to drive simulation logic.

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
#ifndef	ADDR_NO_SUBS
    a->when = TAU - 2;
    a->when_z = TAU - 1;
#else                                           /* ADDR_NO_SUBS */
    a->when = 0;
    a->when_z = 0;
#endif                                          /* ADDR_NO_SUBS */
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
#ifndef	ADDR_NO_SUBS
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
#else                                           /* ADDR_NO_SUBS */
    a->value = v;
#endif                                          /* ADDR_NO_SUBS */
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
#ifndef	ADDR_NO_SUBS
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
#else                                           /* ADDR_NO_SUBS */
    // continue to provide the weak indicator of high-z stats
    a->value = ~0;
#endif                                          /* ADDR_NO_SUBS */
}

// addr_is_z(Addr): return true if Addr is not driven

Bit addr_is_z(Addr a)
{
    return a->when_z > a->when;
}

// test code below this line

static unsigned     saw_v_count;
static Word         saw_v_data;
static Tau          saw_v_at;

static unsigned     saw_z_count;
static Tau          saw_z_at;

static void         snap_v(Addr);
static void         snap_z(Addr);

static void         check_addr_to_v(Addr, Word);
static void         check_addr_to_z(Addr);

// addr_post: Power-On Self Test for the Addr code
//
// This function should be called every time the program starts
// before any code relies on Addr not being completely broken.

void addr_post()
{
}

// addr_bist: Power-On Self Test for the Addr code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Addr code is
// not working correctly.

void addr_bist()
{
    Addr                a;
    ADDR_INIT(a);
    addr_invar(a);
    assert(0 == a->value);
    assert(0 == a->valid->len);
    assert(0 == a->z->len);
    assert(0 == a->busy);
#ifndef	ADDR_NO_SUBS
    assert(a->when_z > a->when);
    assert(addr_is_z(a));

    ADDR_ON_VALID(a, snap_v, a);
    ADDR_ON_Z(a, snap_z, a);
#else                                           /* ADDR_NO_SUBS */
    ASSERT_EQ_integer(0, a->when);
    ASSERT_EQ_integer(0, a->when_z);
    (void)snap_v(a);
    (void)snap_z(a);
#endif                                          /* ADDR_NO_SUBS */

    check_addr_to_z(a);
    check_addr_to_v(a, 0x0000);
    check_addr_to_v(a, 0xFFFF);
    check_addr_to_z(a);
    check_addr_to_v(a, 0xFFFF);
    check_addr_to_z(a);
    check_addr_to_z(a);
    check_addr_to_v(a, 0xAAAA);
    check_addr_to_v(a, 0xAAAA);
    check_addr_to_v(a, 0x5555);
    check_addr_to_z(a);
}

// addr_bench: performance verification for the Addr code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Addr.

void addr_bench()
{
}

static void snap_v(Addr a)
{
#ifndef	ADDR_NO_SUBS
    assert(!addr_is_z(a));
#else                                           /* ADDR_NO_SUBS */
    (void)a;
#endif                                          /* ADDR_NO_SUBS */
    saw_v_count++;
    saw_v_data = a->value;
    saw_v_at = TAU;
}

static void snap_z(Addr a)
{
#ifndef	ADDR_NO_SUBS
    assert(addr_is_z(a));
#else                                           /* ADDR_NO_SUBS */
    (void)a;
#endif                                          /* ADDR_NO_SUBS */
    saw_z_count++;
    saw_z_at = TAU;
}

static void check_addr_to_v(Addr a, Word v)
{
#ifndef	ADDR_NO_SUBS
    unsigned            sv_v_count = saw_v_count;
    Word                sv_v_data = saw_v_data;
    Tau                 sv_v_at = saw_v_at;

    int                 was_z = addr_is_z(a);

    addr_to(a, v);

    if (v == sv_v_data && !was_z) {
        // there should have been no callback.
        ASSERT_EQ_integer(sv_v_count, saw_v_count);
        ASSERT_EQ_integer(sv_v_data, saw_v_data);
        ASSERT_EQ_integer(sv_v_at, saw_v_at);
    } else {
        ASSERT_EQ_integer(sv_v_count + 1, saw_v_count);
        ASSERT_EQ_integer(v, saw_v_data);
        ASSERT_EQ_integer(TAU, saw_v_at);
    }
    // No matter what, the Addr "when" should be updated.
    ASSERT_EQ_integer(TAU, a->when);

    TAU++;
#else                                           /* ADDR_NO_SUBS */
    addr_to(a, v);
    TAU++;
#endif                                          /* ADDR_NO_SUBS */
}

static void check_addr_to_z(Addr a)
{
#ifndef	ADDR_NO_SUBS

    unsigned            sv_z_count = saw_z_count;
    Tau                 sv_z_at = saw_z_at;
    Bit                 was_z = addr_is_z(a);

    addr_z(a);

    if (was_z) {
        ASSERT_EQ_integer(sv_z_count, saw_z_count);
        ASSERT_EQ_integer(sv_z_at, saw_z_at);
    } else {
        ASSERT_EQ_integer(sv_z_count + 1, saw_z_count);
        ASSERT_EQ_integer(TAU, saw_z_at);
    }

#else                                           /* ADDR_NO_SUBS */
    addr_z(a);
#endif                                          /* ADDR_NO_SUBS */
}
