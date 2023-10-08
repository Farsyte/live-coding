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

static f8080State   i8080_xthl_M1T4;            // finish decode of XTHL (direct to xthl_M2T1)

// read byte at stack+0 into Z, will go into L later.
static f8080State   i8080_xthl_M2T1;            // SP->IDAL->ADDR, STATUS_SREAD
static f8080State   i8080_xthl_M2T2;            // IDAL+1->SP
static f8080State   i8080_xthl_M2TW;            // ... wait state
static f8080State   i8080_xthl_M2T3;            // DATA->Z

// read byte at stack+1 into W, will go into H later.
static f8080State   i8080_xthl_M3T1;            // SP->IDAL->ADDR, STATUS_SREAD
static f8080State   i8080_xthl_M3T2;            // do not IDAL+1->SP this time
static f8080State   i8080_xthl_M3TW;            // ... wait state
static f8080State   i8080_xthl_M3T3;            // DATA->W

// write H to stack+1
static f8080State   i8080_xthl_M4T1;            // SP->IDAL->ADDR, STATUS_SWRITE
static f8080State   i8080_xthl_M4T2;            // H->DATA, IDAL-1->SP
static f8080State   i8080_xthl_M4T3;            // WR low then high

// write L to stack+0
static f8080State   i8080_xthl_M5T1;            // SP->IDAL->ADDR, STATUS_SWRITE
static f8080State   i8080_xthl_M5T2;            // L->DATA
static f8080State   i8080_xthl_M5T3;            // WR low then high
static f8080State   i8080_xthl_M5T4;            // WZ -> IDAL
static f8080State   i8080_xthl_M5T5;            // IDAL -> HL

// i8080_stack_init: link t-state handlers for stack ops into cpu state

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
    cpu->m1t4[OP_XTHL] = i8080_xthl_M1T4;
}

#define DECL_PUSH_M1T4(RH,RL)	NAME_PUSH_M1T4(RH,RL) (i8080 cpu, int phase)
#define DECL_PUSH_M1T5(RH,RL)	NAME_PUSH_M1T5(RH,RL) (i8080 cpu, int phase)

#define DECL_PUSH_M2T1(RH,RL)	NAME_PUSH_M2T1(RH,RL) (i8080 cpu, int phase)
#define DECL_PUSH_M2T2(RH,RL)	NAME_PUSH_M2T2(RH,RL) (i8080 cpu, int phase)
#define DECL_PUSH_M2T3(RH,RL)	NAME_PUSH_M2T3(RH,RL) (i8080 cpu, int phase)

#define DECL_PUSH_M3T1(RH,RL)	NAME_PUSH_M3T1(RH,RL) (i8080 cpu, int phase)
#define DECL_PUSH_M3T2(RH,RL)	NAME_PUSH_M3T2(RH,RL) (i8080 cpu, int phase)
#define DECL_PUSH_M3T3(RH,RL)	NAME_PUSH_M3T3(RH,RL) (i8080 cpu, int phase)

// i8080_push_M1T4<<RP>>: finish decode for PUSH RP
// load SP into IDAL preparing for predecrement in T5

