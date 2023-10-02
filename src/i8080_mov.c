#include "clock.h"
#include "i8080_impl.h"

// i8080_mov: manage most MOV instructions
//
// This implementation uses one individual T-state management function
// for each source register, and one for each destination; each of the
// instructions will mix-and-match the correct source and destination.

// It is indeed possible to use a single function for T4 and a single
// function for T5, but this means additional work within each function
// to select the register, when we already are pulling the function
// pointer from different places for each function.
//
// using 64 distinct M1T4 functions would reduce runtime by whatever it
// cost us to fetch from cpu->m1t5[cpu->IR->value], which may or may not
// be significant enough to warrent the cost of having so many more small
// functions kicking around.

#define NAME_M1T4(R)	i8080_state_movT4 ## R
#define DEFN_M1T4(R)	static f8080State NAME_M1T4(R)

#define NAME_M1T5(R)	i8080_state_movT5 ## R
#define DEFN_M1T5(R)	static f8080State NAME_M1T5(R)

// MOV Rd,M: common code out until M2T3 which moves (HL) to R.
static f8080State   i8080_state_movT4rd;
static f8080State   i8080_state_movM2T1rd;
static f8080State   i8080_state_movM2T2rd;
static f8080State   i8080_state_movM2TWrd;

#define NAME_RD_M2T3(R)	i8080_state_movT3rd ## R
#define DEFN_RD_M2T3(R)	static f8080State NAME_RD_M2T3(R)

// MOV M,Rs: M1T4 copies R to TMP, then common code to move TMP to (HL).

#define NAME_WR_M1T4(R)	i8080_state_movT4wr ## R
#define DEFN_WR_M1T4(R)	static f8080State NAME_WR_M1T4(R)

static f8080State   i8080_state_movM2T1wr;
static f8080State   i8080_state_movM2T2wr;
static f8080State   i8080_state_movM2T3wr;

DEFN_M1T4(B);
DEFN_M1T4(C);
DEFN_M1T4(D);
DEFN_M1T4(E);
DEFN_M1T4(H);
DEFN_M1T4(L);
DEFN_M1T4(A);

DEFN_M1T5(B);
DEFN_M1T5(C);
DEFN_M1T5(D);
DEFN_M1T5(E);
DEFN_M1T5(H);
DEFN_M1T5(L);
DEFN_M1T5(A);

DEFN_WR_M1T4(B);
DEFN_WR_M1T4(C);
DEFN_WR_M1T4(D);
DEFN_WR_M1T4(E);
DEFN_WR_M1T4(H);
DEFN_WR_M1T4(L);
DEFN_WR_M1T4(A);

DEFN_RD_M2T3(B);
DEFN_RD_M2T3(C);
DEFN_RD_M2T3(D);
DEFN_RD_M2T3(E);
DEFN_RD_M2T3(H);
DEFN_RD_M2T3(L);
DEFN_RD_M2T3(A);

static p8080State   i8080_state_movT4[8] = {
    NAME_M1T4(B),
    NAME_M1T4(C),
    NAME_M1T4(D),
    NAME_M1T4(E),
    NAME_M1T4(H),
    NAME_M1T4(L),
    0,
    NAME_M1T4(A),
};

static p8080State   i8080_state_movT5[8] = {
    NAME_M1T5(B),
    NAME_M1T5(C),
    NAME_M1T5(D),
    NAME_M1T5(E),
    NAME_M1T5(H),
    NAME_M1T5(L),
    0,
    NAME_M1T5(A),
};

static p8080State   i8080_state_movT4wr[8] = {
    NAME_WR_M1T4(B),
    NAME_WR_M1T4(C),
    NAME_WR_M1T4(D),
    NAME_WR_M1T4(E),
    NAME_WR_M1T4(H),
    NAME_WR_M1T4(L),
    0,
    NAME_WR_M1T4(A),
};

static p8080State   i8080_state_movT3rd[8] = {
    NAME_RD_M2T3(B),
    NAME_RD_M2T3(C),
    NAME_RD_M2T3(D),
    NAME_RD_M2T3(E),
    NAME_RD_M2T3(H),
    NAME_RD_M2T3(L),
    0,
    NAME_RD_M2T3(A),
};

// i8080_mov_init: set up decoding for MOV instructions
// operating purely on our eight-bit registers.

void i8080_mov_init(i8080 cpu)
{
    // MOV Rd, Rs (where Rd, Rs != M)
    for (int sss = 0; sss < 8; ++sss) {
        p8080State          m1t4 = i8080_state_movT4[sss];
        if (!m1t4)
            continue;
        for (int ddd = 0; ddd < 8; ++ddd) {
            p8080State          m1t5 = i8080_state_movT5[ddd];
            if (!m1t5)
                continue;
            Byte                op = 0x40 | (ddd << 3) | sss;
            cpu->m1t4[op] = m1t4;
            cpu->m1t5[op] = m1t5;
        }
    }

    // MOV Rd,M
    for (int ddd = 0; ddd < 8; ++ddd) {
        Byte                op = 0x40 | (ddd << 3) | 6;
        if (ddd == 6)   // do not overwrite MOV M,M -- this is HLT.
            continue;
        cpu->m1t4[op] = i8080_state_movT4rd;
        cpu->m2t3[op] = i8080_state_movT3rd[ddd];
    }

    // MOV M,Rs
    for (int sss = 0; sss < 8; ++sss) {
        Byte                op = 0x40 | (6 << 3) | sss;
        if (sss == 6)   // do not overwrite MOV M,M -- this is HLT.
            continue;
        cpu->m1t4[op] = i8080_state_movT4wr[sss];
    }

}

