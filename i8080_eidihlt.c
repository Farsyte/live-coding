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
          break;
      case PHI2_RISE:
          *cpu->ADDR = *cpu->PC;
          *cpu->DATA = STATUS_HALTACK;
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
          *cpu->DATA = ~0;
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
