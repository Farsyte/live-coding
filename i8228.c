#include "i8228.h"

static void         latch_status(i8228 ctl);
static void         start_reads(i8228 ctl);
static void         start_writes(i8228 ctl);
static void         release_ctls(i8228 ctl);

// i8228_invar: verify the invariants for a i8228.

void i8228_invar(i8228 ctl)
{
    assert(ctl);
    assert(ctl->name);
    assert(ctl->name[0]);

    assert(ctl->DATA);

    edge_invar(ctl->STSTB_);
    edge_invar(ctl->DBIN);
    edge_invar(ctl->WR_);
    edge_invar(ctl->HLDA);

    edge_invar(ctl->MEMR_);
    edge_invar(ctl->MEMW_);
    edge_invar(ctl->IOR_);
    edge_invar(ctl->IOW_);
    edge_invar(ctl->INTA_);

}

// i8228_init(s): initialize the given i8228 to have this name
// and an initial state.

void i8228_init(i8228 ctl, Cstr name)
{
    assert(ctl);
    assert(name);
    assert(name[0]);

    ctl->name = name;

    pEdge               MEMR_ = ctl->MEMR_;
    pEdge               MEMW_ = ctl->MEMW_;
    pEdge               IOR_ = ctl->IOR_;
    pEdge               IOW_ = ctl->IOW_;
    pEdge               INTA_ = ctl->INTA_;

    edge_init(MEMR_, format("%s:/MEMR", name));
    edge_hi(MEMR_);

    edge_init(MEMW_, format("%s:/MEMW", name));
    edge_hi(MEMW_);

    edge_init(IOR_, format("%s:/IOR", name));
    edge_hi(IOR_);

    edge_init(IOW_, format("%s:/IOW", name));
    edge_hi(IOW_);

    edge_init(INTA_, format("%s:/INTA", name));
    edge_hi(INTA_);

    ctl->status = STATUS_RESET;
    ctl->activated = 0;
}

// i8228_linked(s): inputs are linked, finish set-up.

void i8228_linked(i8228 ctl)
{
    i8228_invar(ctl);

    pEdge               STSTB_ = ctl->STSTB_;
    pEdge               DBIN = ctl->DBIN;
    pEdge               WR_ = ctl->WR_;
    pEdge               HLDA = ctl->HLDA;

    assert(0 == STSTB_->value);
    assert(0 == DBIN->value);
    assert(1 == WR_->value);
    assert(0 == HLDA->value);

    EDGE_ON_FALL(STSTB_, latch_status, ctl);

    EDGE_ON_RISE(DBIN, start_reads, ctl);
    EDGE_ON_FALL(WR_, start_writes, ctl);

    EDGE_ON_FALL(DBIN, release_ctls, ctl);
    EDGE_ON_RISE(WR_, release_ctls, ctl);
    EDGE_ON_RISE(HLDA, release_ctls, ctl);
}

// latch_status(ctl): receive a new Status byte.
//
// copy the contents of the data bus into the internal latch,
// then initiate updates to the read control signals.

static void latch_status(i8228 ctl)
{
    ctl->status = *ctl->DATA;
}

// start_reads(ctl): activate the "read" cycle controls
//
// Activate the "read" control signals that are appropriate
// based on the latched status byte.

static void start_reads(i8228 ctl)
{
    pEdge               activated = 0;
    Byte                status = ctl->status;

    if ((status & STATUS_MEMR) && !(status & STATUS_HLTA))
        activated = ctl->MEMR_;
    else if (status & STATUS_INP)
        activated = ctl->IOR_;
    else if (status & STATUS_INTA)
        activated = ctl->INTA_;
    else
        return;

    ctl->activated = activated;
    edge_lo(activated);
}

// start_writes(ctl): activate the "write" cycle controls
//
// Activate the "write" control signals that are appropriate
// based on the latched status byte.

static void start_writes(i8228 ctl)
{
    pEdge               activated;
    if (ctl->status & STATUS_OUT)
        activated = ctl->IOW_;
    else
        activated = ctl->MEMW_;
    ctl->activated = activated;
    edge_lo(activated);
}

static void release_ctls(i8228 ctl)
{
    pEdge               activated = ctl->activated;
    if (activated) {
        ctl->activated = 0;
        ctl->status = STATUS_RESET;
        edge_hi(activated);
    }
}

