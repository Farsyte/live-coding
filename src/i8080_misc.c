#include "i8080_impl.h"

// i8080_misc: manage misc instructions

static f8080State   i8080_state_sphl_t4;
static f8080State   i8080_state_sphl_t5;
static f8080State   i8080_state_xchg_t4;
static f8080State   i8080_state_pchl_t4;
static f8080State   i8080_state_pchl_t5;

// i8080_misc_init: set up decoding for misc instructions

void i8080_misc_init(i8080 cpu)
{
    cpu->m1t4[OP_SPHL] = i8080_state_sphl_t4;
    cpu->m1t4[OP_XCHG] = i8080_state_xchg_t4;
    cpu->m1t4[OP_PCHL] = i8080_state_pchl_t4;
}

// i8080_state_sphl_t4: load HL into IDAL for SPHL instruction.

static void i8080_state_sphl_t4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          addr_to(cpu->IDAL, (((Word) (cpu->H->value)) << 8) | cpu->L->value);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_sphl_t5;
          break;
    }
}

// i8080_state_sphl_t5: latch IDAL into SP for SPHL instruction.

static void i8080_state_sphl_t5(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          addr_to(cpu->SP, cpu->IDAL->value);
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_xchg_t4: swap values of HL and DE register pairs.

static void i8080_state_xchg_t4(i8080 cpu, int phase)
{
    Byte                t;
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:

          t = cpu->H->value;
          data_to(cpu->H, cpu->D->value);
          data_to(cpu->D, t);

          t = cpu->L->value;
          data_to(cpu->L, cpu->E->value);
          data_to(cpu->E, t);

          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_sphl_t5;
          break;
    }
}

// i8080_state_pchl_t4: load HL into IDAL for PCHL instruction.

static void i8080_state_pchl_t4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          addr_to(cpu->IDAL, (((Word) (cpu->H->value)) << 8) | cpu->L->value);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_pchl_t5;
          break;
    }
}

// i8080_state_pchl_t5: latch IDAL into PC for PCHL instruction.

static void i8080_state_pchl_t5(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          addr_to(cpu->PC, cpu->IDAL->value);
          break;
      case PHI2_FALL:
          break;
    }
}
