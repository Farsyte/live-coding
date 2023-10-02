#include "i8080_impl.h"

// i8080_incdec: provide INR, DCR, INX, DCX

#define NAME_INR_M1T4(R)	i8080_state_inrT4 ## R
#define DEFN_INR_M1T4(R)	static f8080State NAME_INR_M1T4(R)

DEFN_INR_M1T4(B);
DEFN_INR_M1T4(C);
DEFN_INR_M1T4(D);
DEFN_INR_M1T4(E);
DEFN_INR_M1T4(H);
DEFN_INR_M1T4(L);
DEFN_INR_M1T4(M);
DEFN_INR_M1T4(A);

static p8080State   i8080_state_inrT4[8] = {
    NAME_INR_M1T4(B),
    NAME_INR_M1T4(C),
    NAME_INR_M1T4(D),
    NAME_INR_M1T4(E),
    NAME_INR_M1T4(H),
    NAME_INR_M1T4(L),
    NAME_INR_M1T4(M),
    NAME_INR_M1T4(A),
};

#define NAME_DCR_M1T4(R)	i8080_state_dcrT4 ## R
#define DEFN_DCR_M1T4(R)	static f8080State NAME_DCR_M1T4(R)

DEFN_DCR_M1T4(B);
DEFN_DCR_M1T4(C);
DEFN_DCR_M1T4(D);
DEFN_DCR_M1T4(E);
DEFN_DCR_M1T4(H);
DEFN_DCR_M1T4(L);
DEFN_DCR_M1T4(M);
DEFN_DCR_M1T4(A);

static p8080State   i8080_state_dcrT4[8] = {
    NAME_DCR_M1T4(B),
    NAME_DCR_M1T4(C),
    NAME_DCR_M1T4(D),
    NAME_DCR_M1T4(E),
    NAME_DCR_M1T4(H),
    NAME_DCR_M1T4(L),
    NAME_DCR_M1T4(M),
    NAME_DCR_M1T4(A),
};

#define NAME_ALU_TO_R(R)	i8080_state_inrT5 ## R
#define DEFN_ALU_TO_R(R)	static f8080State NAME_ALU_TO_R(R)

DEFN_ALU_TO_R(B);
DEFN_ALU_TO_R(C);
DEFN_ALU_TO_R(D);
DEFN_ALU_TO_R(E);
DEFN_ALU_TO_R(H);
DEFN_ALU_TO_R(L);
DEFN_ALU_TO_R(A);

#define NAME_RP_IDAL_T4(RP)	i8080_state_rp_idal_T4 ## RP
#define DEFN_RP_IDAL_T4(RP)	static f8080State NAME_RP_IDAL_T4(RP)

DEFN_RP_IDAL_T4(BC);
DEFN_RP_IDAL_T4(DE);
DEFN_RP_IDAL_T4(HL);
DEFN_RP_IDAL_T4(SP);

static p8080State   i8080_state_rp_idal_T4[4] = {
    NAME_RP_IDAL_T4(BC),
    NAME_RP_IDAL_T4(DE),
    NAME_RP_IDAL_T4(HL),
    NAME_RP_IDAL_T4(SP),
};

#define NAME_INX_T5(RP)	i8080_state_inx_t5 ## RP
#define DEFN_INX_T5(RP)	static f8080State NAME_INX_T5(RP)

DEFN_INX_T5(BC);
DEFN_INX_T5(DE);
DEFN_INX_T5(HL);
DEFN_INX_T5(SP);

static p8080State   i8080_state_inx_t5[4] = {
    NAME_INX_T5(BC),
    NAME_INX_T5(DE),
    NAME_INX_T5(HL),
    NAME_INX_T5(SP),
};

#define NAME_DCX_T5(RP)	i8080_state_dcx_t5 ## RP
#define DEFN_DCX_T5(RP)	static f8080State NAME_DCX_T5(RP)

DEFN_DCX_T5(BC);
DEFN_DCX_T5(DE);
DEFN_DCX_T5(HL);
DEFN_DCX_T5(SP);

static p8080State   i8080_state_dcx_t5[4] = {
    NAME_DCX_T5(BC),
    NAME_DCX_T5(DE),
    NAME_DCX_T5(HL),
    NAME_DCX_T5(SP),
};

// INR M and DCR M require two additional machine cycles.