// === === === === === === === === === === === === === === === ===
//                           TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static Data         DATA;

static Edge         STSTB_;
static Edge         DBIN;
static Edge         WR_;
static Edge         HLDA;

static i8228        ctl;

static void         i8228_test_init();

static void         check_outputs(Edge);
static void         check_case_nil(Byte status);
static void         check_case_rd(Edge, Byte status);
static void         check_case_rd_hlda(Edge, Byte status);
static void         check_case_wr(Edge e, Byte status);

// === === === === === === === === === === === === === === === ===
//                        POWER-ON SELF TEST
// === === === === === === === === === === === === === === === ===

// i8228_post: Power-On Self Test for the i8228 code
//
// This function should be called every time the program starts
// before any code relies on i8228 not being completely broken.

void i8228_post()
{
    i8228_test_init();

    pEdge               MEMR_ = ctl->MEMR_;
    pEdge               MEMW_ = ctl->MEMW_;
    pEdge               IOR_ = ctl->IOR_;
    pEdge               IOW_ = ctl->IOW_;
    pEdge               INTA_ = ctl->INTA_;

    // initially, NONE of our control outputs are active.
    check_outputs(0);

    // check preconditions
    assert(0 == STSTB_->value);
    assert(0 == HLDA->value);

    TAU++;

    // come out of reset
    edge_hi(STSTB_);
    check_outputs(0);

    check_case_nil(STATUS_RESET);

    check_case_rd(MEMR_, STATUS_FETCH);
    check_case_rd_hlda(MEMR_, STATUS_FETCH);

    check_case_rd(MEMR_, STATUS_MREAD);
    check_case_rd_hlda(MEMR_, STATUS_MREAD);

    check_case_wr(MEMW_, STATUS_MWRITE);

    check_case_rd(MEMR_, STATUS_SREAD);
    check_case_rd_hlda(MEMR_, STATUS_SREAD);

    check_case_wr(MEMW_, STATUS_SWRITE);

    check_case_rd(IOR_, STATUS_INPUTRD);
    check_case_rd_hlda(IOR_, STATUS_INPUTRD);

    check_case_wr(IOW_, STATUS_OUTPUTWR);

    check_case_rd(INTA_, STATUS_INTACK);
    check_case_rd_hlda(INTA_, STATUS_INTACK);

    check_case_nil(STATUS_HALTACK);

    check_case_rd(INTA_, STATUS_INTACKW);
    check_case_rd_hlda(INTA_, STATUS_INTACKW);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

// i8228_bist: Power-On Self Test for the i8228 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the i8228 code is
// not working correctly.

void i8228_bist()
{
    i8228_test_init();

    pEdge               MEMR_ = ctl->MEMR_;
    pEdge               MEMW_ = ctl->MEMW_;
    pEdge               IOR_ = ctl->IOR_;
    pEdge               IOW_ = ctl->IOW_;
    pEdge               INTA_ = ctl->INTA_;

    // initially, NONE of our control outputs are active.
    check_outputs(0);

    // check preconditions
    assert(0 == STSTB_->value);
    assert(0 == HLDA->value);

    TAU++;

    // come out of reset
    edge_hi(STSTB_);
    check_outputs(0);

    check_case_nil(STATUS_RESET);

    check_case_rd(MEMR_, STATUS_FETCH);
    check_case_rd_hlda(MEMR_, STATUS_FETCH);

    check_case_rd(MEMR_, STATUS_MREAD);
    check_case_rd_hlda(MEMR_, STATUS_MREAD);

    check_case_wr(MEMW_, STATUS_MWRITE);

    check_case_rd(MEMR_, STATUS_SREAD);
    check_case_rd_hlda(MEMR_, STATUS_SREAD);

    check_case_wr(MEMW_, STATUS_SWRITE);

    check_case_rd(IOR_, STATUS_INPUTRD);
    check_case_rd_hlda(IOR_, STATUS_INPUTRD);

    check_case_wr(IOW_, STATUS_OUTPUTWR);

    check_case_rd(INTA_, STATUS_INTACK);
    check_case_rd_hlda(INTA_, STATUS_INTACK);

    check_case_nil(STATUS_HALTACK);

    check_case_rd(INTA_, STATUS_INTACKW);
    check_case_rd_hlda(INTA_, STATUS_INTACKW);
}

// === === === === === === === === === === === === === === === ===
//                           BENCHMARKING
// === === === === === === === === === === === === === === === ===

static void bench_i8228(void *arg)
{
    (void)arg;          // not used by this worker
}

// i8228_bench: performance verification for the i8228 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of i8228.

void i8228_bench()
{
    i8228_test_init();

    double              dt = RTC_ELAPSED(bench_i8228, 0);

    BENCH_TOP("i8228");
    BENCH_VAL(dt);
    BENCH_END();
}

// === === === === === === === === === === === === === === === ===
//                           TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static void i8228_test_init()
{
    EDGE_INIT(STSTB_);
    EDGE_INIT(DBIN);
    EDGE_INIT(WR_);
    EDGE_INIT(HLDA);

    DATA[0] = STATUS_RESET;

    edge_lo(STSTB_);
    edge_lo(DBIN);
    edge_hi(WR_);
    edge_lo(HLDA);

    I8228_INIT(ctl);

    ctl->DATA = DATA;

    ctl->STSTB_ = STSTB_;
    ctl->DBIN = DBIN;
    ctl->WR_ = WR_;
    ctl->HLDA = HLDA;

    i8228_linked(ctl);
}

static void check_case_nil(Byte status)
{
    TAU += 1;

    *DATA = status;

    edge_lo(STSTB_);
    check_outputs(0);

    TAU += 1;

    edge_hi(STSTB_);
    check_outputs(0);

    TAU += 1;
}

static void check_case_rd(Edge e, Byte status)
{
    TAU += 1;

    *DATA = status;

    edge_lo(STSTB_);
    check_outputs(0);

    TAU += 1;

    edge_hi(STSTB_);
    check_outputs(0);

    TAU += 1;

    edge_hi(DBIN);
    check_outputs(e);

    TAU += 1;

    edge_lo(DBIN);
    check_outputs(0);
}

static void check_case_rd_hlda(Edge e, Byte status)
{
    TAU += 1;

    *DATA = status;

    edge_lo(STSTB_);
    check_outputs(0);

    TAU += 1;

    edge_hi(STSTB_);
    check_outputs(0);

    TAU += 1;

    edge_hi(DBIN);
    check_outputs(e);

    TAU += 1;

    edge_hi(HLDA);
    check_outputs(0);

    TAU += 1;

    edge_lo(DBIN);
    check_outputs(0);

    TAU += 1;

    edge_lo(HLDA);
    check_outputs(0);
}

static void check_case_wr(Edge e, Byte status)
{
    TAU += 1;

    *DATA = status;

    edge_lo(STSTB_);
    check_outputs(0);

    TAU += 1;

    edge_hi(STSTB_);
    check_outputs(0);

    TAU += 1;

    edge_lo(WR_);
    check_outputs(e);

    TAU += 1;

    edge_hi(WR_);
    check_outputs(0);
}

static void check_outputs(Edge e)
{
    pEdge               MEMR_ = ctl->MEMR_;
    if (e == MEMR_) {
        ASSERT_EQ_integer(0, MEMR_->value);
    } else {
        ASSERT_EQ_integer(1, MEMR_->value);
    }
    pEdge               MEMW_ = ctl->MEMW_;
    if (e == MEMW_) {
        ASSERT_EQ_integer(0, MEMW_->value);
    } else {
        ASSERT_EQ_integer(1, MEMW_->value);
    }
    pEdge               IOR_ = ctl->IOR_;
    if (e == IOR_) {
        ASSERT_EQ_integer(0, IOR_->value);
    } else {
        ASSERT_EQ_integer(1, IOR_->value);
    }
    pEdge               IOW_ = ctl->IOW_;
    if (e == IOW_) {
        ASSERT_EQ_integer(0, IOW_->value);
    } else {
        ASSERT_EQ_integer(1, IOW_->value);
    }
    pEdge               INTA_ = ctl->INTA_;
    if (e == INTA_) {
        ASSERT_EQ_integer(0, INTA_->value);
    } else {
        ASSERT_EQ_integer(1, INTA_->value);
    }
}
