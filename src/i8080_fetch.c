#include "i8080_impl.h"

// i8080_fetch: manage the STATUS_FETCH machine cycle
// T1 drives PC to IDAL to address, STATUS_FETCH to data, and SYNC high.
// T2 drives IDAL+1 to PC, releases Data and Sync, drives DBIN
// TW waits for READY if needed
// T3 samples Data to IR, releases DBIN
// control delivered via cpu->m1t4[IR] for the T4 cycle

static f8080State   i8080_state_pc_out_status;
static f8080State   i8080_state_pc_inc;
static f8080State   i8080_state_fetch_wait;
static f8080State   i8080_state_op_rd;
static f8080State   i8080_state_nop;

// Initialize the FETCH facility.
//
// Fill in cpu->state_fetch with the function to use to
// manage the STATE_FETCH cycles, and establish the service
// function for the NOP instruction.
//
// This should be the only symbol exported by this file into the
// global namespace.

void i8080_fetch_init(i8080 cpu)
{
    cpu->state_fetch = i8080_state_pc_out_status;

    cpu->m1t4[OP_NOP] = i8080_state_nop;
}

// i8080_state_pc_out_status: publish PC via IDAL, STATUS. Start SYNC.

static void i8080_state_pc_out_status(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          // data_z(cpu->IR);
          break;
      case PHI2_RISE:
          addr_to(cpu->IDAL, cpu->PC->value);
          addr_to(cpu->ADDR, cpu->IDAL->value);
          data_to(cpu->DATA, STATUS_FETCH);
          edge_hi(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_pc_inc;
          break;
    }
}

// i8080_state_pc_inc: inc IDAL into PC, end SYNC, start DBIN, check READY

static void i8080_state_pc_inc(i8080 cpu, int phase)
{
    Byte                IR;
    p8080State          m1t2;
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          IR = cpu->IR->value;
          m1t2 = cpu->m1t2[IR];
          if (NULL != m1t2)
              m1t2(cpu, phase);
          addr_to(cpu->PC, cpu->IDAL->value + 1);
          data_z(cpu->DATA);
          edge_lo(cpu->SYNC);
          edge_hi(cpu->DBIN);
          break;
      case PHI2_FALL:
          if (cpu->READY->value)
              cpu->state_next = i8080_state_op_rd;      // no bist coverage
          else
              cpu->state_next = i8080_state_fetch_wait;
          break;
    }
}

// i8080_state_fetch_wait: raise WAIT, re-raise DBIN, check READY

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

// i8080_state_nop: raise RETM1_INT and do nothing else.

static void i8080_state_nop(i8080 cpu, int phase)
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
