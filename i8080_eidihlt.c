#include "clock.h"
#include "i8080_impl.h"

// i8080_eidihlt: manage ei, di, and hlt instructions
//
static f8080State   i8080_state_ei;
static f8080State   i8080_state_di;
static f8080State   i8080_state_hlt;
static f8080State   i8080_state_hlt_m2t1;
static f8080State   i8080_state_hlt_m2t2;
static f8080State   i8080_state_hlt_m2wh;

// i8080_eidihlt_init: set up decoding for EI DI HLT

void i8080_eidihlt_init(i8080 cpu)
{
    cpu->m1t4[OP_EI] = i8080_state_ei;
    cpu->m1t4[OP_DI] = i8080_state_di;
    cpu->m1t4[OP_HLT] = i8080_state_hlt;
}

// i8080_state_ei: M1-T4 cycle for EI: raise RETM1_INT, raise INTE

static void i8080_state_ei(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          edge_hi(cpu->INTE);
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_di: M1-T4 cycle for EI: raise RETM1_INT, lower INTE

static void i8080_state_di(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          edge_lo(cpu->INTE);
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_hlt: M1-T4 cycle for HLT: transition to the M2 for halt

static void i8080_state_hlt(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          cpu->state_next = i8080_state_hlt_m2t1;
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_hlt_m2t1: start HALTACK: output PC and STATUS; raise SYNC

static void i8080_state_hlt_m2t1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          data_z(cpu->IR);
          break;
      case PHI2_RISE:
          addr_to(cpu->ADDR, cpu->PC->value);
          data_to(cpu->DATA, STATUS_HALTACK);
          edge_hi(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_hlt_m2t2;
          break;
    }
}

// i8080_state_hlt_m2t2: more HALTACK: drop SYNC

static void i8080_state_hlt_m2t2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_z(cpu->DATA);
          edge_lo(cpu->SYNC);
          cpu->state_next = i8080_state_hlt_m2wh;
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_hlt_m2wh: more HALTACK: raise WAIT

static void i8080_state_hlt_m2wh(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->WAIT);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          break;
    }
}

static Byte         i8080_eidihlt_program[] = {
    OP_NOP,
    OP_EI,
    OP_DI,
    OP_EI,
    OP_HLT,
    0xFF,               // make this look like uninitialized memory
};

void i8080_eidihlt_post(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;

    Word                pc0 = ts->cpu->PC->value;

    memcpy(ts->rom[0]->cells + ts->cpu->PC->value,
           i8080_eidihlt_program, sizeof(i8080_eidihlt_program));

    Tau                 t0 = TAU;

    clock_run_until(t0 + 9 * 5 * 7);

    ASSERT_EQ_integer(1, cpu->READY->value);

    ASSERT_EQ_integer(pc0 + 5, cpu->PC->value);
    ASSERT_EQ_integer(0, cpu->SYNC->value);
    ASSERT_EQ_integer(1, cpu->INTE->value);
    ASSERT_EQ_integer(0, cpu->DBIN->value);
    ASSERT_EQ_integer(1, cpu->WAIT->value);
}

void i8080_eidihlt_bist(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;
    pSigSess            ss = ts->ss;
    SigPlot             sp;

    Word                pc0 = ts->cpu->PC->value;

    memcpy(ts->rom[0]->cells + ts->cpu->PC->value,
           i8080_eidihlt_program, sizeof(i8080_eidihlt_program));

    Tau                 t0 = TAU;

    clock_run_until(t0 + 9 * 5 * 7);

    sigplot_init(sp, ss, "i8080_bist_eidihlt", "Intel 8080 Single Chip 8-bit Microprocessor",
                 // "NOP, DI, EI", t0 + 9 * 4, 9 * 12);
                 "NOP EI HLT", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    ASSERT_EQ_integer(1, cpu->READY->value);

    ASSERT_EQ_integer(pc0 + 5, cpu->PC->value);
    ASSERT_EQ_integer(0, cpu->SYNC->value);
    ASSERT_EQ_integer(1, cpu->INTE->value);
    ASSERT_EQ_integer(0, cpu->DBIN->value);
    ASSERT_EQ_integer(1, cpu->WAIT->value);
}
