#include "i8080_impl.h"

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
          edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:
          data_to(cpu->W, 0x00);
          data_to(cpu->Z, cpu->DATA->value);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
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
          addr_to(cpu->IDAL, (cpu->W->value << 8) | cpu->Z->value);
          addr_to(cpu->ADDR, cpu->IDAL->value);
          data_to(cpu->DATA, STATUS_INPUTRD);
          edge_hi(cpu->SYNC);
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
          data_z(cpu->DATA);
          edge_lo(cpu->SYNC);
          edge_hi(cpu->DBIN);
          break;
      case PHI2_FALL:
          if (cpu->READY->value)
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
          edge_lo(cpu->WAIT);
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->A, cpu->DATA->value);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
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
          edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:
          data_to(cpu->W, 0x00);
          data_to(cpu->Z, cpu->DATA->value);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
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
          addr_to(cpu->IDAL, (cpu->W->value << 8) | cpu->Z->value);
          addr_to(cpu->ADDR, cpu->IDAL->value);
          data_to(cpu->DATA, STATUS_OUTPUTWR);
          edge_hi(cpu->SYNC);
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
          data_to(cpu->DATA, cpu->A->value);
          edge_lo(cpu->SYNC);
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
          edge_hi(cpu->RETM1_INT);
          edge_lo(cpu->WR_);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          edge_hi(cpu->WR_);
          addr_z(cpu->ADDR);
          break;
    }
}
