#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "addr.h"
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
    assert(a->when_z > a->when);
    assert(addr_is_z(a));

    ADDR_ON_VALID(a, snap_v, a);
    ADDR_ON_Z(a, snap_z, a);

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
    assert(!addr_is_z(a));
    saw_v_count++;
    saw_v_data = a->value;
    saw_v_at = TAU;
}

static void snap_z(Addr a)
{
    assert(addr_is_z(a));
    saw_z_count++;
    saw_z_at = TAU;
}

static void check_addr_to_v(Addr a, Word v)
{
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
}

static void check_addr_to_z(Addr a)
{
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
}
