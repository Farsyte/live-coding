#include "chip/i8080_impl.h"

// i8080_io: provide IN and OUT instructions

static f8080State   i8080_io_in_M2T3;
static f8080State   i8080_io_in_M3T1;
static f8080State   i8080_io_in_M3T2;
static f8080State   i8080_io_in_M3TW;
static f8080State   i8080_io_in_M3T3;

static f8080State   i8080_io_out_M2T3;
static f8080State   i8080_io_out_M3T1;
static f8080State   i8080_io_out_M3T2;
static f8080State   i8080_io_out_M3T3;

// i8080_io_init: set opcode decode entries for IN and OUT

void i8080_io_init(i8080 cpu)
{
    cpu->m1t4[OP_IN] = cpu->state_2bops;
    cpu->m2t3[OP_IN] = i8080_io_in_M2T3;

    cpu->m1t4[OP_OUT] = cpu->state_2bops;
    cpu->m2t3[OP_OUT] = i8080_io_out_M2T3;
}

// i8080_io_in_M2T3: accept port number for IN instruction

static void i8080_io_in_M2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(W, 0x00);
          DSET(Z, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_io_in_M3T1;
          break;
    }
}

// i8080_io_in_M3T1: initiate machine cycle for IN execution

static void i8080_io_in_M3T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, RP(W, Z));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_INPUTRD);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_io_in_M3T2;
          break;
    }
}

// i8080_io_in_M3T2: start data transfer for IN instruction

static void i8080_io_in_M3T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DTRI(DATA);
          RAISE(DBIN);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_io_in_M3T3;
          else
              cpu->state_next = i8080_io_in_M3TW;
          break;
    }
}

// i8080_io_in_M3TW: handle WAIT states for IN instruction

static void i8080_io_in_M3TW(i8080 cpu, int phase)
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
              cpu->state_next = i8080_io_in_M3T3;
          else
              cpu->state_next = i8080_io_in_M3TW;
          break;
    }
}

// i8080_io_in_M3T3: finish data transfer for IN instruction

static void i8080_io_in_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          DSET(A, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_io_out_M2T3: accept port number for OUT instruction

static void i8080_io_out_M2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(W, 0x00);
          DSET(Z, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_io_out_M3T1;
          break;
    }
}

// i8080_io_out_M3T1: initiate machine cycle for OUT execution

static void i8080_io_out_M3T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, RP(W, Z));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_OUTPUTWR);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_io_out_M3T2;
          break;
    }
}

// i8080_io_out_M3T2: start data transfer for OUT instruction

static void i8080_io_out_M3T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DSET(DATA, VAL(A));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_io_out_M3T3;
          break;
    }
}

// i8080_io_out_M3T3: finish data transfer for OUT instruction

static void i8080_io_out_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          LOWER(WR_);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          RAISE(WR_);
          ATRI(ADDR);
          break;
    }
}
