#include "i8080_impl.h"

// i8080_mov: manage most MOV instructions

static f8080State   i8080_state_movT4B;
static f8080State   i8080_state_movT4C;
static f8080State   i8080_state_movT4D;
static f8080State   i8080_state_movT4E;
static f8080State   i8080_state_movT4H;
static f8080State   i8080_state_movT4L;
static f8080State   i8080_state_movT4A;

static f8080State   i8080_state_movT5B;
static f8080State   i8080_state_movT5C;
static f8080State   i8080_state_movT5D;
static f8080State   i8080_state_movT5E;
static f8080State   i8080_state_movT5H;
static f8080State   i8080_state_movT5L;
static f8080State   i8080_state_movT5A;

static p8080State   i8080_state_movT4[8] = {
    i8080_state_movT4B,
    i8080_state_movT4C,
    i8080_state_movT4D,
    i8080_state_movT4E,
    i8080_state_movT4H,
    i8080_state_movT4L,
    0,
    i8080_state_movT4A,
};

static p8080State   i8080_state_movT5[8] = {
    i8080_state_movT5B,
    i8080_state_movT5C,
    i8080_state_movT5D,
    i8080_state_movT5E,
    i8080_state_movT5H,
    i8080_state_movT5L,
    0,
    i8080_state_movT5A,
};

// i8080_eidihlt_init: set up decoding for EI DI HLT

void i8080_mov_init(i8080 cpu)
{
    for (int sss = 0; sss < 8; ++sss) {
        p8080State          t4 = i8080_state_movT4[sss];
        if (!t4)
            continue;
        for (int ddd = 0; ddd < 8; ++ddd) {
            p8080State          t5 = i8080_state_movT5[sss];
            if (!t5)
                continue;
            Byte                op = 0x40 | (ddd << 3) | sss;
            cpu->m1t4[op] = i8080_state_movT4[sss];
            cpu->m1t5[op] = i8080_state_movT5[ddd];
        }
    }
}

static void i8080_state_movT4B(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->TMP, cpu->B->value);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t5[cpu->IR->value];
          break;
    }
}

static void i8080_state_movT4C(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->TMP, cpu->C->value);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t5[cpu->IR->value];
          break;
    }
}

static void i8080_state_movT4D(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->TMP, cpu->D->value);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t5[cpu->IR->value];
          break;
    }
}

static void i8080_state_movT4E(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->TMP, cpu->E->value);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t5[cpu->IR->value];
          break;
    }
}

static void i8080_state_movT4H(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->TMP, cpu->H->value);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t5[cpu->IR->value];
          break;
    }
}

static void i8080_state_movT4L(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->TMP, cpu->L->value);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t5[cpu->IR->value];
          break;
    }
}

static void i8080_state_movT4A(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->TMP, cpu->A->value);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t5[cpu->IR->value];
          break;
    }
}

static void i8080_state_movT5B(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->B, cpu->TMP->value);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_movT5C(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->C, cpu->TMP->value);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_movT5D(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->D, cpu->TMP->value);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_movT5E(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->E, cpu->TMP->value);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_movT5H(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->H, cpu->TMP->value);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_movT5L(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->L, cpu->TMP->value);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_movT5A(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->A, cpu->TMP->value);
          break;
      case PHI2_FALL:
          break;
    }
}

void i8080_mov_bist(CpuTestSys ts)
{
    (void)ts;
}
