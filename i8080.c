#include "clock.h"
#include "decoder.h"
#include "i8080.h"
#include "i8224.h"
#include "i8228.h"
#include "sigtrace.h"
#include "target.h"

// phase numbers
// NOTE: name is short because these will appear EVERYWHERE.
#define	PHI1_RISE	1
#define	PHI2_RISE	3
#define	PHI2_FALL	7

static void         cpu_phi1_rise(i8080 cpu);
static void         cpu_phi2_rise(i8080 cpu);
static void         cpu_phi2_fall(i8080 cpu);

static f8080State   i8080_state_reset;
static f8080State   i8080_state_pc_out_status;
static f8080State   i8080_state_pc_inc;
static f8080State   i8080_state_op_rd;
static f8080State   i8080_state_x;

// i8080: Single Chip 8-bit Microprocessor
//
// This module simulates the behavior of the 8080 CPU, matching
// the external signals (within simulation quantization) and modeling
// some but not all known internal state.

// i8080_invar: verify the invariants for a i8080.

void i8080_invar(i8080 cpu)
{
    assert(cpu);
    assert(cpu->name);
    assert(cpu->name[0]);

    edge_invar(cpu->PHI1);
    edge_invar(cpu->PHI2);
    edge_invar(cpu->SYNC);
    edge_invar(cpu->DBIN);
    edge_invar(cpu->WR_);
    edge_invar(cpu->READY);
    edge_invar(cpu->WAIT);
    edge_invar(cpu->HOLD);
    edge_invar(cpu->HLDA);
    edge_invar(cpu->INT);
    edge_invar(cpu->INTE);
    edge_invar(cpu->RESET);
}

// i8080_init(s): initialize the given i8080 to have this name
// and an initial state.

void i8080_init(i8080 cpu, Cstr name)
{
    assert(cpu);
    assert(name);
    assert(name[0]);

    pEdge               SYNC = cpu->SYNC;
    pEdge               DBIN = cpu->DBIN;
    pEdge               WR_ = cpu->WR_;

    pEdge               WAIT = cpu->WAIT;
    pEdge               HOLD = cpu->HOLD;
    pEdge               HLDA = cpu->HLDA;

    pEdge               INT = cpu->INT;
    pEdge               INTE = cpu->INTE;

    pEdge               RESET_INT = cpu->RESET_INT;
    pEdge               RETM1_INT = cpu->RETM1_INT;
    pEdge               INH_PC_INC = cpu->INH_PC_INC;

    cpu->name = name;
    *cpu->ADDR = ~0;
    *cpu->DATA = ~0;

    edge_init(SYNC, format("%s:SYNC", name));
    edge_lo(SYNC);

    edge_init(DBIN, format("%s:DBIN", name));
    edge_lo(DBIN);

    edge_init(WR_, format("%s:/WR", name));
    edge_hi(WR_);

    edge_init(WAIT, format("%s:WAIT", name));
    edge_lo(WAIT);

    edge_init(HOLD, format("%s:HOLD", name));
    edge_lo(HOLD);

    edge_init(HLDA, format("%s:HLDA", name));
    edge_lo(HLDA);

    edge_init(INT, format("%s:INT", name));
    edge_lo(INT);

    edge_init(INTE, format("%s:INTE", name));
    edge_lo(INTE);

    edge_init(RESET_INT, format("%s:RESET_INT", name));
    edge_hi(RESET_INT);

    edge_init(RETM1_INT, format("%s:RETM1_INT", name));
    edge_lo(RETM1_INT);

    edge_init(INH_PC_INC, format("%s:INH_PC_INC", name));
    edge_lo(INH_PC_INC);

    cpu->state = i8080_state_reset;
    cpu->state_next = i8080_state_reset;
    cpu->state_m1t1 = i8080_state_pc_out_status;
}

// I8080_INIT(s): initialize the given i8080 to have this name
// and an initial state.

#define I8080_INIT(gen) i8080_init(gen, #gen)

// i8080_linked(s): finish initialization, called after pEdge fields are set.

void i8080_linked(i8080 cpu)
{
    i8080_invar(cpu);

    ASSERT_EQ_integer(0, cpu->READY->value);
    ASSERT_EQ_integer(1, cpu->RESET->value);
    ASSERT_EQ_integer(1, cpu->RESET_INT->value);
    ASSERT_EQ_integer(0, cpu->RETM1_INT->value);
    ASSERT_EQ_integer(0, cpu->INH_PC_INC->value);

    EDGE_ON_RISE(cpu->PHI1, cpu_phi1_rise, cpu);
    EDGE_ON_RISE(cpu->PHI2, cpu_phi2_rise, cpu);
    EDGE_ON_FALL(cpu->PHI2, cpu_phi2_fall, cpu);
}

