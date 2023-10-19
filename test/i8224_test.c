#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "chip/i8224.h"
#include "common/clock.h"
#include "common/sigtrace.h"
#include "common/sigtrace_data.h"
#include "common/sigtrace_edge.h"
#include "common/support.h"

static i8224        gen;

static Edge         SYNC;
static Edge         RESIN_;
static Edge         RDYIN;

static void         i8224_test_init();

static int          Tstate = 0;
static Tau          i8224_test_osc_freq_hz = 0;
static Tau          i8224_test_mcycle_ns = 0;

static void         i8224_post_on_phi1_rise(i8224 gen);
static void         i8224_post_on_phi2_rise(i8224 gen);

static void         edge_change(pEdge e);

static void         test_incr(int *ctr);

// i8224_post: Power-On Self Test for the I8224 code
//
// This function should be called every time the program starts
// before any code relies on I8224 not being completely broken.

void i8224_post()
{
    i8224_test_init();
    ASSERT_EQ_integer(1, gen->OSC->rise->len);
    ASSERT_EQ_integer(0, gen->OSC->fall->len);

    int                 count_phi1_rise[1] = { 0 };
    EDGE_ON_RISE(gen->PHI1, test_incr, count_phi1_rise);
    EDGE_ON_RISE(gen->PHI1, i8224_post_on_phi1_rise, gen);

    int                 count_phi1_fall[1] = { 0 };
    EDGE_ON_FALL(gen->PHI1, test_incr, count_phi1_fall);

    int                 count_phi2_rise[1] = { 0 };
    EDGE_ON_RISE(gen->PHI2, test_incr, count_phi2_rise);
    EDGE_ON_RISE(gen->PHI2, i8224_post_on_phi2_rise, gen);

    int                 count_phi2_fall[1] = { 0 };
    EDGE_ON_FALL(gen->PHI2, test_incr, count_phi2_fall);

    int                 count_ststb_rise[1] = { 0 };
    EDGE_ON_RISE(gen->STSTB_, test_incr, count_ststb_rise);

    int                 count_ststb_fall[1] = { 0 };
    EDGE_ON_FALL(gen->STSTB_, test_incr, count_ststb_fall);

    // - cycling the clock once moves us to a new state.
    // - after nine cycles, we are back at the initial state.

    assert(0 == gen->RESIN_->value);
    assert(0 == gen->RDYIN->value);
    assert(0 == gen->SYNC->value);

    assert(1 == gen->RESET->value);
    assert(0 == gen->READY->value);
    assert(0 == gen->STSTB_->value);

    int                 state = gen->state;
    clock_run_one();
    for (int i = 0; i < 8; ++i) {
        ASSERT_NE_integer(state, gen->state);
        clock_run_one();
    }
    ASSERT_EQ_integer(state, gen->state);

    Bit                 expected_RESIN_ = 0;
    Bit                 expected_RDYIN = 0;
    Bit                 expected_SYNC = 0;
    Bit                 expected_RESET = 1;
    Bit                 expected_READY = 0;
    Bit                 expected_STSTB_ = 0;

    for (int mcycle = 1; mcycle < 53; ++mcycle) {

        switch (mcycle) {
          default:
              expected_SYNC = 0;
              break;

          case 4:
              edge_hi(gen->RESIN_);
              expected_RESIN_ = 1;
              expected_RESET = 0;
              expected_STSTB_ = 1;
              break;

          case 12:
              edge_hi(gen->RDYIN);
              expected_RDYIN = 1;
              expected_READY = 1;
              break;

        }

        expected_SYNC = (mcycle >= 4) && ((mcycle % 4) == 1);
        expected_STSTB_ = (mcycle >= 4) && ((mcycle % 4) != 1);

        clock_run_until(clock_future_tau(i8224_test_mcycle_ns));

        ASSERT_EQ_integer(expected_RESIN_, gen->RESIN_->value);
        ASSERT_EQ_integer(expected_RDYIN, gen->RDYIN->value);
        ASSERT_EQ_integer(expected_SYNC, gen->SYNC->value);
        ASSERT_EQ_integer(expected_RESET, gen->RESET->value);
        ASSERT_EQ_integer(expected_READY, gen->READY->value);
        ASSERT_EQ_integer(expected_STSTB_, gen->STSTB_->value);
    }

    ASSERT_EQ_integer(53, *count_phi1_rise);
    ASSERT_EQ_integer(53, *count_phi1_fall);
    ASSERT_EQ_integer(53, *count_phi2_rise);
    ASSERT_EQ_integer(53, *count_phi2_fall);

    ASSERT_EQ_integer(13, *count_ststb_rise);
    ASSERT_EQ_integer(12, *count_ststb_fall);
}

