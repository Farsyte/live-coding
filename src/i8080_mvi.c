#include "clock.h"
#include "i8080_impl.h"

// i8080_mvi: manage most MVI instructions

// This is one way to not write nearly the same function
// over and over.

// NAME_M2T3(R): use token-pasting to construct the name.

#define NAME_M2T3(R)	i8080_state_mviM2T3 ## R
#define DEFN_M2T3(R)	static f8080State NAME_M2T3(R)

static f8080State   i8080_state_mviM3T1M;
static f8080State   i8080_state_mviM3T2M;
static f8080State   i8080_state_mviM3T3M;

DEFN_M2T3(A);
DEFN_M2T3(B);
DEFN_M2T3(C);
DEFN_M2T3(D);
DEFN_M2T3(E);
DEFN_M2T3(H);
DEFN_M2T3(L);
DEFN_M2T3(M);

static p8080State   i8080_state_mviM2T3[8] = {
    NAME_M2T3(B),
    NAME_M2T3(C),
    NAME_M2T3(D),
    NAME_M2T3(E),
    NAME_M2T3(H),
    NAME_M2T3(L),
    NAME_M2T3(M),
    NAME_M2T3(A),
};

// i8080_mvi_init: set up decoding for MVI instructions

void i8080_mvi_init(i8080 cpu)
{
    for (int ddd = 0; ddd < 8; ++ddd) {
        p8080State          m2t3 = i8080_state_mviM2T3[ddd];
        if (!m2t3)
            continue;
        Byte                op = 0x06 | (ddd << 3);
        cpu->m1t4[op] = cpu->state_2bops;
        cpu->m2t3[op] = i8080_state_mviM2T3[ddd];
    }
}

// The IMPL_M2T3(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define DECL_M2T3(R)	NAME_M2T3(R) (i8080 cpu, int phase)
#define IMPL_M2T3(R)                                            \
    static void DECL_M2T3(R) {                                  \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            edge_lo(cpu->WAIT);                                 \
            edge_hi(cpu->RETM1_INT);                            \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->R, cpu->DATA->value);                  \
            edge_lo(cpu->DBIN);                                 \
            addr_z(cpu->ADDR);                                  \
            break;                                              \
        case PHI2_FALL:                                         \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M2T3(R)

IMPL_M2T3(A);
IMPL_M2T3(B);
IMPL_M2T3(C);
IMPL_M2T3(D);
IMPL_M2T3(E);
IMPL_M2T3(H);
IMPL_M2T3(L);

static void i8080_state_mviM2T3M(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          // edge_hi(cpu->RETM1_INT);                            
          break;
      case PHI2_RISE:
          data_to(cpu->TMP, cpu->DATA->value);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_mviM3T1M;
          break;
    }
}

// TODO add function comment

static void i8080_state_mviM3T1M(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          addr_to(cpu->IDAL, (cpu->H->value << 8) | cpu->L->value);
          addr_to(cpu->ADDR, cpu->IDAL->value);
          data_to(cpu->DATA, STATUS_MWRITE);
          edge_hi(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_mviM3T2M;
          break;
    }
}

// TODO add function comment

static void i8080_state_mviM3T2M(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          edge_lo(cpu->SYNC);
          data_to(cpu->DATA, cpu->TMP->value);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_mviM3T3M;
    }
}

// TODO add function comment

static void i8080_state_mviM3T3M(i8080 cpu, int phase)
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
