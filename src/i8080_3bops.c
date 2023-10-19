#include "chip/i8080_impl.h"

// i8080_3bops: provide support for fetching 3rd byte of multibyte instructions
// T1 drives PC to IDAL to address, STATUS_MREAD to data, and SYNC high.
// T2 drives IDAL+1 to PC, releases Data and Sync, drives DBIN
// TW waits for READY if needed
// T2 and TW deliver control to cpu->m2t4[IR] when READY

static f8080State   i8080_state_3bops_t1;
static f8080State   i8080_state_3bops_t2;
static f8080State   i8080_state_3bops_tw;
// 3bops uses cpu->m3t3[IR]

// Initialize the 3BOPS facility.
//
// Publish to the i8080 the pointer to the T-state handler function to
// use for the M1T4 cycle for instructions that have a second byte in
// the instruction stream. This submodule handles the memory read
// transaction, ending with handing control to the appropriate handler
// for the M2 T3 cycle, when the incoming data is present on the data
// bus and the PC has been incremented.

void i8080_3bops_init(i8080 cpu)
{
    cpu->state_3bops_t1 = i8080_state_3bops_t1;
}

// i8080_state_3bops_t1: publish PC via IDAL, STATUS. Start SYNC.

static void i8080_state_3bops_t1(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_3bops_t2;
          break;
    }
}

// i8080_state_3bops_t2: inc IDAL into PC, end SYNC, start DBIN, check READY

static void i8080_state_3bops_t2(i8080 cpu, int phase)
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
              cpu->state_next = cpu->m3t3[VAL(IR)];     // no bist coverage
          else
              cpu->state_next = i8080_state_3bops_tw;
          break;
    }
}

// i8080_state_3bops_tw: raise WAIT, re-raise DBIN, check READY

static void i8080_state_3bops_tw(i8080 cpu, int phase)
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
              cpu->state_next = cpu->m3t3[VAL(IR)];
          break;
    }
}