#define IMPL_PUSH_M1T4(RH,RL)                                           \
    static void DECL_PUSH_M1T4(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            ASET(IDAL, VAL(SP));                                        \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M1T5(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M1T4(RH,RL)

// i8080_push_M1T5<<RP>>: finish decode for PUSH RP
// store decremented IDAL back into SP and route control
// to the stack write machine cycle.

#define IMPL_PUSH_M1T5(RH,RL)                                           \
    static void DECL_PUSH_M1T5(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            ASET(SP, DEC(IDAL));                                        \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M2T1(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M1T5(RH,RL)

// i8080_push_M2T1<<RP>>: start high byte stack write operation
// address comes from SP (do not assume IDAL still set)

#define IMPL_PUSH_M2T1(RH,RL)                                           \
    static void DECL_PUSH_M2T1(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            ASET(IDAL, VAL(SP));                                        \
            ASET(ADDR, VAL(IDAL));                                      \
            DSET(DATA, STATUS_SWRITE);                                  \
            RAISE(SYNC);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M2T2(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M2T1(RH,RL)

// i8080_push_M2T2<<RP>>: continue high byte stack write operation
// write decremented IDAL back to SP for next cycle

#define IMPL_PUSH_M2T2(RH,RL)                                           \
    static void DECL_PUSH_M2T2(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            ASET(SP, DEC(IDAL));                                        \
            LOWER(SYNC);                                                \
            DSET(DATA, VAL(RH));                                        \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M2T3(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M2T2(RH,RL)

// i8080_push_M2T3<<RP>>: finish high byte stack write operation

#define IMPL_PUSH_M2T3(RH,RL)                                           \
    static void DECL_PUSH_M2T3(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            LOWER(WR_);                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            RAISE(WR_);                                                 \
            ATRI(ADDR);                                                 \
            cpu->state_next = NAME_PUSH_M3T1(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M2T3(RH,RL)

// i8080_push_M3T1<<RP>>: start low byte stack write operation
// address comes from SP (do not assume IDAL still set)

#define IMPL_PUSH_M3T1(RH,RL)                                           \
    static void DECL_PUSH_M3T1(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            ASET(IDAL, VAL(SP));                                        \
            ASET(ADDR, VAL(IDAL));                                      \
            DSET(DATA, STATUS_SWRITE);                                  \
            RAISE(SYNC);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M3T2(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M3T1(RH,RL)

// i8080_push_M3T2<<RP>>: continue low byte stack write operation
// do not update SP, this is a pre-decrement store operation.

#define IMPL_PUSH_M3T2(RH,RL)                                           \
    static void DECL_PUSH_M3T2(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            LOWER(SYNC);                                                \
            DSET(DATA, VAL(RL));                                        \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_PUSH_M3T3(RH,RL);                    \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_PUSH_M3T2(RH,RL)

// i8080_push_M3T3<<RP>>: finish low byte stack write operation
// This is the final t-state of the instruction.

#define IMPL_PUSH_M3T3(RH,RL)                                           \
    static void DECL_PUSH_M3T3(RH,RL) {                                 \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            RAISE(RETM1_INT);                                           \
            LOWER(WR_);                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            RAISE(WR_);                                                 \
            ATRI(ADDR);                                                 \
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

// i8080_push_M2T2PSW: modified M2T2 for PSW
// the high byte pushed comes from A.

static void i8080_push_M2T2PSW(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(SP, DEC(IDAL));
          LOWER(SYNC);
          DSET(DATA, VAL(A));
          break;
      case PHI2_FALL:
          cpu->state_next = NAME_PUSH_M2T3(P, SW);
          break;
    }
}

// i8080_push_M3T2PSW: modified M2T2 for PSW
// the low byte pushed comes from FLAGS, assuring
// that the 0x02 bit is set and 0x28 bits are not.

static void i8080_push_M3T2PSW(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DSET(DATA, (VAL(FLAGS) & ~0x28) | 0x02);
          break;
      case PHI2_FALL:
          cpu->state_next = NAME_PUSH_M3T3(P, SW);
          break;
    }
}

#define DECL_POP_M1T4(RH,RL)	NAME_POP_M1T4(RH,RL) (i8080 cpu, int phase)

#define DECL_POP_M2T1(RH,RL)	NAME_POP_M2T1(RH,RL) (i8080 cpu, int phase)
#define DECL_POP_M2T2(RH,RL)	NAME_POP_M2T2(RH,RL) (i8080 cpu, int phase)
#define DECL_POP_M2TW(RH,RL)	NAME_POP_M2TW(RH,RL) (i8080 cpu, int phase)
#define DECL_POP_M2T3(RH,RL)	NAME_POP_M2T3(RH,RL) (i8080 cpu, int phase)

#define DECL_POP_M3T1(RH,RL)	NAME_POP_M3T1(RH,RL) (i8080 cpu, int phase)
#define DECL_POP_M3T2(RH,RL)	NAME_POP_M3T2(RH,RL) (i8080 cpu, int phase)
#define DECL_POP_M3TW(RH,RL)	NAME_POP_M3TW(RH,RL) (i8080 cpu, int phase)
#define DECL_POP_M3T3(RH,RL)	NAME_POP_M3T3(RH,RL) (i8080 cpu, int phase)

// i8080_pop_M1T4<<RP>>: finish decode for POP RP

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

// i8080_pop_M2T1<<RP>>: start low byte stack read operation
// address comes from SP

#define IMPL_POP_M2T1(RH,RL)                                            \
    static void DECL_POP_M2T1(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            ASET(IDAL, VAL(SP));                                        \
            ASET(ADDR, VAL(IDAL));                                      \
            DSET(DATA, STATUS_SREAD);                                   \
            RAISE(SYNC);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_POP_M2T2(RH,RL);                     \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M2T1(RH,RL)

// i8080_pop_M2T2<<RP>>: continue low byte stack read operation
// write incremented IDAL back to SP for post-increment load

#define IMPL_POP_M2T2(RH,RL)                                            \
    static void DECL_POP_M2T2(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            LOWER(SYNC);                                                \
            DTRI(DATA);                                                 \
            RAISE(DBIN);                                                \
            ASET(SP, INC(IDAL));                                        \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (VAL(READY))                                             \
                cpu->state_next = NAME_POP_M2T3(RH,RL);                 \
            else                                                        \
                cpu->state_next = NAME_POP_M2TW(RH,RL);                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M2T2(RH,RL)

// i8080_pop_M2T2<<RP>>: wait state for low byte stack read operation

#define IMPL_POP_M2TW(RH,RL)                                            \
    static void DECL_POP_M2TW(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            RAISE(WAIT);                                                \
            break;                                                      \
        case PHI2_RISE:                                                 \
            /* do not issue a falling edge, */                          \
            /* but do re-issue the rising edge. */                      \
            VAL(DBIN) = 0;                                              \
            RAISE(DBIN);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (VAL(READY))                                             \
                cpu->state_next = NAME_POP_M2T3(RH,RL);                 \
            else                                                        \
                cpu->state_next = NAME_POP_M2TW(RH,RL);                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M2TW(RH,RL)

// i8080_pop_M2T3<<RP>>: finish low byte stack read operation

#define IMPL_POP_M2T3(RH,RL)                                            \
    static void DECL_POP_M2T3(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            LOWER(WAIT);                                                \
            break;                                                      \
        case PHI2_RISE:                                                 \
            DSET(RL, VAL(DATA));                                        \
            LOWER(DBIN);                                                \
            ATRI(ADDR);                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_POP_M3T1(RH,RL);                     \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M2T3(RH,RL)

// i8080_pop_M3T1<<RP>>: start high byte stack read operation
// address comes from SP

#define IMPL_POP_M3T1(RH,RL)                                            \
    static void DECL_POP_M3T1(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            ASET(IDAL, VAL(SP));                                        \
            ASET(ADDR, VAL(IDAL));                                      \
            DSET(DATA, STATUS_SREAD);                                   \
            RAISE(SYNC);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            cpu->state_next = NAME_POP_M3T2(RH,RL);                     \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M3T1(RH,RL)

// i8080_pop_M3T2<<RP>>: continue high byte stack read operation
// write back post-incremented address to SP

#define IMPL_POP_M3T2(RH,RL)                                            \
    static void DECL_POP_M3T2(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            LOWER(SYNC);                                                \
            DTRI(DATA);                                                 \
            RAISE(DBIN);                                                \
            ASET(SP, INC(IDAL));                                        \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (VAL(READY))                                             \
                cpu->state_next = NAME_POP_M3T3(RH,RL);                 \
            else                                                        \
                cpu->state_next = NAME_POP_M3TW(RH,RL);                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M3T2(RH,RL)

// i8080_pop_M3TW<<RP>>: wait state for high byte stack read operation

#define IMPL_POP_M3TW(RH,RL)                                            \
    static void DECL_POP_M3TW(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            RAISE(WAIT);                                                \
            break;                                                      \
        case PHI2_RISE:                                                 \
            /* do not issue a falling edge, */                          \
            /* but do re-issue the rising edge. */                      \
            VAL(DBIN) = 0;                                              \
            RAISE(DBIN);                                                \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if (VAL(READY))                                             \
                cpu->state_next = NAME_POP_M3T3(RH,RL);                 \
            else                                                        \
                cpu->state_next = NAME_POP_M3TW(RH,RL);                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M3TW(RH,RL)

// i8080_push_M3T3<<RP>>: finish high byte stack read operation
// This is the final t-state of the instruction.

#define IMPL_POP_M3T3(RH,RL)                                            \
    static void DECL_POP_M3T3(RH,RL) {                                  \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            RAISE(RETM1_INT);                                           \
            LOWER(WAIT);                                                \
            break;                                                      \
        case PHI2_RISE:                                                 \
            DSET(RH, VAL(DATA));                                        \
            LOWER(DBIN);                                                \
            ATRI(ADDR);                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_POP_M3T3(RH,RL)

#define IMPL_POP(RH,RL)                                                 \
    IMPL_POP_M1T4(RH,RL);                                               \
    IMPL_POP_M2T1(RH,RL);                                               \
    IMPL_POP_M2T2(RH,RL);                                               \
    IMPL_POP_M2TW(RH,RL);                                               \
    IMPL_POP_M2T3(RH,RL);                                               \
    IMPL_POP_M3T1(RH,RL);                                               \
    IMPL_POP_M3T2(RH,RL);                                               \
    IMPL_POP_M3TW(RH,RL);                                               \
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

// i8080_pop_M2T3PSW: specialized M2T3 pop for PSW
// this byte is placed into FLAGS, after assuring
// that the 0x02 bit is set and the 0x28 bits are clear.

static void i8080_pop_M2T3PSW(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(FLAGS, (VAL(DATA) & ~0x28) | 0x02);
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = NAME_POP_M3T1(P, SW);
          break;
    }
}

// i8080_pop_M3T3PSW: specialized M2T3 pop for PSW
// this byte is placed into A.

static void i8080_pop_M3T3PSW(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          LOWER(WAIT);
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

// === === === === === === === === === === === === === === === ===
// XTHL: Exchange HL with word on top of stack
// === === === === === === === === === === === === === === === ===

// finish decode of XTHL (direct to xthl_M2T1)
static void i8080_xthl_M1T4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M2T1;
          break;
    }
}

// read byte at stack+0 into Z, will go into L later.
// SP->IDAL->ADDR, STATUS_SREAD
static void i8080_xthl_M2T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, VAL(SP));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_SREAD);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M2T2;
          break;
    }
}

// IDAL+1->SP
static void i8080_xthl_M2T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DTRI(DATA);
          RAISE(DBIN);
          ASET(SP, INC(IDAL));
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_xthl_M2T3;
          else
              cpu->state_next = i8080_xthl_M2TW;
          break;
    }
}

// ... wait state
static void i8080_xthl_M2TW(i8080 cpu, int phase)
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
              cpu->state_next = i8080_xthl_M2T3;
          else
              cpu->state_next = i8080_xthl_M2TW;
          break;
    }
}

static void i8080_xthl_M2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(Z, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M3T1;
          break;
    }
}

// read byte at stack+1 into W, will go into H later.
// SP->IDAL->ADDR, STATUS_SREAD
static void i8080_xthl_M3T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, VAL(SP));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_SREAD);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M3T2;
          break;
    }
}

// IDAL+1->SP
static void i8080_xthl_M3T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DTRI(DATA);
          RAISE(DBIN);
          // NOT THIS TIME: ASET(SP, INC(IDAL));                                        
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_xthl_M3T3;
          else
              cpu->state_next = i8080_xthl_M3TW;
          break;
    }
}

// ... wait state
static void i8080_xthl_M3TW(i8080 cpu, int phase)
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
              cpu->state_next = i8080_xthl_M3T3;
          else
              cpu->state_next = i8080_xthl_M3TW;
          break;
    }
}

static void i8080_xthl_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          break;
      case PHI2_RISE:
          DSET(W, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M4T1;
          break;
    }
}