// i8224_bist: Power-On Self Test for the I8224 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the I8224 code is
// not working correctly.

static Tau          t0;

void i8224_bist()
{

    i8224_test_init();

    pEdge               OSC = gen->OSC;
    pEdge               SYNC = gen->SYNC;
    pEdge               RESIN_ = gen->RESIN_;
    pEdge               RDYIN = gen->RDYIN;

    pEdge               PHI1 = gen->PHI1;
    pEdge               PHI2 = gen->PHI2;
    pEdge               STSTB_ = gen->STSTB_;
    pEdge               RESET = gen->RESET;
    pEdge               READY = gen->READY;

    ASSERT_EQ_integer(1, OSC->rise->len);
    ASSERT_EQ_integer(0, OSC->fall->len);

    int                 state = gen->state;
    clock_run_one();
    for (int i = 0; i < 8; ++i) {
        ASSERT_NE_integer(state, gen->state);
        clock_run_one();
    }
    ASSERT_EQ_integer(state, gen->state);

    int                 count_phi1_rise[1] = { 0 };
    EDGE_ON_RISE(PHI1, test_incr, count_phi1_rise);
    EDGE_ON_RISE(PHI1, i8224_post_on_phi1_rise, gen);

    int                 count_phi1_fall[1] = { 0 };
    EDGE_ON_FALL(PHI1, test_incr, count_phi1_fall);

    int                 count_phi2_rise[1] = { 0 };
    EDGE_ON_RISE(PHI2, test_incr, count_phi2_rise);
    EDGE_ON_RISE(PHI2, i8224_post_on_phi2_rise, gen);

    int                 count_phi2_fall[1] = { 0 };
    EDGE_ON_FALL(PHI2, test_incr, count_phi2_fall);

    int                 count_ststb_rise[1] = { 0 };
    EDGE_ON_RISE(STSTB_, test_incr, count_ststb_rise);

    int                 count_ststb_fall[1] = { 0 };
    EDGE_ON_FALL(STSTB_, test_incr, count_ststb_fall);

    // - cycling the clock once moves us to a new state.
    // - after nine cycles, we are back at the initial state.

    EDGE_ON_RISE(PHI1, edge_change, PHI1);
    EDGE_ON_FALL(PHI1, edge_change, PHI1);
    EDGE_ON_RISE(PHI2, edge_change, PHI2);
    EDGE_ON_FALL(PHI2, edge_change, PHI2);
    EDGE_ON_RISE(SYNC, edge_change, SYNC);
    EDGE_ON_FALL(SYNC, edge_change, SYNC);
    EDGE_ON_RISE(STSTB_, edge_change, STSTB_);
    EDGE_ON_FALL(STSTB_, edge_change, STSTB_);
    EDGE_ON_RISE(RESET, edge_change, RESET);
    EDGE_ON_FALL(RESET, edge_change, RESET);
    EDGE_ON_RISE(READY, edge_change, READY);
    EDGE_ON_FALL(READY, edge_change, READY);

    assert(0 == RESIN_->value);
    assert(0 == RDYIN->value);
    assert(0 == SYNC->value);

    assert(1 == RESET->value);
    assert(0 == READY->value);
    assert(0 == STSTB_->value);

    Bit                 expected_RESIN_ = 0;
    Bit                 expected_RDYIN = 0;
    Bit                 expected_SYNC = 0;
    Bit                 expected_RESET = 1;
    Bit                 expected_READY = 0;
    Bit                 expected_STSTB_ = 0;

    SigSess             ss;

    SigTraceEdge        trace_PHI1;
    SigTraceEdge        trace_PHI2;
    SigTraceEdge        trace_SYNC;
    SigTraceEdge        trace_STSTB;
    SigTraceEdge        trace_RESIN;
    SigTraceEdge        trace_RESET;
    SigTraceEdge        trace_RDYIN;
    SigTraceEdge        trace_READY;

    sigsess_init(ss, "i8224_bist");

    sigtrace_edge_init(trace_PHI1, ss, PHI1);
    sigtrace_edge_init(trace_PHI2, ss, PHI2);
    sigtrace_edge_init(trace_SYNC, ss, SYNC);
    sigtrace_edge_init(trace_STSTB, ss, STSTB_);
    sigtrace_edge_init(trace_RESIN, ss, RESIN_);
    sigtrace_edge_init(trace_RESET, ss, RESET);
    sigtrace_edge_init(trace_RDYIN, ss, RDYIN);
    sigtrace_edge_init(trace_READY, ss, READY);

    // set t0 to the value of TAU that will be present
    // when we expect run the first rising edge of PHI1.

    t0 = TAU + 1;
    for (int mcycle = 1; mcycle < 53; ++mcycle) {

        switch (mcycle) {
          default:
              expected_SYNC = 0;
              break;

          case 4:
              edge_hi(RESIN_);
              expected_RESIN_ = 1;
              expected_RESET = 0;
              expected_STSTB_ = 1;
              break;

          case 12:
              edge_hi(RDYIN);
              expected_RDYIN = 1;
              expected_READY = 1;
              break;

        }

        expected_SYNC = (mcycle >= 4) && ((mcycle % 4) == 1);
        expected_STSTB_ = (mcycle >= 4) && ((mcycle % 4) != 1);

        clock_run_until(clock_future_tau(i8224_test_mcycle_ns));

        ASSERT_EQ_integer(expected_RESIN_, RESIN_->value);
        ASSERT_EQ_integer(expected_RDYIN, RDYIN->value);
        ASSERT_EQ_integer(expected_SYNC, SYNC->value);
        ASSERT_EQ_integer(expected_RESET, RESET->value);
        ASSERT_EQ_integer(expected_READY, READY->value);
        ASSERT_EQ_integer(expected_STSTB_, STSTB_->value);
    }

    ASSERT_EQ_integer(52, *count_phi1_rise);
    ASSERT_EQ_integer(52, *count_phi1_fall);
    ASSERT_EQ_integer(52, *count_phi2_rise);
    ASSERT_EQ_integer(52, *count_phi2_fall);

    ASSERT_EQ_integer(13, *count_ststb_rise);
    ASSERT_EQ_integer(12, *count_ststb_fall);

    sigtrace_fini(trace_PHI1->base);
    sigtrace_fini(trace_PHI2->base);
    sigtrace_fini(trace_SYNC->base);
    sigtrace_fini(trace_STSTB->base);
    sigtrace_fini(trace_RESIN->base);
    sigtrace_fini(trace_RESET->base);
    sigtrace_fini(trace_RDYIN->base);
    sigtrace_fini(trace_READY->base);

    SigPlot             p1;
    sigplot_init(p1, ss, "i8224_bist_reset",
                 "Intel 8224 Clock Generator and Driver for 8080A CPU",
                 "Coming out of RESET in BIST context", t0, 72);
    sigtrace_plot(trace_PHI1->base, p1);
    sigtrace_plot(trace_PHI2->base, p1);
    sigtrace_plot(trace_SYNC->base, p1);
    sigtrace_plot(trace_STSTB->base, p1);
    sigtrace_plot(trace_RESIN->base, p1);
    sigtrace_plot(trace_RESET->base, p1);
    sigplot_fini(p1);

    SigPlot             p2;
    sigplot_init(p2, ss, "i8224_bist_ready",
                 "Intel 8224 Clock Generator and Driver for 8080A CPU",
                 "Initial rise of READY in BIST context", t0 + 72, 72);
    sigtrace_plot(trace_PHI1->base, p2);
    sigtrace_plot(trace_PHI2->base, p2);
    sigtrace_plot(trace_SYNC->base, p2);
    sigtrace_plot(trace_STSTB->base, p2);
    sigtrace_plot(trace_RDYIN->base, p2);
    sigtrace_plot(trace_READY->base, p2);
    sigplot_fini(p2);

    SigPlot             p3;
    sigplot_init(p3, ss, "i8224_bist_steady",
                 "Intel 8224 Clock Generator and Driver for 8080A CPU",
                 "Steady state operation in BIST context", t0 + 288, 72);
    sigtrace_plot(trace_PHI1->base, p3);
    sigtrace_plot(trace_PHI2->base, p3);
    sigtrace_plot(trace_SYNC->base, p3);
    sigtrace_plot(trace_STSTB->base, p3);
    sigplot_fini(p3);

    sigsess_fini(ss);

}