static void cpu_phi1_rise(i8080 cpu)
{
    if (cpu->RETM1_INT->value) {
        cpu->state_next = cpu->state_m1t1;
        edge_lo(cpu->RETM1_INT);
    }

    cpu->state = cpu->state_next;
    cpu->state(cpu, PHI1_RISE);
}

static void cpu_phi2_rise(i8080 cpu)
{
    cpu->state(cpu, PHI2_RISE);
}

static void cpu_phi2_fall(i8080 cpu)
{
    cpu->state(cpu, PHI2_FALL);
    // if in last T-state of a M-cycle,
    // then set state_next to state_m1t1.
}

static void i8080_state_reset(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          if (0 == cpu->RESET->value)
              edge_lo(cpu->RESET_INT);
          break;
      case PHI2_FALL:
          if (0 == cpu->RESET_INT->value)
              cpu->state_next = i8080_state_pc_out_status;
          break;
    }
}

static void i8080_state_pc_out_status(i8080 cpu, int phase)
{
    STUB("cpu %s phase %d", cpu->name, phase);
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          *cpu->DATA = STATUS_FETCH;
          edge_hi(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_pc_inc;
          break;
    }
}

static void i8080_state_pc_inc(i8080 cpu, int phase)
{
    STUB("cpu %s phase %d", cpu->name, phase);
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          *cpu->DATA = ~0;
          edge_lo(cpu->SYNC);
          edge_hi(cpu->DBIN);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_op_rd;
          break;
    }
}

static void i8080_state_op_rd(i8080 cpu, int phase)
{
    STUB("cpu %s phase %d", cpu->name, phase);
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          edge_lo(cpu->DBIN);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_x;
          break;
    }
}

//    Edge RETM1_INT;
//    Edge INH_PC_INC;

static void i8080_state_x(i8080 cpu, int phase)
{
    STUB("cpu %s phase %d", cpu->name, phase);
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          break;
    }
}

// Test code below this line.

static void         i8080_test_init();
static void         i8080_trace_init();
static void         i8080_trace_fini();

static i8080        cpu;
static i8224        gen;
static i8228        ctl;
static Decoder      dec;

static Edge         RESIN_;                     // owned by environment
static Edge         RDYIN;                      // owned by environment

static const pAddr  ADDR = cpu->ADDR;
static const pData  DATA = cpu->DATA;

static const pEdge  SYNC = cpu->SYNC;
static const pEdge  DBIN = cpu->DBIN;
static const pEdge  WR_ = cpu->WR_;
static const pEdge  WAIT = cpu->WAIT;
static const pEdge  HOLD = cpu->HOLD;
static const pEdge  HLDA = cpu->HLDA;
static const pEdge  INT = cpu->INT;
static const pEdge  INTE = cpu->INTE;
static const pEdge  RESET_INT = cpu->RESET_INT;

static const pEdge  PHI1 = gen->PHI1;
static const pEdge  PHI2 = gen->PHI2;
static const pEdge  STSTB_ = gen->STSTB_;
static const pEdge  RESET = gen->RESET;
static const pEdge  READY = gen->READY;

static const pEdge  MEMR_ = ctl->MEMR_;
static const pEdge  MEMW_ = ctl->MEMW_;
static const pEdge  IOR_ = ctl->IOR_;
static const pEdge  IOW_ = ctl->IOW_;
static const pEdge  INTA_ = ctl->INTA_;

static SigSess      ss;

static SigTrace     trace_SYNC;
static SigTrace     trace_DBIN;
static SigTrace     trace_WR_;
static SigTrace     trace_WAIT;
static SigTrace     trace_HOLD;
static SigTrace     trace_HLDA;
static SigTrace     trace_INT;
static SigTrace     trace_INTE;

static SigTrace     trace_PHI1;
static SigTrace     trace_PHI2;
static SigTrace     trace_STSTB_;
static SigTrace     trace_RESIN_;
static SigTrace     trace_RESET;
static SigTrace     trace_RESET_INT;
static SigTrace     trace_RDYIN;
static SigTrace     trace_READY;

static SigTrace     trace_MEMR_;
static SigTrace     trace_MEMW_;
static SigTrace     trace_IOR_;
static SigTrace     trace_IOW_;
static SigTrace     trace_INTA_;

static SigPlot      sp;

// i8080_post: Power-On Self Test for the i8080 code
//
// This function should be called every time the program starts
// before any code relies on i8080 not being completely broken.

void i8080_post()
{
    i8080_test_init();
}

// i8080_bist: Power-On Self Test for the i8080 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the i8080 code is
// not working correctly.

