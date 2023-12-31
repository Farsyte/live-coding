#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "chip/i8080_test.h"
#include "chip/i8228.h"
#include "common/sigtrace.h"
#include "common/sigtrace_data.h"
#include "common/sigtrace_edge.h"

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
    SigSess             ss;
    SigTraceData        trace_DATA;
    SigTraceEdge        trace_STSTB_;
    SigTraceEdge        trace_DBIN;
    SigTraceEdge        trace_WR_;
    SigTraceEdge        trace_HLDA;
    SigTraceEdge        trace_MEMR_;
    SigTraceEdge        trace_MEMW_;
    SigTraceEdge        trace_IOR_;
    SigTraceEdge        trace_IOW_;
    SigTraceEdge        trace_INTA_;
    SigPlot             pl;
    Tau                 tau_min;

    i8228_test_init();

    pEdge               MEMR_ = ctl->MEMR_;
    pEdge               MEMW_ = ctl->MEMW_;
    pEdge               IOR_ = ctl->IOR_;
    pEdge               IOW_ = ctl->IOW_;
    pEdge               INTA_ = ctl->INTA_;

    sigsess_init(ss, "i8228_bist");
    sigtrace_data_init(trace_DATA, ss, DATA);
    sigtrace_edge_init(trace_STSTB_, ss, STSTB_);
    sigtrace_edge_init(trace_DBIN, ss, DBIN);
    sigtrace_edge_init(trace_WR_, ss, WR_);
    sigtrace_edge_init(trace_HLDA, ss, HLDA);
    sigtrace_edge_init(trace_MEMR_, ss, MEMR_);
    sigtrace_edge_init(trace_MEMW_, ss, MEMW_);
    sigtrace_edge_init(trace_IOR_, ss, IOR_);
    sigtrace_edge_init(trace_IOW_, ss, IOW_);
    sigtrace_edge_init(trace_INTA_, ss, INTA_);

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

    tau_min = TAU;
    check_case_rd(MEMR_, STATUS_FETCH);
    sigplot_init(pl, ss, "i8228_bist_fetch",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical FETCH cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_MEMR_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd_hlda(MEMR_, STATUS_FETCH);
    sigplot_init(pl, ss, "i8228_bist_fetch_with_hold",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a FETCH ended by HOLD", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_HLDA->base, pl);
    sigtrace_plot(trace_MEMR_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd(MEMR_, STATUS_MREAD);
    sigplot_init(pl, ss, "i8228_bist_mread",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical MREAD cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_MEMR_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd_hlda(MEMR_, STATUS_MREAD);
    sigplot_init(pl, ss, "i8228_bist_mread_with_hold",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a MREAD ended by HOLD", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_HLDA->base, pl);
    sigtrace_plot(trace_MEMR_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_wr(MEMW_, STATUS_MWRITE);
    sigplot_init(pl, ss, "i8228_bist_mwrite",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical MWRITE cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_WR_->base, pl);
    sigtrace_plot(trace_MEMW_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd(MEMR_, STATUS_SREAD);
    sigplot_init(pl, ss, "i8228_bist_sread",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical MREAD cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_MEMR_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd_hlda(MEMR_, STATUS_SREAD);
    sigplot_init(pl, ss, "i8228_bist_sread_with_hold",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a MREAD ended by HOLD", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_HLDA->base, pl);
    sigtrace_plot(trace_MEMR_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_wr(MEMW_, STATUS_SWRITE);
    sigplot_init(pl, ss, "i8228_bist_swrite",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical MWRITE cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_WR_->base, pl);
    sigtrace_plot(trace_MEMW_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd(IOR_, STATUS_INPUTRD);
    sigplot_init(pl, ss, "i8228_bist_inputrd",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical INPUTRD cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_IOR_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd_hlda(IOR_, STATUS_INPUTRD);
    sigplot_init(pl, ss, "i8228_bist_inputrd_with_hold",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical INPUTRD cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_HLDA->base, pl);
    sigtrace_plot(trace_IOR_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_wr(IOW_, STATUS_OUTPUTWR);
    sigplot_init(pl, ss, "i8228_bist_outputwr",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical OUTPUTWR cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_WR_->base, pl);
    sigtrace_plot(trace_IOW_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd(INTA_, STATUS_INTACK);
    sigplot_init(pl, ss, "i8228_bist_intack",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical INTACK cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_INTA_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd_hlda(INTA_, STATUS_INTACK);
    sigplot_init(pl, ss, "i8228_bist_intack_with_hold",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during an INTACK ended by HOLD", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_HLDA->base, pl);
    sigtrace_plot(trace_INTA_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_nil(STATUS_HALTACK);
    sigplot_init(pl, ss, "i8228_bist_haltack",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical HALTACK cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_INTA_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd(INTA_, STATUS_INTACKW);
    sigplot_init(pl, ss, "i8228_bist_intackw",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during a typical INTACKW cycle", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_INTA_->base, pl);
    sigplot_fini(pl);

    tau_min = TAU;
    check_case_rd_hlda(INTA_, STATUS_INTACKW);
    sigplot_init(pl, ss, "i8228_bist_intackw_with_hold",
                 "Intel 8228 System Controller and Bus Driver 8080A CPU",
                 "Behavior during an INTACKW ended by HOLD", tau_min, TAU - tau_min);
    sigtrace_plot(trace_DATA->base, pl);
    sigtrace_plot(trace_STSTB_->base, pl);
    sigtrace_plot(trace_DBIN->base, pl);
    sigtrace_plot(trace_HLDA->base, pl);
    sigtrace_plot(trace_INTA_->base, pl);
    sigplot_fini(pl);

    sigtrace_fini(trace_DATA->base);
    sigtrace_fini(trace_STSTB_->base);
    sigtrace_fini(trace_DBIN->base);
    sigtrace_fini(trace_WR_->base);
    sigtrace_fini(trace_HLDA->base);
    sigtrace_fini(trace_MEMR_->base);
    sigtrace_fini(trace_MEMW_->base);
    sigtrace_fini(trace_IOR_->base);
    sigtrace_fini(trace_IOW_->base);
    sigtrace_fini(trace_INTA_->base);
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
    DATA_INIT(DATA);

    EDGE_INIT(STSTB_, 0);
    EDGE_INIT(DBIN, 0);
    EDGE_INIT(WR_, 1);
    EDGE_INIT(HLDA, 0);

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
    data_to(DATA, status);
    check_outputs(0);

    TAU += 1;
    edge_lo(STSTB_);
    check_outputs(0);

    TAU += 1;
    edge_hi(STSTB_);
    check_outputs(0);

    TAU += 1;
    data_z(DATA);
    check_outputs(0);

    TAU += 5;
}

static void check_case_rd(Edge e, Byte status)
{
    TAU += 1;
    data_to(DATA, status);
    check_outputs(0);

    TAU += 1;
    edge_lo(STSTB_);
    check_outputs(0);

    TAU += 1;
    edge_hi(STSTB_);
    check_outputs(0);

    TAU += 1;
    data_z(DATA);
    check_outputs(0);

    TAU += 1;
    edge_hi(DBIN);
    check_outputs(e);

    TAU += 1;
    edge_lo(DBIN);
    check_outputs(0);

    TAU += 1;
}

static void check_case_rd_hlda(Edge e, Byte status)
{
    TAU += 1;
    data_to(DATA, status);
    check_outputs(0);

    TAU += 1;
    edge_lo(STSTB_);
    check_outputs(0);

    TAU += 1;
    edge_hi(STSTB_);
    check_outputs(0);

    TAU += 1;
    data_z(DATA);
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

    TAU += 1;
}

static void check_case_wr(Edge e, Byte status)
{
    TAU += 1;
    data_to(DATA, status);
    check_outputs(0);

    TAU += 1;
    edge_lo(STSTB_);
    check_outputs(0);

    TAU += 1;
    edge_hi(STSTB_);
    check_outputs(0);

    TAU += 1;
    data_z(DATA);
    check_outputs(0);

    TAU += 1;
    edge_lo(WR_);
    check_outputs(e);

    TAU += 1;
    edge_hi(WR_);
    check_outputs(0);

    TAU += 1;
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
