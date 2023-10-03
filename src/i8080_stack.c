#include "i8080_impl.h"

// i8080_stack: support stack operations.

// PUSH: decrement, write, decrement, write.
// POP: read, increment, read, increment.

#define NAME_PUSH_M1T4(RH,RL)	i8080_push_M1T4##RH##RL
#define	DEFN_PUSH_M1T4(RH,RL)	static f8080State NAME_PUSH_M1T4(RH,RL)

#define NAME_PUSH_M1T5(RH,RL)	i8080_push_M1T5##RH##RL
#define	DEFN_PUSH_M1T5(RH,RL)	static f8080State NAME_PUSH_M1T5(RH,RL)

#define NAME_PUSH_M2T1(RH,RL)	i8080_push_M2T1##RH##RL
#define	DEFN_PUSH_M2T1(RH,RL)	static f8080State NAME_PUSH_M2T1(RH,RL)

#define NAME_PUSH_M2T2(RH,RL)	i8080_push_M2T2##RH##RL
#define	DEFN_PUSH_M2T2(RH,RL)	static f8080State NAME_PUSH_M2T2(RH,RL)

#define NAME_PUSH_M2T3(RH,RL)	i8080_push_M2T3##RH##RL
#define	DEFN_PUSH_M2T3(RH,RL)	static f8080State NAME_PUSH_M2T3(RH,RL)

#define NAME_PUSH_M3T1(RH,RL)	i8080_push_M3T1##RH##RL
#define	DEFN_PUSH_M3T1(RH,RL)	static f8080State NAME_PUSH_M3T1(RH,RL)

#define NAME_PUSH_M3T2(RH,RL)	i8080_push_M3T2##RH##RL
#define	DEFN_PUSH_M3T2(RH,RL)	static f8080State NAME_PUSH_M3T2(RH,RL)

#define NAME_PUSH_M3T3(RH,RL)	i8080_push_M3T3##RH##RL
#define	DEFN_PUSH_M3T3(RH,RL)	static f8080State NAME_PUSH_M3T3(RH,RL)

#define DEFN_PUSH(RH,RL)                                                \
    DEFN_PUSH_M1T4(RH,RL);                                              \
    DEFN_PUSH_M1T5(RH,RL);                                              \
    DEFN_PUSH_M2T1(RH,RL);                                              \
    DEFN_PUSH_M2T2(RH,RL);                                              \
    DEFN_PUSH_M2T3(RH,RL);                                              \
    DEFN_PUSH_M3T1(RH,RL);                                              \
    DEFN_PUSH_M3T2(RH,RL);                                              \
    DEFN_PUSH_M3T3(RH,RL)

DEFN_PUSH(B, C);
DEFN_PUSH(D, E);
DEFN_PUSH(H, L);
DEFN_PUSH(P, SW);

#define NAME_POP_M1T4(RH,RL)	i8080_pop_M1T4##RH##RL
#define	DEFN_POP_M1T4(RH,RL)	static f8080State NAME_POP_M1T4(RH,RL)

#define NAME_POP_M2T1(RH,RL)	i8080_pop_M2T1##RH##RL
#define	DEFN_POP_M2T1(RH,RL)	static f8080State NAME_POP_M2T1(RH,RL)

#define NAME_POP_M2T2(RH,RL)	i8080_pop_M2T2##RH##RL
#define	DEFN_POP_M2T2(RH,RL)	static f8080State NAME_POP_M2T2(RH,RL)

#define NAME_POP_M2TW(RH,RL)	i8080_pop_M2TW##RH##RL
#define	DEFN_POP_M2TW(RH,RL)	static f8080State NAME_POP_M2TW(RH,RL)

#define NAME_POP_M2T3(RH,RL)	i8080_pop_M2T3##RH##RL
#define	DEFN_POP_M2T3(RH,RL)	static f8080State NAME_POP_M2T3(RH,RL)

#define NAME_POP_M3T1(RH,RL)	i8080_pop_M3T1##RH##RL
#define	DEFN_POP_M3T1(RH,RL)	static f8080State NAME_POP_M3T1(RH,RL)

