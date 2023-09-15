#include "clock.h"
#include "i8224.h"
#include "support.h"

static void         i8224_on_osc_rise(i8224 gen);
static void         i8224_on_reset_rise(i8224 gen);
static void         i8224_on_reset_fall(i8224 gen);

// i8224_invar: verify the invariants for a i8224.
// - input pointer is not NULL
// - it has a name that is not empty
// - state is 0 during init, and 1-9 when ready to run
// - when state is not zero, all pEdge fields point at valid Edge structs

void i8224_invar(i8224 gen)
{
    assert(gen);

    int                 state = gen->state;

    if (state != 0) {
        edge_invar(gen->OSC);
        edge_invar(gen->SYNC);
        edge_invar(gen->RESIN_);
        edge_invar(gen->RDYIN);
    }

    edge_invar(gen->PHI1);
    edge_invar(gen->PHI2);
    edge_invar(gen->STSTB_);
    edge_invar(gen->RESET);
    edge_invar(gen->READY);

    assert(0 <= state);
    assert(state <= 9);

    (void)gen;
}

// i8224_init(s): initialize the given i8224 to have this name
// and an initial state.

void i8224_init(i8224 gen, Cstr name)
{
    assert(gen);
    assert(name);
    assert(name[0]);

    gen->name = name;

    pEdge               PHI1 = gen->PHI1;
    pEdge               PHI2 = gen->PHI2;
    pEdge               STSTB_ = gen->STSTB_;
    pEdge               RESET = gen->RESET;
    pEdge               READY = gen->READY;

    edge_init(PHI1, format("%s:Φ₁", name));
    edge_lo(PHI1);

    edge_init(PHI2, format("%s:Φ₂", name));
    edge_lo(PHI2);

    edge_init(STSTB_, format("%s:/STSTB", name));
    edge_lo(STSTB_);

    edge_init(RESET, format("%s:RESET", name));
    edge_hi(RESET);

    edge_init(READY, format("%s:READY", name));
    edge_lo(READY);

    gen->state = 0;

    i8224_invar(gen);
}

// i8224_linked(s): finish initialization, called after pEdge fields are set.

void i8224_linked(i8224 gen)
{
    gen->state = 1;
    i8224_invar(gen);

    assert(0 == gen->OSC->value);
    assert(0 == gen->SYNC->value);
    assert(0 == gen->RESIN_->value);
    assert(0 == gen->RDYIN->value);

    EDGE_ON_RISE(gen->OSC, i8224_on_osc_rise, gen);
    EDGE_ON_RISE(gen->RESET, i8224_on_reset_rise, gen);
    EDGE_ON_FALL(gen->RESET, i8224_on_reset_fall, gen);
}

static void i8224_on_osc_rise(i8224 gen)
{
    int                 state = gen->state;
    switch (state++) {

      default:
          FAIL("state (%d) is invalid, must be 1 <= state <= 9", state);
          break;

      case 0:
          FAIL("received OSC RISE before initialization was complete");
          break;

      case 1:
          edge_to(gen->STSTB_, !gen->RESET->value);
          edge_hi(gen->PHI1);
          break;
      case 2:
          break;
      case 3:
          edge_lo(gen->PHI1);
          edge_hi(gen->PHI2);
          break;
      case 4:
          edge_to(gen->RESET, !gen->RESIN_->value);
          edge_to(gen->READY, gen->RDYIN->value);
          break;
      case 5:
          break;
      case 6:
          break;
      case 7:
          break;
      case 8:
          edge_lo(gen->PHI2);
          break;
      case 9:
          if (gen->SYNC->value)
              edge_lo(gen->STSTB_);
          state = 1;
          break;

    }
    gen->state = state;
}

static void i8224_on_reset_rise(i8224 gen)
{
    edge_lo(gen->STSTB_);
}

static void i8224_on_reset_fall(i8224 gen)
{
    edge_hi(gen->STSTB_);
}

// everything below here is POST, BIST, and BENCH.

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
    ASSERT_EQ_integer(1, gen->OSC->rise->len);
    ASSERT_EQ_integer(0, gen->OSC->fall->len);

    int                 state = gen->state;
    clock_run_one();
    for (int i = 0; i < 8; ++i) {
        ASSERT_NE_integer(state, gen->state);
        clock_run_one();
    }
    ASSERT_EQ_integer(state, gen->state);

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

    EDGE_ON_RISE(gen->PHI1, edge_change, gen->PHI1);
    EDGE_ON_FALL(gen->PHI1, edge_change, gen->PHI1);
    EDGE_ON_RISE(gen->PHI2, edge_change, gen->PHI2);
    EDGE_ON_FALL(gen->PHI2, edge_change, gen->PHI2);
    EDGE_ON_RISE(gen->SYNC, edge_change, gen->SYNC);
    EDGE_ON_FALL(gen->SYNC, edge_change, gen->SYNC);
    EDGE_ON_RISE(gen->STSTB_, edge_change, gen->STSTB_);
    EDGE_ON_FALL(gen->STSTB_, edge_change, gen->STSTB_);
    EDGE_ON_RISE(gen->RESET, edge_change, gen->RESET);
    EDGE_ON_FALL(gen->RESET, edge_change, gen->RESET);
    EDGE_ON_RISE(gen->READY, edge_change, gen->READY);
    EDGE_ON_FALL(gen->READY, edge_change, gen->READY);

    assert(0 == gen->RESIN_->value);
    assert(0 == gen->RDYIN->value);
    assert(0 == gen->SYNC->value);

    assert(1 == gen->RESET->value);
    assert(0 == gen->READY->value);
    assert(0 == gen->STSTB_->value);

    Bit                 expected_RESIN_ = 0;
    Bit                 expected_RDYIN = 0;
    Bit                 expected_SYNC = 0;
    Bit                 expected_RESET = 1;
    Bit                 expected_READY = 0;
    Bit                 expected_STSTB_ = 0;

    // set t0 to the value of TAU that will be present
    // when we expect run the first rising edge of PHI1.

    t0 = TAU + 1;
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

    ASSERT_EQ_integer(52, *count_phi1_rise);
    ASSERT_EQ_integer(52, *count_phi1_fall);
    ASSERT_EQ_integer(52, *count_phi2_rise);
    ASSERT_EQ_integer(52, *count_phi2_fall);

    ASSERT_EQ_integer(13, *count_ststb_rise);
    ASSERT_EQ_integer(12, *count_ststb_fall);
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

    gen->OSC = CLOCK;

    EDGE_INIT(SYNC);
    edge_lo(SYNC);
    gen->SYNC = SYNC;

    EDGE_INIT(RESIN_);
    edge_lo(RESIN_);
    gen->RESIN_ = RESIN_;

    EDGE_INIT(RDYIN);
    edge_lo(RDYIN);
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