// write H to stack+1
// SP->IDAL->ADDR, STATUS_SWRITE
static void i8080_xthl_M4T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, VAL(SP));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_SWRITE);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M4T2;
          break;
    }
}

// H->DATA, IDAL-1->SP
static void i8080_xthl_M4T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(SP, DEC(IDAL));
          LOWER(SYNC);
          DSET(DATA, VAL(H));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M4T3;
          break;
    }
}

// WR low then high
static void i8080_xthl_M4T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WR_);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          RAISE(WR_);
          ATRI(ADDR);
          cpu->state_next = i8080_xthl_M5T1;
          break;
    }
}

// write L to stack+0
// SP->IDAL->ADDR, STATUS_SWRITE
static void i8080_xthl_M5T1(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, VAL(SP));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_SWRITE);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M5T2;
          break;
    }
}

// H->DATA, IDAL-1->SP
static void i8080_xthl_M5T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DSET(DATA, VAL(L));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M5T3;
          break;
    }
}

// WR low then high
static void i8080_xthl_M5T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WR_);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          RAISE(WR_);
          ATRI(ADDR);
          cpu->state_next = i8080_xthl_M5T4;
          break;
    }
}

// WZ -> IDAL
static void i8080_xthl_M5T4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, RP(W, Z));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_xthl_M5T5;
          break;
    }
}

// IDAL -> HL
static void i8080_xthl_M5T5(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          DSET(H, VAL(IDAL) >> 8);
          DSET(L, VAL(IDAL));
          break;
      case PHI2_FALL:
          break;
    }
}