// The IMPL_M1T4(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M1T4(R)                                            \
    static void NAME_M1T4(R) (i8080 cpu, int phase) {           \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->TMP, cpu->R->value);                   \
            break;                                              \
        case PHI2_FALL:                                         \
            cpu->state_next = cpu->m1t5[cpu->IR->value];        \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M1T4(R)

IMPL_M1T4(A);
IMPL_M1T4(B);
IMPL_M1T4(C);
IMPL_M1T4(D);
IMPL_M1T4(E);
IMPL_M1T4(H);
IMPL_M1T4(L);

// The IMPL_M1T5(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M1T5(R)                                            \
    static void NAME_M1T5(R) (i8080 cpu, int phase) {           \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            edge_hi(cpu->RETM1_INT);                            \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->R, cpu->TMP->value);                   \
            break;                                              \
        case PHI2_FALL:                                         \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M1T5(R)

IMPL_M1T5(A);
IMPL_M1T5(B);
IMPL_M1T5(C);
IMPL_M1T5(D);
IMPL_M1T5(E);
IMPL_M1T5(H);
IMPL_M1T5(L);

// MOV Rd,M: common code out until M2T3 which moves (HL) to R.

// TODO add function comment

static void i8080_state_movT4rd(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_movM2T1rd;
          break;
    }
}

// TODO add function comment

static void i8080_state_movM2T1rd(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          addr_to(cpu->IDAL, (cpu->H->value << 8) | cpu->L->value);
          addr_to(cpu->ADDR, cpu->IDAL->value);
          data_to(cpu->DATA, STATUS_MREAD);
          edge_hi(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_movM2T2rd;
          break;
    }
}

// TODO add function comment

static void i8080_state_movM2T2rd(i8080 cpu, int phase)
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
              cpu->state_next = cpu->m2t3[cpu->IR->value];
          else
              cpu->state_next = i8080_state_movM2TWrd;
          break;
    }
}

// TODO add function comment

static void i8080_state_movM2TWrd(i8080 cpu, int phase)
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
          else
              cpu->state_next = i8080_state_movM2TWrd;
          break;
    }
}

// The IMPL_M2T3(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define	DECL_RD_M2T3(R)	NAME_RD_M2T3(R)(i8080 cpu, int phase)
#define IMPL_RD_M2T3(R)                        \
    static void DECL_RD_M2T3(R) {              \
        switch (phase) {                       \
        case PHI1_RISE:                        \
            edge_lo(cpu->WAIT);                \
            edge_hi(cpu->RETM1_INT);           \
            break;                             \
        case PHI2_RISE:                        \
            data_to(cpu->R, cpu->DATA->value); \
            edge_lo(cpu->DBIN);                \
            addr_z(cpu->ADDR);                 \
            break;                             \
        case PHI2_FALL:                        \
            break;                             \
        }                                      \
    }                                          \
    DEFN_RD_M2T3(R)

IMPL_RD_M2T3(B);
IMPL_RD_M2T3(C);
IMPL_RD_M2T3(D);
IMPL_RD_M2T3(E);
IMPL_RD_M2T3(H);
IMPL_RD_M2T3(L);
IMPL_RD_M2T3(A);

// MOV M,Rs: M1T4 copies R to TMP, then common code to move TMP to (HL).

// i8080_state_movT4wr ## R

// The IMPL_M2T3(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define	DECL_WR_M1T4(R)	NAME_WR_M1T4(R)(i8080 cpu, int phase)
#define IMPL_WR_M1T4(R)                              \
    static void DECL_WR_M1T4(R) {                    \
        switch (phase) {                             \
        case PHI1_RISE:                              \
            break;                                   \
        case PHI2_RISE:                              \
            data_to(cpu->TMP, cpu->R->value);        \
            break;                                   \
        case PHI2_FALL:                              \
            cpu->state_next = i8080_state_movM2T1wr; \
            break;                                   \
        }                                            \
    }                                                \
    DEFN_WR_M1T4(R)

IMPL_WR_M1T4(B);
IMPL_WR_M1T4(C);
IMPL_WR_M1T4(D);
IMPL_WR_M1T4(E);
IMPL_WR_M1T4(H);
IMPL_WR_M1T4(L);
IMPL_WR_M1T4(A);

// TODO add function comment

static void i8080_state_movM2T1wr(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_movM2T2wr;
          break;
    }
}

// TODO add function comment

static void i8080_state_movM2T2wr(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          edge_lo(cpu->SYNC);
          data_to(cpu->DATA, cpu->TMP->value);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_movM2T3wr;
    }
}

// TODO add function comment

static void i8080_state_movM2T3wr(i8080 cpu, int phase)
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
