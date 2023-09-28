#include "i8080_impl.h"

// i8080_misc: manage misc instructions

#define NAME_M1T4(RH,RL)	i8080_state_dadM1T4 ## RH##RL
#define DEFN_M1T4(RH,RL)	static f8080State NAME_M1T4(RH,RL)

DEFN_M1T4(B, C);
DEFN_M1T4(D, E);
DEFN_M1T4(H, L);
DEFN_M1T4(S, P);

static p8080State   i8080_state_dadM1T4[8] = {
    NAME_M1T4(B, C),
    NAME_M1T4(D, E),
    NAME_M1T4(H, L),
    NAME_M1T4(S, P),
};

#define NAME_M2T1(RH,RL)	i8080_state_dadM2T1 ## RH##RL
#define DEFN_M2T1(RH,RL)	static f8080State NAME_M2T1(RH,RL)

DEFN_M2T1(B, C);
DEFN_M2T1(D, E);
DEFN_M2T1(H, L);
DEFN_M2T1(S, P);

#define NAME_M2T2(RH,RL)	i8080_state_dadM2T2 ## RH##RL
#define DEFN_M2T2(RH,RL)	static f8080State NAME_M2T2(RH,RL)

DEFN_M2T2(B, C);
DEFN_M2T2(D, E);
DEFN_M2T2(H, L);
DEFN_M2T2(S, P);

#define NAME_M2T3(RH,RL)	i8080_state_dadM2T3 ## RH##RL
#define DEFN_M2T3(RH,RL)	static f8080State NAME_M2T3(RH,RL)

DEFN_M2T3(B, C);
DEFN_M2T3(D, E);
DEFN_M2T3(H, L);
DEFN_M2T3(S, P);

#define NAME_M3T1(RH,RL)	i8080_state_dadM3T1 ## RH##RL
#define DEFN_M3T1(RH,RL)	static f8080State NAME_M3T1(RH,RL)

DEFN_M3T1(B, C);
DEFN_M3T1(D, E);
DEFN_M3T1(H, L);
DEFN_M3T1(S, P);

#define NAME_M3T2(RH,RL)	i8080_state_dadM3T2 ## RH##RL
#define DEFN_M3T2(RH,RL)	static f8080State NAME_M3T2(RH,RL)

DEFN_M3T2(B, C);
DEFN_M3T2(D, E);
DEFN_M3T2(H, L);
DEFN_M3T2(S, P);

#define NAME_M3T3(RH,RL)	i8080_state_dadM3T3 ## RH##RL
#define DEFN_M3T3(RH,RL)	static f8080State NAME_M3T3(RH,RL)

DEFN_M3T3(B, C);
DEFN_M3T3(D, E);
DEFN_M3T3(H, L);
DEFN_M3T3(S, P);

// i8080_dad_init: set up decoding for dad instructions

void i8080_dad_init(i8080 cpu)
{
    for (int rp = 0; rp < 4; ++rp) {
        Byte                op = 0x09 | (rp << 4);
        cpu->m1t4[op] = i8080_state_dadM1T4[rp];
    }
}

// The IMPL_M1T4(RH,RL) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M1T4(RH,RL)                                        \
    static void NAME_M1T4(RH,RL) (i8080 cpu, int phase) {       \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            break;                                              \
        case PHI2_RISE:                                         \
            break;                                              \
        case PHI2_FALL:                                         \
            cpu->state_next = NAME_M2T1(RH,RL);                 \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M1T4(RH,RL)

IMPL_M1T4(B, C);
IMPL_M1T4(D, E);
IMPL_M1T4(H, L);
IMPL_M1T4(S, P);

// The IMPL_M2T1(RH,RL) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define	DECL_M2T1(RH,RL) NAME_M2T1(RH,RL) (i8080 cpu, int phase)

#define IMPL_M2T1(RH,RL)                                        \
    static void DECL_M2T1(RH,RL) {                              \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->ACT, cpu->RL->value);                  \
            break;                                              \
        case PHI2_FALL:                                         \
            cpu->state_next = NAME_M2T2(RH,RL);                 \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M2T1(RH,RL)

IMPL_M2T1(B, C);
IMPL_M2T1(D, E);
IMPL_M2T1(H, L);

static void i8080_state_dadM2T1SP(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->ACT, cpu->SP->value);
          break;
      case PHI2_FALL:
          cpu->state_next = NAME_M2T2(S, P);
          break;
    }
}

