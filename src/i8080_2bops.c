#include "i8080_impl.h"

// i8080_b2: provide support for fetching 2nd byte of multibyte instructions
// T1 drives PC to address, STATUS to data, and SYNC high.
// T2 increments PC, releases Data and Sync, drives DBIN
// TW waits for READY if needed
// T2 and TW deliver control to cpu->m2t3[IR] when READY

static f8080State   i8080_state_2bops;          // M1 T4 handler for ops with 2 bytes
static f8080State   i8080_state_2bops_t1;
static f8080State   i8080_state_2bops_t2;
static f8080State   i8080_state_2bops_tw;

// Initialize the 2BOPS facility.
//
// Publish to the i8080 the pointer to the T-state handler function to
// use for the M1T4 cycle for instructions that have a second byte in
// the instruction stream. This submodule handles the memory read
// transaction, ending with handing control to the appropriate handler
// for the M2 T3 cycle, when the incoming data is present on the data
// bus and the PC has been incremented.

void i8080_2bops_init(i8080 cpu)
{
    cpu->state_2bops = i8080_state_2bops;
    cpu->state_2bops_t1 = i8080_state_2bops_t1;
}

// i8080_state_2bops: M1 T4 used for 2-byte opcodes

static void i8080_state_2bops(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_2bops_t1;
          break;
    }
}

// i8080_state_2bops_t1: publish PC, STATUS. Start SYNC.

static void i8080_state_2bops_t1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          addr_to(cpu->ADDR, cpu->PC->value);
          data_to(cpu->DATA, STATUS_MREAD);
          edge_hi(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_2bops_t2;
          break;
    }
}

// i8080_state_2bops_t2: inc PC, end SYNC, start DBIN, check READY

static void i8080_state_2bops_t2(i8080 cpu, int phase)
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
              cpu->state_next = cpu->m2t3[cpu->IR->value];      // no bist coverage
          else
              cpu->state_next = i8080_state_2bops_tw;
          break;
    }
}

// i8080_state_2bops_tw: raise WAIT, re-raise DBIN, check READY

static void i8080_state_2bops_tw(i8080 cpu, int phase)
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
              cpu->state_next = cpu->m2t3[cpu->IR->value];
          break;
    }
}
