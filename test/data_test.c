#include "data.h"
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

static unsigned     saw_v_count;
static Byte         saw_v_data;
static Tau          saw_v_at;

static unsigned     saw_z_count;
static Tau          saw_z_at;

static void         snap_v(Data);
static void         snap_z(Data);

static void         check_data_to_v(Data, Byte);
static void         check_data_to_z(Data);

// data_post: Power-On Self Test for the Data code
//
// This function should be called every time the program starts
// before any code relies on Data not being completely broken.

void data_post()
{
}

// data_bist: Power-On Self Test for the Data code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the Data code is
// not working correctly.

void data_bist()
{
    Data                d;
    DATA_INIT(d);
    data_invar(d);
    assert(0 == d->value);
    assert(0 == d->valid->len);
    assert(0 == d->z->len);
    assert(0 == d->busy);
    assert(d->when_z > d->when);
    assert(data_is_z(d));

    DATA_ON_VALID(d, snap_v, d);
    DATA_ON_Z(d, snap_z, d);

    check_data_to_z(d);
    check_data_to_v(d, 0x00);
    check_data_to_v(d, 0xFF);
    check_data_to_z(d);
    check_data_to_v(d, 0xFF);
    check_data_to_z(d);
    check_data_to_z(d);
    check_data_to_v(d, 0xAA);
    check_data_to_v(d, 0xAA);
    check_data_to_v(d, 0x55);
    check_data_to_z(d);
}

// data_bench: performance verification for the Data code
//
// This function should be called as needed to measure the performance
// of the time critical parts of Data.

void data_bench()
{
}

static void snap_v(Data d)
{
    assert(!data_is_z(d));
    saw_v_count++;
    saw_v_data = d->value;
    saw_v_at = TAU;
}

static void snap_z(Data d)
{
    assert(data_is_z(d));
    saw_z_count++;
    saw_z_at = TAU;
}

static void check_data_to_v(Data d, Byte v)
{
    unsigned            sv_v_count = saw_v_count;
    Byte                sv_v_data = saw_v_data;
    Tau                 sv_v_at = saw_v_at;

    int                 was_z = data_is_z(d);

    data_to(d, v);

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
    // No matter what, the Data "when" should be updated.
    ASSERT_EQ_integer(TAU, d->when);

    TAU++;
}

static void check_data_to_z(Data d)
{
    unsigned            sv_z_count = saw_z_count;
    Tau                 sv_z_at = saw_z_at;
    Bit                 was_z = data_is_z(d);

    data_z(d);

    if (was_z) {
        ASSERT_EQ_integer(sv_z_count, saw_z_count);
        ASSERT_EQ_integer(sv_z_at, saw_z_at);
    } else {
        ASSERT_EQ_integer(sv_z_count + 1, saw_z_count);
        ASSERT_EQ_integer(TAU, saw_z_at);
    }
}
