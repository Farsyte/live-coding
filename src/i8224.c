#include "chip/i8224.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "common/clock.h"
#include "common/sigtrace.h"
#include "simext/support.h"

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

    edge_init(PHI1, format("%s:\\Phi_1", name), 0);
    edge_init(PHI2, format("%s:\\Phi_2", name), 0);
    edge_init(STSTB_, format("%s:/STSTB", name), 0);
    edge_init(RESET, format("%s:RESET", name), 1);
    edge_init(READY, format("%s:READY", name), 0);

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

          // NO BIST COVERAGE for any state numbers other than 1..9

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

// NO BIST COVERAGE for rising RESET

static void i8224_on_reset_rise(i8224 gen)
{
    edge_lo(gen->STSTB_);
}

static void i8224_on_reset_fall(i8224 gen)
{
    edge_hi(gen->STSTB_);
}