static f8080State   i8080_state_inrM2T1M;
static f8080State   i8080_state_inrM2T2M;
static f8080State   i8080_state_inrM2TWM;
static f8080State   i8080_state_inrM2T3M;

static f8080State   i8080_state_dcrM2T1M;
static f8080State   i8080_state_dcrM2T2M;
static f8080State   i8080_state_dcrM2TWM;
static f8080State   i8080_state_dcrM2T3M;

static f8080State   i8080_state_inrdcrM3T1M;
static f8080State   i8080_state_inrdcrM3T2M;
static f8080State   i8080_state_inrdcrM3T3M;

static unsigned     i8080_incdec_flags(unsigned alu, unsigned ac, unsigned old);

// i8080_incdec_init(i8080): set up the opcode decode entries
// for the many increment and decrement instructions.

void i8080_incdec_init(i8080 cpu)
{
    for (int ddd = 0; ddd < 8; ++ddd) {
        Byte                op = 0x04 | (ddd << 3);
        cpu->m1t4[op] = i8080_state_inrT4[ddd];
    }

    for (int ddd = 0; ddd < 8; ++ddd) {
        Byte                op = 0x05 | (ddd << 3);
        cpu->m1t4[op] = i8080_state_dcrT4[ddd];
    }

    for (int rp = 0; rp < 4; ++rp) {
        p8080State          m1t4 = i8080_state_rp_idal_T4[rp];
        p8080State          m1t5 = i8080_state_inx_t5[rp];
        Byte                op = 0x03 | (rp << 4);
        cpu->m1t4[op] = m1t4;
        cpu->m1t5[op] = m1t5;
    }

    for (int rp = 0; rp < 4; ++rp) {
        p8080State          m1t4 = i8080_state_rp_idal_T4[rp];
        p8080State          m1t5 = i8080_state_dcx_t5[rp];
        Byte                op = 0x0B | (rp << 4);
        cpu->m1t4[op] = m1t4;
        cpu->m1t5[op] = m1t5;
    }
}