#define NAME_POP_M3T2(RH,RL)	i8080_pop_M3T2##RH##RL
#define	DEFN_POP_M3T2(RH,RL)	static f8080State NAME_POP_M3T2(RH,RL)

#define NAME_POP_M3TW(RH,RL)	i8080_pop_M3TW##RH##RL
#define	DEFN_POP_M3TW(RH,RL)	static f8080State NAME_POP_M3TW(RH,RL)

#define NAME_POP_M3T3(RH,RL)	i8080_pop_M3T3##RH##RL
#define	DEFN_POP_M3T3(RH,RL)	static f8080State NAME_POP_M3T3(RH,RL)

#define DEFN_POP(RH,RL)                                                 \
    DEFN_POP_M1T4(RH,RL);                                               \
    DEFN_POP_M2T1(RH,RL);                                               \
    DEFN_POP_M2T2(RH,RL);                                               \
    DEFN_POP_M2TW(RH,RL);                                               \
    DEFN_POP_M2T3(RH,RL);                                               \
    DEFN_POP_M3T1(RH,RL);                                               \
    DEFN_POP_M3T2(RH,RL);                                               \
    DEFN_POP_M3TW(RH,RL);                                               \
    DEFN_POP_M3T3(RH,RL)

DEFN_POP(B, C);
DEFN_POP(D, E);
DEFN_POP(H, L);
DEFN_POP(P, SW);

static p8080State   i8080_push_m1t4[4] = {
    NAME_PUSH_M1T4(B, C),
    NAME_PUSH_M1T4(D, E),
    NAME_PUSH_M1T4(H, L),
    NAME_PUSH_M1T4(PS, W),
};

static p8080State   i8080_pop_m1t4[4] = {
    NAME_POP_M1T4(B, C),
    NAME_POP_M1T4(D, E),
    NAME_POP_M1T4(H, L),
    NAME_POP_M1T4(PS, W),
};

// TODO write the function comment.

void i8080_stack_init(i8080 cpu)
{
    for (int rp = 0; rp < 4; ++rp) {
        Byte                op = 0xC5 | (rp << 4);
        cpu->m1t4[op] = i8080_push_m1t4[rp];
    }
    for (int rp = 0; rp < 4; ++rp) {
        Byte                op = 0xC1 | (rp << 4);
        cpu->m1t4[op] = i8080_pop_m1t4[rp];
    }
}