void i8080_bist()
{
    i8080_test_init();
    i8080_trace_init();

    Tau                 t0 = TAU + 0;

    clock_run_until(t0 + 1 + 9 * 1 + 6);
    edge_hi(RESIN_);
    clock_run_until(t0 + 9 * 100);

    sigplot_init(sp, ss, "i8080_bist_reset",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "Coming out of RESET in BIST context", t0, 146);
    sigplot_sig(sp, trace_PHI1);
    sigplot_sig(sp, trace_PHI2);
    sigplot_sig(sp, trace_RESIN_);
    sigplot_sig(sp, trace_RESET);
    sigplot_sig(sp, trace_RESET_INT);
    sigplot_sig(sp, trace_SYNC);
    sigplot_sig(sp, trace_STSTB_);
    sigplot_sig(sp, trace_DBIN);
    sigplot_sig(sp, trace_MEMR_);
    sigplot_fini(sp);

    i8080_trace_fini();
}

// i8080_bench: performance verification for the i8080 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of i8080.

void i8080_bench()
{
    i8080_test_init();
}

static void i8080_test_init()
{
    clock_init(18000000);
    I8080_INIT(cpu);
    I8224_INIT(gen);
    I8228_INIT(ctl);
    DECODER_INIT(dec);

    EDGE_INIT(RESIN_);
    edge_lo(RESIN_);

    EDGE_INIT(RDYIN);
    edge_lo(RDYIN);

    cpu->PHI1 = PHI1;
    cpu->PHI2 = PHI2;
    cpu->READY = READY;
    cpu->RESET = RESET;

    gen->OSC = CLOCK;
    gen->SYNC = SYNC;
    gen->RESIN_ = RESIN_;
    gen->RDYIN = RDYIN;

    ctl->DATA = DATA;
    ctl->STSTB_ = STSTB_;
    ctl->DBIN = DBIN;
    ctl->WR_ = WR_;
    ctl->HLDA = HLDA;

    dec->ADDR = ADDR;
    dec->MEMR_ = MEMR_;
    dec->MEMW_ = MEMW_;
    dec->IOR_ = IOR_;
    dec->IOW_ = IOW_;

    i8080_linked(cpu);
    i8224_linked(gen);
    i8228_linked(ctl);
    decoder_linked(dec);

    i8080_invar(cpu);
    i8224_invar(gen);
    i8228_invar(ctl);
    decoder_invar(dec);

    (void)WAIT;         // output: "we are in a wait state."
    (void)HOLD;         // input: "please hold for DMA"

    (void)INT;          // input: ... from interrupt controller
    (void)INTE;         // output: "interrutps enabled"

    (void)INTA_;        // output: "fetching interrupt opcode"
}

static void i8080_trace_init()
{

    sigsess_init(ss, "i8080_bist");

    sigtrace_init(trace_SYNC, ss, SYNC);
    sigtrace_init(trace_DBIN, ss, DBIN);
    sigtrace_init(trace_WR_, ss, WR_);
    sigtrace_init(trace_WAIT, ss, WAIT);
    sigtrace_init(trace_HOLD, ss, HOLD);
    sigtrace_init(trace_HLDA, ss, HLDA);
    sigtrace_init(trace_INT, ss, INT);
    sigtrace_init(trace_INTE, ss, INTE);
    sigtrace_init(trace_PHI1, ss, PHI1);
    sigtrace_init(trace_PHI2, ss, PHI2);
    sigtrace_init(trace_STSTB_, ss, STSTB_);
    sigtrace_init(trace_RESIN_, ss, RESIN_);
    sigtrace_init(trace_RESET, ss, RESET);
    sigtrace_init(trace_RESET_INT, ss, RESET_INT);
    sigtrace_init(trace_RDYIN, ss, RDYIN);
    sigtrace_init(trace_READY, ss, READY);
    sigtrace_init(trace_MEMR_, ss, MEMR_);
    sigtrace_init(trace_MEMW_, ss, MEMW_);
    sigtrace_init(trace_IOR_, ss, IOR_);
    sigtrace_init(trace_IOW_, ss, IOW_);
    sigtrace_init(trace_INTA_, ss, INTA_);
}

static void i8080_trace_fini()
{

    sigtrace_fini(trace_SYNC);
    sigtrace_fini(trace_DBIN);
    sigtrace_fini(trace_WR_);
    sigtrace_fini(trace_WAIT);
    sigtrace_fini(trace_HOLD);
    sigtrace_fini(trace_HLDA);
    sigtrace_fini(trace_INT);
    sigtrace_fini(trace_INTE);
    sigtrace_fini(trace_PHI1);
    sigtrace_fini(trace_PHI2);
    sigtrace_fini(trace_STSTB_);
    sigtrace_fini(trace_RESIN_);
    sigtrace_fini(trace_RESET);
    sigtrace_fini(trace_RESET_INT);
    sigtrace_fini(trace_RDYIN);
    sigtrace_fini(trace_READY);
    sigtrace_fini(trace_MEMR_);
    sigtrace_fini(trace_MEMW_);
    sigtrace_fini(trace_IOR_);
    sigtrace_fini(trace_IOW_);
    sigtrace_fini(trace_INTA_);

    sigsess_fini(ss);
}