// The IMPL_INR_M1T4(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_INR_M1T4(R)                                        \
    static void NAME_INR_M1T4(R) (i8080 cpu, int phase) {       \
        unsigned old;                                           \
        unsigned tmp;                                           \
        unsigned alu;                                           \
        unsigned ac;                                            \
        unsigned flags;                                         \
                                                                \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            break;                                              \
        case PHI2_RISE:                                         \
                                                                \
            old = cpu->FLAGS->value;                            \
            tmp = cpu->R->value;                                \
            alu = tmp + 1;                                      \
            ac = 16 & ((tmp & 15) + 1);                         \
                                                                \
            flags = i8080_incdec_flags(alu, ac, old);           \
                                                                \
            data_to(cpu->TMP, tmp);                             \
            data_to(cpu->ALU, alu);                             \
            data_to(cpu->FLAGS, flags);                         \
            break;                                              \
        case PHI2_FALL:                                         \
            cpu->state_next = NAME_ALU_TO_R(R);                 \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_INR_M1T4(R)

IMPL_INR_M1T4(A);
IMPL_INR_M1T4(B);
IMPL_INR_M1T4(C);
IMPL_INR_M1T4(D);
IMPL_INR_M1T4(E);
IMPL_INR_M1T4(H);
IMPL_INR_M1T4(L);

// The IMPL_DCR_M1T4(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_DCR_M1T4(R)                                        \
    static void NAME_DCR_M1T4(R) (i8080 cpu, int phase) {       \
        unsigned old;                                           \
        unsigned tmp;                                           \
        unsigned alu;                                           \
        unsigned ac;                                            \
        unsigned flags;                                         \
                                                                \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            break;                                              \
        case PHI2_RISE:                                         \
                                                                \
            old = cpu->FLAGS->value;                            \
            tmp = cpu->R->value;                                \
            alu = tmp - 1;                                      \
            ac = 16 & ((tmp & 15) - 1);                         \
                                                                \
            flags = i8080_incdec_flags(alu, ac, old);           \
                                                                \
            data_to(cpu->TMP, tmp);                             \
            data_to(cpu->ALU, alu);                             \
            data_to(cpu->FLAGS, flags);                         \
            break;                                              \
        case PHI2_FALL:                                         \
            cpu->state_next = NAME_ALU_TO_R(R);                 \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_DCR_M1T4(R)

IMPL_DCR_M1T4(A);
IMPL_DCR_M1T4(B);
IMPL_DCR_M1T4(C);
IMPL_DCR_M1T4(D);
IMPL_DCR_M1T4(E);
IMPL_DCR_M1T4(H);
IMPL_DCR_M1T4(L);

// The IMPL_ALU_TO_R(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_ALU_TO_R(R)                                        \
    static void NAME_ALU_TO_R(R) (i8080 cpu, int phase) {       \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            edge_hi(cpu->RETM1_INT);                            \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->R, cpu->ALU->value);                   \
            break;                                              \
        case PHI2_FALL:                                         \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_ALU_TO_R(R)

IMPL_ALU_TO_R(A);
IMPL_ALU_TO_R(B);
IMPL_ALU_TO_R(C);
IMPL_ALU_TO_R(D);
IMPL_ALU_TO_R(E);
IMPL_ALU_TO_R(H);
IMPL_ALU_TO_R(L);

// i8080_state_rp_idal_T4RP: latch a register pair into IDAL

#define IMPL_RP_IDAL_T4(RH,RL)                                          \
    static void NAME_RP_IDAL_T4(RH##RL) (i8080 cpu, int phase) {        \
        Byte RH;                                                        \
        Byte RL;                                                        \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            RH = cpu->RH->value;                                        \
            RL = cpu->RL->value;                                        \
            addr_to(cpu->IDAL, (((Word)RH)<<8) | RL);                   \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = cpu->m1t5[cpu->IR->value];                \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_RP_IDAL_T4(RH##RL)

IMPL_RP_IDAL_T4(B, C);
IMPL_RP_IDAL_T4(D, E);
IMPL_RP_IDAL_T4(H, L);

// i8080_state_rp_idal_T4SP: latch SP into IDAL

static void i8080_state_rp_idal_T4SP(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          addr_to(cpu->IDAL, cpu->SP->value);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->m1t5[cpu->IR->value];
          break;
    }
}

// i8080_state_inx_t5RP: increment IDAL into RH,RL

#define IMPL_INX_T5(RH,RL)                                          \
    static void NAME_INX_T5(RH##RL) (i8080 cpu, int phase)          \
    {                                                               \
        Word RP;                                                    \
        switch (phase) {                                            \
        case PHI1_RISE:                                             \
            edge_hi(cpu->RETM1_INT);                                \
            break;                                                  \
        case PHI2_RISE:                                             \
            RP = cpu->IDAL->value + 1;                              \
            data_to(cpu->RH, RP >> 8);                              \
            data_to(cpu->RL, RP);                                   \
            break;                                                  \
        case PHI2_FALL:                                             \
            break;                                                  \
        }                                                           \
    }                                                               \
    DEFN_INX_T5(RH##RL)

IMPL_INX_T5(B, C);
IMPL_INX_T5(D, E);
IMPL_INX_T5(H, L);

// i8080_state_inx_t5SP: increment IDAL into SP

static void i8080_state_inx_t5SP(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          addr_to(cpu->SP, cpu->IDAL->value + 1);
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_dcx_t5RP: decrement IDAL into RP

#define IMPL_DCX_T5(RH,RL)                                          \
    static void NAME_DCX_T5(RH##RL) (i8080 cpu, int phase)          \
    {                                                               \
        Word RP;                                                    \
        switch (phase) {                                            \
        case PHI1_RISE:                                             \
            edge_hi(cpu->RETM1_INT);                                \
            break;                                                  \
        case PHI2_RISE:                                             \
            RP = cpu->IDAL->value - 1;                              \
            data_to(cpu->RH, RP >> 8);                              \
            data_to(cpu->RL, RP);                                   \
            break;                                                  \
        case PHI2_FALL:                                             \
            break;                                                  \
        }                                                           \
    }                                                               \
    DEFN_DCX_T5(RH##RL)

IMPL_DCX_T5(B, C);
IMPL_DCX_T5(D, E);
IMPL_DCX_T5(H, L);

// i8080_state_dcx_t5SP: decrement IDAL into SP

static void i8080_state_dcx_t5SP(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          addr_to(cpu->SP, cpu->IDAL->value - 1);
          break;
      case PHI2_FALL:
          break;
    }
}

// TODO write function comment for i8080_state_inrM1T4M

static void i8080_state_inrT4M(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_inrM2T1M;
          break;
    }
}

// TODO write function comment for i8080_state_inrM2T1M

static void i8080_state_inrM2T1M(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_inrM2T2M;
          break;
    }
}

// TODO write function comment for i8080_state_inrM2T2M

static void i8080_state_inrM2T2M(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_inrM2T3M;
          else
              cpu->state_next = i8080_state_inrM2TWM;
          break;
    }
}

// TODO write function comment for i8080_state_inrM2T2M

static void i8080_state_inrM2TWM(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_inrM2T3M;
          else
              cpu->state_next = i8080_state_inrM2TWM;
          break;
    }
}

// TODO write function comment for i8080_state_inrM2T3M

static void i8080_state_inrM2T3M(i8080 cpu, int phase)
{
    unsigned            old;
    unsigned            tmp;
    unsigned            alu;
    unsigned            ac;
    unsigned            flags;

    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:

          old = cpu->FLAGS->value;
          tmp = cpu->DATA->value;
          alu = tmp + 1;
          ac = 16 & ((tmp & 15) + 1);

          flags = i8080_incdec_flags(alu, ac, old);

          data_to(cpu->TMP, tmp);
          data_to(cpu->ALU, alu);
          data_to(cpu->FLAGS, flags);

          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_inrdcrM3T1M;
          break;
    }
}

// TODO write function comment for i8080_state_dcrM1T4M

static void i8080_state_dcrT4M(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_dcrM2T1M;
          break;
    }
}

// TODO write function comment for i8080_state_dcrM2T1M

static void i8080_state_dcrM2T1M(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_dcrM2T2M;
          break;
    }
}