#define DECL_PUSH_M1T4(RH,RL)	NAME_PUSH_M1T4(RH,RL) (i8080 cpu, int phase)
#define IMPL_PUSH_M1T4(RH,RL)                                           \
    static void DECL_PUSH_M1T4(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            addr_to(cpu->IDAL, cpu->SP->value);                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M1T5(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M1T4(RH,RL)

#define DECL_PUSH_M1T5(RH,RL)	NAME_PUSH_M1T5(RH,RL) (i8080 cpu, int phase)
#define IMPL_PUSH_M1T5(RH,RL)                                           \
    static void DECL_PUSH_M1T5(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            addr_to(cpu->SP, cpu->IDAL->value - 1);                     \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M2T1(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M1T5(RH,RL)

// m2 is a "write byte to stack" cycle.

#define DECL_PUSH_M2T1(RH,RL)	NAME_PUSH_M2T1(RH,RL) (i8080 cpu, int phase)
#define IMPL_PUSH_M2T1(RH,RL)                                           \
    static void DECL_PUSH_M2T1(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            addr_to(cpu->IDAL, cpu->SP->value);                         \
            addr_to(cpu->ADDR, cpu->IDAL->value);                       \
            data_to(cpu->DATA, STATUS_SWRITE);                          \
            edge_hi(cpu->SYNC);                                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M2T2(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M2T1(RH,RL)

#define DECL_PUSH_M2T2(RH,RL)	NAME_PUSH_M2T2(RH,RL) (i8080 cpu, int phase)
#define IMPL_PUSH_M2T2(RH,RL)                                           \
    static void DECL_PUSH_M2T2(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            addr_to(cpu->SP, cpu->IDAL->value - 1);                     \
            edge_lo(cpu->SYNC);                                         \
            data_to(cpu->DATA, cpu->RH->value);                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M2T3(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M2T2(RH,RL)

#define DECL_PUSH_M2T3(RH,RL)	NAME_PUSH_M2T3(RH,RL) (i8080 cpu, int phase)
#define IMPL_PUSH_M2T3(RH,RL)                                           \
    static void DECL_PUSH_M2T3(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            edge_lo(cpu->WR_);                                          \
            break;                                                      \
        case PHI2_RISE:                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            edge_hi(cpu->WR_);                                          \
            addr_z(cpu->ADDR);                                          \
            cpu->state_next = NAME_PUSH_M3T1(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M2T3(RH,RL)

// m3 is a "write byte to stack" cycle.

#define DECL_PUSH_M3T1(RH,RL)	NAME_PUSH_M3T1(RH,RL) (i8080 cpu, int phase)
#define IMPL_PUSH_M3T1(RH,RL)                                           \
    static void DECL_PUSH_M3T1(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            addr_to(cpu->IDAL, cpu->SP->value);                         \
            addr_to(cpu->ADDR, cpu->IDAL->value);                       \
            data_to(cpu->DATA, STATUS_SWRITE);                          \
            edge_hi(cpu->SYNC);                                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M3T2(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M3T1(RH,RL)

#define DECL_PUSH_M3T2(RH,RL)	NAME_PUSH_M3T2(RH,RL) (i8080 cpu, int phase)
#define IMPL_PUSH_M3T2(RH,RL)                                           \
    static void DECL_PUSH_M3T2(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            edge_lo(cpu->SYNC);                                         \
            data_to(cpu->DATA, cpu->RL->value);                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M3T3(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M3T2(RH,RL)

#define DECL_PUSH_M3T3(RH,RL)	NAME_PUSH_M3T3(RH,RL) (i8080 cpu, int phase)
#define IMPL_PUSH_M3T3(RH,RL)                                           \
    static void DECL_PUSH_M3T3(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            edge_hi(cpu->RETM1_INT);                                    \
            edge_lo(cpu->WR_);                                          \
            break;                                                      \
        case PHI2_RISE:                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            edge_hi(cpu->WR_);                                          \
            addr_z(cpu->ADDR);                                          \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M3T3(RH,RL)

#define IMPL_PUSH(RH,RL)                                                \
    IMPL_PUSH_M1T4(RH,RL);                                              \
    IMPL_PUSH_M1T5(RH,RL);                                              \
    IMPL_PUSH_M2T1(RH,RL);                                              \
    IMPL_PUSH_M2T2(RH,RL);                                              \
    IMPL_PUSH_M2T3(RH,RL);                                              \
    IMPL_PUSH_M3T1(RH,RL);                                              \
    IMPL_PUSH_M3T2(RH,RL);                                              \
    IMPL_PUSH_M3T3(RH,RL)

IMPL_PUSH(B, C);
IMPL_PUSH(D, E);
IMPL_PUSH(H, L);

IMPL_PUSH_M1T4(P, SW);
IMPL_PUSH_M1T5(P, SW);
IMPL_PUSH_M2T1(P, SW);
// IMPL_PUSH_M2T2(P, SW) has to be specialized for PSW
IMPL_PUSH_M2T3(P, SW);
IMPL_PUSH_M3T1(P, SW);
// IMPL_PUSH_M3T2(P, SW) has to be specialized for PSW
IMPL_PUSH_M3T3(P, SW);

// specialized DECL_PUSH_M2T2(RH,RL) for the PSW

static void i8080_push_M2T2PSW(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          addr_to(cpu->SP, cpu->IDAL->value - 1);
          edge_lo(cpu->SYNC);
          data_to(cpu->DATA, cpu->A->value);
          break;
      case PHI2_FALL:
          cpu->state_next = NAME_PUSH_M2T3(P, SW);
          break;
    }
}

// specialized DECL_PUSH_M3T2(RH,RL) for the PSW

static void i8080_push_M3T2PSW(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          edge_lo(cpu->SYNC);
          data_to(cpu->DATA, cpu->FLAGS->value);
          break;
      case PHI2_FALL:
          cpu->state_next = NAME_PUSH_M3T3(P, SW);
          break;
    }
}

#define DECL_POP_M1T4(RH,RL)	NAME_POP_M1T4(RH,RL) (i8080 cpu, int phase)
#define IMPL_POP_M1T4(RH,RL)                                            \
    static void DECL_POP_M1T4(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_POP_M2T1(RH,RL);                     \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M1T4(RH,RL)

// m2 is a "write byte to stack" cycle.

#define DECL_POP_M2T1(RH,RL)	NAME_POP_M2T1(RH,RL) (i8080 cpu, int phase)
#define IMPL_POP_M2T1(RH,RL)                                            \
    static void DECL_POP_M2T1(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            addr_to(cpu->IDAL, cpu->SP->value);                         \
            addr_to(cpu->ADDR, cpu->IDAL->value);                       \
            data_to(cpu->DATA, STATUS_SREAD);                           \
            edge_hi(cpu->SYNC);                                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_POP_M2T2(RH,RL);                     \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M2T1(RH,RL)

#define DECL_POP_M2T2(RH,RL)	NAME_POP_M2T2(RH,RL) (i8080 cpu, int phase)
#define IMPL_POP_M2T2(RH,RL)                                            \
    static void DECL_POP_M2T2(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            data_z(cpu->DATA);                                          \
            edge_lo(cpu->SYNC);                                         \
            edge_hi(cpu->DBIN);                                         \
            addr_to(cpu->SP, cpu->IDAL->value + 1);                     \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (cpu->READY->value)                                      \
                cpu->state_next = NAME_POP_M2T3(RH,RL);                 \
            else                                                        \
                cpu->state_next = NAME_POP_M2TW(RH,RL);                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M2T2(RH,RL)

#define DECL_POP_M2TW(RH,RL)	NAME_POP_M2TW(RH,RL) (i8080 cpu, int phase)
#define IMPL_POP_M2TW(RH,RL)                                            \
    static void DECL_POP_M2TW(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            edge_hi(cpu->WAIT);                                         \
            break;                                                      \
        case PHI2_RISE:                                                 \
            /* do not issue a falling edge, */                          \
            /* but do re-issue the rising edge. */                      \
            cpu->DBIN->value = 0;                                       \
            edge_hi(cpu->DBIN);                                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (cpu->READY->value)                                      \
                cpu->state_next = NAME_POP_M2T3(RH,RL);                 \
            else                                                        \
                cpu->state_next = NAME_POP_M2TW(RH,RL);                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M2TW(RH,RL)

#define DECL_POP_M2T3(RH,RL)	NAME_POP_M2T3(RH,RL) (i8080 cpu, int phase)
#define IMPL_POP_M2T3(RH,RL)                                            \
    static void DECL_POP_M2T3(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            edge_lo(cpu->WAIT);                                         \
            break;                                                      \
        case PHI2_RISE:                                                 \
            data_to(cpu->RL, cpu->DATA->value);                         \
            edge_lo(cpu->DBIN);                                         \
            addr_z(cpu->ADDR);                                          \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_POP_M3T1(RH,RL);                     \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M2T3(RH,RL)

// m3 is a "write byte to stack" cycle.

#define DECL_POP_M3T1(RH,RL)	NAME_POP_M3T1(RH,RL) (i8080 cpu, int phase)
#define IMPL_POP_M3T1(RH,RL)                                            \
    static void DECL_POP_M3T1(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            addr_to(cpu->IDAL, cpu->SP->value);                         \
            addr_to(cpu->ADDR, cpu->IDAL->value);                       \
            data_to(cpu->DATA, STATUS_SREAD);                           \
            edge_hi(cpu->SYNC);                                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_POP_M3T2(RH,RL);                     \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M3T1(RH,RL)

#define DECL_POP_M3T2(RH,RL)	NAME_POP_M3T2(RH,RL) (i8080 cpu, int phase)
#define IMPL_POP_M3T2(RH,RL)                                            \
    static void DECL_POP_M3T2(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            data_z(cpu->DATA);                                          \
            edge_lo(cpu->SYNC);                                         \
            edge_hi(cpu->DBIN);                                         \
            addr_to(cpu->SP, cpu->IDAL->value + 1);                     \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (cpu->READY->value)                                      \
                cpu->state_next = NAME_POP_M3T3(RH,RL);                 \
            else                                                        \
                cpu->state_next = NAME_POP_M3TW(RH,RL);                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M3T2(RH,RL)

#define DECL_POP_M3TW(RH,RL)	NAME_POP_M3TW(RH,RL) (i8080 cpu, int phase)
#define IMPL_POP_M3TW(RH,RL)                                            \
    static void DECL_POP_M3TW(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            edge_hi(cpu->WAIT);                                         \
            break;                                                      \
        case PHI2_RISE:                                                 \
            /* do not issue a falling edge, */                          \
            /* but do re-issue the rising edge. */                      \
            cpu->DBIN->value = 0;                                       \
            edge_hi(cpu->DBIN);                                         \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (cpu->READY->value)                                      \
                cpu->state_next = NAME_POP_M3T3(RH,RL);                 \
            else                                                        \
                cpu->state_next = NAME_POP_M3TW(RH,RL);                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M3TW(RH,RL)

#define DECL_POP_M3T3(RH,RL)	NAME_POP_M3T3(RH,RL) (i8080 cpu, int phase)
#define IMPL_POP_M3T3(RH,RL)                                            \
    static void DECL_POP_M3T3(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            edge_hi(cpu->RETM1_INT);                                    \
            edge_lo(cpu->WAIT);                                         \
            break;                                                      \
        case PHI2_RISE:                                                 \
            data_to(cpu->RH, cpu->DATA->value);                         \
            edge_lo(cpu->DBIN);                                         \
            addr_z(cpu->ADDR);                                          \
            break;                                                      \
        case PHI2_FALL:                                                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M3T3(RH,RL)

#define IMPL_POP(RH,RL)                                                \
    IMPL_POP_M1T4(RH,RL);                                              \
    IMPL_POP_M2T1(RH,RL);                                              \
    IMPL_POP_M2T2(RH,RL);                                              \
    IMPL_POP_M2TW(RH,RL);                                              \
    IMPL_POP_M2T3(RH,RL);                                              \
    IMPL_POP_M3T1(RH,RL);                                              \
    IMPL_POP_M3T2(RH,RL);                                              \
    IMPL_POP_M3TW(RH,RL);                                              \
    IMPL_POP_M3T3(RH,RL)

IMPL_POP(B, C);
IMPL_POP(D, E);
IMPL_POP(H, L);

IMPL_POP_M1T4(P, SW);
IMPL_POP_M2T1(P, SW);
IMPL_POP_M2T2(P, SW);
IMPL_POP_M2TW(P, SW);
// IMPL_POP_M2T3(P, SW); // has to be specialized for PSW
IMPL_POP_M3T1(P, SW);
IMPL_POP_M3T2(P, SW);
IMPL_POP_M3TW(P, SW);
// IMPL_POP_M3T3(P, SW); // has to be specialized for PSW

// Specialize IMPL_POP_M2T3 for PSW
static void i8080_pop_M2T3PSW(i8080 cpu, int phase)
{
    Byte                F;
    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:
          F = cpu->DATA->value;
          F |= 0x02;
          F &= ~0x28;
          data_to(cpu->FLAGS, F);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = NAME_POP_M3T1(P, SW);
          break;
    }
}

// Specialize DECL_POP_M3T3 for PSW

static void i8080_pop_M3T3PSW(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          edge_lo(cpu->WAIT);
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