// The IMPL_M2T2(RH,RL) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define	DECL_M2T2(RH,RL) NAME_M2T2(RH,RL) (i8080 cpu, int phase)

#define IMPL_M2T2(RH,RL)                                                \
    static void DECL_M2T2(RH,RL) {                                      \
        unsigned alu;                                                   \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            data_to(cpu->TMP, cpu->L->value);                           \
            alu = cpu->ACT->value + cpu->TMP->value;                    \
            data_to(cpu->ALU, alu);                                     \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_M2T3(RH,RL);                         \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_M2T2(RH,RL)

IMPL_M2T2(B, C);
IMPL_M2T2(D, E);
IMPL_M2T2(H, L);
IMPL_M2T2(S, P);

// The IMPL_M2T3(RH,RL) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define	DECL_M2T3(RH,RL) NAME_M2T3(RH,RL) (i8080 cpu, int phase)

#define IMPL_M2T3(RH,RL)                                                \
    static void DECL_M2T3(RH,RL) {                                      \
        unsigned alu;                                                   \
        unsigned flags;                                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            alu = cpu->ACT->value + cpu->TMP->value;                    \
            flags = cpu->FLAGS->value;                                  \
            if (alu & 0x100)                                            \
                data_to(cpu->FLAGS, flags | FLAGS_CY);                  \
            else                                                        \
                data_to(cpu->FLAGS, flags &~ FLAGS_CY);                 \
            data_to(cpu->L, cpu->ALU->value);                           \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_M3T1(RH,RL);                         \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_M2T3(RH,RL)

IMPL_M2T3(B, C);
IMPL_M2T3(D, E);
IMPL_M2T3(H, L);
IMPL_M2T3(S, P);

// The IMPL_M3T1(RH,RL) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define	DECL_M3T1(RH,RL) NAME_M3T1(RH,RL) (i8080 cpu, int phase)

#define IMPL_M3T1(RH,RL)                                        \
    static void DECL_M3T1(RH,RL) {                              \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->ACT, cpu->RH->value);                  \
            break;                                              \
        case PHI2_FALL:                                         \
            cpu->state_next = NAME_M3T2(RH,RL);                 \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M3T1(RH,RL)

IMPL_M3T1(B, C);
IMPL_M3T1(D, E);
IMPL_M3T1(H, L);

static void i8080_state_dadM3T1SP(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          data_to(cpu->ACT, cpu->SP->value >> 8);
          break;
      case PHI2_FALL:
          cpu->state_next = NAME_M3T2(S, P);
          break;
    }
}

// The IMPL_M3T2(RH,RL) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define	DECL_M3T2(RH,RL) NAME_M3T2(RH,RL) (i8080 cpu, int phase)

#define IMPL_M3T2(RH,RL)                                                \
    static void DECL_M3T2(RH, RL)                                       \
    {                                                                   \
        unsigned            alu;                                        \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            data_to(cpu->TMP, cpu->H->value);                           \
            alu = cpu->ACT->value + cpu->TMP->value +                   \
                (FLAGS_CY & cpu->FLAGS->value);                         \
            data_to(cpu->ALU, alu);                                     \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_M3T3(RH, RL);                        \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_M3T2(RH, RL)

IMPL_M3T2(B, C);
IMPL_M3T2(D, E);
IMPL_M3T2(H, L);
IMPL_M3T2(S, P);

// The IMPL_M3T3(RH,RL) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define	DECL_M3T3(RH,RL) NAME_M3T3(RH,RL) (i8080 cpu, int phase)

#define IMPL_M3T3(RH,RL)                                                \
    static void DECL_M3T3(RH,RL) {                                      \
        unsigned flags;                                                 \
        unsigned alu;                                                   \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            edge_hi(cpu->RETM1_INT);                                    \
            break;                                                      \
        case PHI2_RISE:                                                 \
            flags = cpu->FLAGS->value;                                  \
            alu = cpu->ACT->value + cpu->TMP->value +                   \
                (FLAGS_CY & flags);                                     \
            if (alu & 0x100)                                            \
                data_to(cpu->FLAGS, flags | FLAGS_CY);                  \
            else                                                        \
                data_to(cpu->FLAGS, cpu->FLAGS->value & ~FLAGS_CY);     \
            data_to(cpu->H, cpu->ALU->value);                           \
            break;                                                      \
        case PHI2_FALL:                                                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_M3T3(RH,RL)

IMPL_M3T3(B, C);
IMPL_M3T3(D, E);
IMPL_M3T3(H, L);
IMPL_M3T3(S, P);