// TODO write function comment for i8080_state_dcrM2T2M

static void i8080_state_dcrM2T2M(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_dcrM2T3M;
          else
              cpu->state_next = i8080_state_dcrM2TWM;
          break;
    }
}

// TODO write function comment for i8080_state_dcrM2T2M

static void i8080_state_dcrM2TWM(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_dcrM2T3M;
          else
              cpu->state_next = i8080_state_dcrM2TWM;
          break;
    }
}

// TODO write function comment for i8080_state_dcrM2T3M

static void i8080_state_dcrM2T3M(i8080 cpu, int phase)
{
    unsigned            old;
    unsigned            tmp;
    unsigned            alu;
    unsigned            ac;
    unsigned            flags;

    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:

          old = cpu->FLAGS->value;
          tmp = cpu->DATA->value;
          alu = tmp - 1;
          ac = 16 & ((tmp & 15) - 1);

          flags = i8080_incdec_flags(alu, ac, old);

          data_to(cpu->TMP, tmp);
          data_to(cpu->ALU, alu);
          data_to(cpu->FLAGS, flags);

          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_inrdcrM3T1M;
          break;
    }
}

// TODO write function comment for i8080_state_inrdcrM3T1M

static void i8080_state_inrdcrM3T1M(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_inrdcrM3T2M;
          break;
    }
}

// TODO write function comment for i8080_state_inrdcrM3T2M

static void i8080_state_inrdcrM3T2M(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          edge_lo(cpu->SYNC);
          data_to(cpu->DATA, cpu->ALU->value);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_inrdcrM3T3M;
          break;
    }
}

// TODO write function comment for i8080_state_inrdcrM3T3M

static void i8080_state_inrdcrM3T3M(i8080 cpu, int phase)
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

// i8080_alu_flags: update the flags based on the extended
// result of the ALU operation, with the aux carry provided
// separately.

static unsigned i8080_incdec_flags(unsigned alu, unsigned ac, unsigned old)
{

    unsigned            cy = old & FLAGS_CY;

    // FLAGS_P is set if there are an even number of bits
    // set in the low 8 bits of the result.

    unsigned            p = alu;
    p ^= p >> 4;
    p ^= p >> 2;
    p ^= p >> 1;
    p = (p & 1) ? 0 : FLAGS_P;

    unsigned            s = alu & 0x80;

    unsigned            z = alu ? 0 : FLAGS_Z;

    // turn on the "always on" 0x02 bit.

    return cy | 0x02 | p | ac | z | s;

}