static const int    bench_mcycles = 10000;

static void bench_i8224(void *arg)
{
    (void)arg;

    Tau                 final_tau = clock_future_tau(i8224_test_mcycle_ns * bench_mcycles);

    clock_run_until(clock_future_tau(i8224_test_mcycle_ns));
    edge_hi(gen->RESIN_);
    clock_run_until(clock_future_tau(i8224_test_mcycle_ns));
    clock_run_until(clock_future_tau(i8224_test_mcycle_ns));
    clock_run_until(clock_future_tau(i8224_test_mcycle_ns));
    edge_hi(gen->RDYIN);

    clock_run_until(final_tau);
}

// i8224_bench: performance verification for the I8224 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of I8224.

void i8224_bench()
{
    i8224_test_init();

    double              dt = RTC_ELAPSED(bench_i8224, 0);

    BENCH_TOP("i8224");
    BENCH_VAL(dt / bench_mcycles);
    BENCH_END();
}

// support code for test and bench

static void i8224_test_init()
{
    I8224_INIT(gen);
    i8224_invar(gen);

    i8224_test_osc_freq_hz = 18000000;
    i8224_test_mcycle_ns = 9000000000 / i8224_test_osc_freq_hz;

    clock_init(i8224_test_osc_freq_hz);

    EDGE_INIT(SYNC, 0);
    EDGE_INIT(RESIN_, 0);
    EDGE_INIT(RDYIN, 0);

    gen->OSC = CLOCK;
    gen->SYNC = SYNC;
    gen->RESIN_ = RESIN_;
    gen->RDYIN = RDYIN;

    i8224_linked(gen);
    i8224_invar(gen);
}

static void i8224_post_on_phi1_rise(i8224 gen)
{
    if (gen->RESET->value)
        Tstate = 0;
    else
        Tstate = (3 & Tstate) + 1;
}

static void i8224_post_on_phi2_rise(i8224 gen)
{
    (void)gen;
    switch (Tstate) {
      case 1:
          edge_hi(SYNC);
          break;
      case 2:
          edge_lo(SYNC);
          break;
    }

}

static void edge_change(pEdge e)
{
    printf("%8.3f μs %s %s\n", clock_elapsed_ns(t0, TAU) / 1000.0, e->value ? "rise" : "fall",
           e->name);
}

static void test_incr(int *ctr)
{
    ++*ctr;
}
