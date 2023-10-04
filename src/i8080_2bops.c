#include "i8080_impl.h"

// i8080_2bops: provide support for fetching 2nd byte of multibyte instructions
// T1 drives PC to IDAL to address, STATUS_MREAD to data, and SYNC high.
// T2 drives IDAL+1 to PC, releases Data and Sync, drives DBIN
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

// i8080_state_2bops_t1: publish PC via IDAL, STATUS. Start SYNC.

static void i8080_state_2bops_t1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, VAL(PC));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_MREAD);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_2bops_t2;
          break;
    }
}

// i8080_state_2bops_t2: inc IDAL into PC, end SYNC, start DBIN, check READY

static void i8080_state_2bops_t2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(PC, INC(IDAL));
          LOWER(SYNC);
          DTRI(DATA);
          RAISE(DBIN);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = cpu->m2t3[VAL(IR)];     // no bist coverage
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
          RAISE(WAIT);
          break;
      case PHI2_RISE:
          // do not issue a falling edge,
          // but do re-issue the rising edge.
          VAL(DBIN) = 0;
          RAISE(DBIN);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = cpu->m2t3[VAL(IR)];
          break;
    }
}
