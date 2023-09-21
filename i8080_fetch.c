#include "i8080_impl.h"

// i8080_fetch: manage the STATUS_FETCH machine cycle
// T1 drives PC to address, STATUS to data, and SYNC high.
// T2 increments PC, releases Data and Sync, drives DBIN
// TW waits for READY if needed
// T3 samples Data to IR, releases DBIN
// control delivered via cpu->m1t4[IR] for the T4 cycle

static f8080State   i8080_state_pc_out_status;
static f8080State   i8080_state_pc_inc;
static f8080State   i8080_state_fetch_wait;
static f8080State   i8080_state_op_rd;
static f8080State   i8080_state_x;

// Initialize the FETCH facility.
//
// Fill in cpu->state_fetch with the function to use to
// manage the STATE_FETCH cycles, and initialize the opcode
// decode table to the "do nothing" service for M1T4.
//
// This should be the only symbol exported by this file into the
// global namespace.

void i8080_fetch_init(i8080 cpu)
{
    cpu->state_fetch = i8080_state_pc_out_status;

    for (unsigned inst = 0x00; inst <= 0xff; inst++)
        cpu->m1t4[inst] = i8080_state_x;
}

// i8080_state_pc_out_status: publish PC, STATUS. Start SYNC.

static void i8080_state_pc_out_status(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          data_z(cpu->IR);
          break;
      case PHI2_RISE:
          addr_to(cpu->ADDR, cpu->PC->value);
          data_to(cpu->DATA, STATUS_FETCH);
          edge_hi(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_pc_inc;
          break;
    }
}

// i8080_state_pc_out_status: end SYNC, start DBIN, check READY

static void i8080_state_pc_inc(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          addr_to(cpu->PC, cpu->PC->value + 1);
          data_z(cpu->DATA);
          edge_lo(cpu->SYNC);
          edge_hi(cpu->DBIN);
          break;
      case PHI2_FALL:
          if (cpu->READY->value)
              cpu->state_next = i8080_state_op_rd;
          else
              cpu->state_next = i8080_state_fetch_wait;
          break;
    }
}

// i8080_state_pc_fetch_wait: raise WAIT, re-raise DBIN, check READY

static void i8080_state_fetch_wait(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->WAIT);
          break;
      case PHI2_RISE:
          // do not issue a falling edge,
          // but do re-issue the rising edge.
          cpu->DBIN->value = 0;
          edge_hi(cpu->DBIN);
          break;
      case PHI2_FALL:
          if (cpu->READY->value)
              cpu->state_next = i8080_state_op_rd;
          break;
    }
}

// i8080_state_op_rd: release WAIT, sample DATA, release DBIN, decode.

static void i8080_state_op_rd(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:
          data_to(cpu->IR, cpu->DATA->value);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t4[cpu->IR->value];
          break;
    }
}

// i8080_state_x: raise RETM1_INT and do nothing else.

static void i8080_state_x(i8080 cpu, int phase)
{
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
