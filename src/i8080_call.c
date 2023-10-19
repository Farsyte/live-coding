#include "chip/i8080_flags.h"
#include "chip/i8080_impl.h"

// i8080_call: transfer of control

#define NAME_C_M1T4(CC)	i8080_state_C ## CC ## _M1T4
#define DEFN_C_M1T4(CC)	static f8080State NAME_C_M1T4(CC)

DEFN_C_M1T4(NZ);
DEFN_C_M1T4(Z);
DEFN_C_M1T4(NC);
DEFN_C_M1T4(C);
DEFN_C_M1T4(PO);
DEFN_C_M1T4(PE);
DEFN_C_M1T4(P);
DEFN_C_M1T4(M);

static f8080State   i8080_state_RST_M1T4;
static f8080State   i8080_state_RST_M1T5;

static f8080State   i8080_state_CALL_M1T4;
static f8080State   i8080_state_CALL_M1T5;

static f8080State   i8080_state_CALL_M2T3;
static f8080State   i8080_state_CALL_M3T3;

// M4 pushes PC_HI onto the stack (if call taken)

static f8080State   i8080_state_CALL_M4T1;
static f8080State   i8080_state_CALL_M4T2;
static f8080State   i8080_state_CALL_M4T3;

// M5 pushes PC_LO onto the stack (if call taken)

static f8080State   i8080_state_CALL_M5T1;
static f8080State   i8080_state_CALL_M5T2;
static f8080State   i8080_state_CALL_M5T3;

// M4 and M5 skipped if call not taken

static f8080State   i8080_state_Cnot_M1T5;
static f8080State   i8080_state_Cnot_M2T3;
static f8080State   i8080_state_Cnot_M3T3;

// i8080_call_init: link T-state handlers for CALL into the CPU state

void i8080_call_init(i8080 cpu)
{

    cpu->m1t4[OP_RST__0] = i8080_state_RST_M1T4;
    cpu->m1t4[OP_RST__1] = i8080_state_RST_M1T4;
    cpu->m1t4[OP_RST__2] = i8080_state_RST_M1T4;
    cpu->m1t4[OP_RST__3] = i8080_state_RST_M1T4;
    cpu->m1t4[OP_RST__4] = i8080_state_RST_M1T4;
    cpu->m1t4[OP_RST__5] = i8080_state_RST_M1T4;
    cpu->m1t4[OP_RST__6] = i8080_state_RST_M1T4;
    cpu->m1t4[OP_RST__7] = i8080_state_RST_M1T4;

    cpu->m1t4[OP_CALL] = i8080_state_CALL_M1T4;
    cpu->m2t3[OP_CALL] = i8080_state_CALL_M2T3;
    cpu->m3t3[OP_CALL] = i8080_state_CALL_M3T3;

    cpu->m1t4[OP_CNZ] = i8080_state_CNZ_M1T4;
    cpu->m1t4[OP_CZ] = i8080_state_CZ_M1T4;
    cpu->m1t4[OP_CNC] = i8080_state_CNC_M1T4;
    cpu->m1t4[OP_CC] = i8080_state_CC_M1T4;
    cpu->m1t4[OP_CPO] = i8080_state_CPO_M1T4;
    cpu->m1t4[OP_CPE] = i8080_state_CPE_M1T4;
    cpu->m1t4[OP_CP] = i8080_state_CP_M1T4;
    cpu->m1t4[OP_CM] = i8080_state_CM_M1T4;

    cpu->m2t3[OP_CNZ] = i8080_state_Cnot_M2T3;
    cpu->m2t3[OP_CZ] = i8080_state_Cnot_M2T3;
    cpu->m2t3[OP_CNC] = i8080_state_Cnot_M2T3;
    cpu->m2t3[OP_CC] = i8080_state_Cnot_M2T3;
    cpu->m2t3[OP_CPO] = i8080_state_Cnot_M2T3;
    cpu->m2t3[OP_CPE] = i8080_state_Cnot_M2T3;
    cpu->m2t3[OP_CP] = i8080_state_Cnot_M2T3;
    cpu->m2t3[OP_CM] = i8080_state_Cnot_M2T3;

    cpu->m3t3[OP_CNZ] = i8080_state_Cnot_M3T3;
    cpu->m3t3[OP_CZ] = i8080_state_Cnot_M3T3;
    cpu->m3t3[OP_CNC] = i8080_state_Cnot_M3T3;
    cpu->m3t3[OP_CC] = i8080_state_Cnot_M3T3;
    cpu->m3t3[OP_CPO] = i8080_state_Cnot_M3T3;
    cpu->m3t3[OP_CPE] = i8080_state_Cnot_M3T3;
    cpu->m3t3[OP_CP] = i8080_state_Cnot_M3T3;
    cpu->m3t3[OP_CM] = i8080_state_Cnot_M3T3;
}

    // CC       flags condition
    // NZ       Z == 0
    // Z        Z == 1
    // NC       CY == 0
    // C        CY == 1
    // PO       P == 0
    // PE       P == 1
    // P        S == 0
    // M        S == 1

// i8080_state_RST_M1T4: finish decode for RST
// starts predecrement of SP

static void i8080_state_RST_M1T4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, VAL(SP));

          // The real 8080 hardware clears W in M1T3.
          DSET(W, 0);
          // The real 8080 hardware sets Z in the final T-state
          // of the RST instruction.
          DSET(Z, VAL(IR) & 0x38);

          cpu->state_next = i8080_state_RST_M1T5;

          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_RST_M1T5: finish decode for RST
// finishes predecrement of SP

static void i8080_state_RST_M1T5(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(SP, DEC(IDAL));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_CALL_M4T1;
          break;
    }
}

// i8080_state_CALL_M1T4: finish decode for CALL
// starts predecrement of SP

static void i8080_state_CALL_M1T4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, VAL(SP));
          cpu->state_next = i8080_state_CALL_M1T5;
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_CALL_M1T5: finish decode for CALL
// finishes predecrement of SP

static void i8080_state_CALL_M1T5(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(SP, DEC(IDAL));
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_2bops_t1;
          break;
    }
}

#define DECL_C_M1T4(CC) NAME_C_M1T4(CC) (i8080 cpu, int phase)

// i8080_state_C<<cc>>_M1T4: finish M1 conditional call work
// evaluate condition, diverting to the "not" path if the
// call will not be taken. Otherwise, start predecrement of
// sp and continue with call logic.

#define IMPL_C_M1T4(CC,FB,FV)                                           \
    static void DECL_C_M1T4(CC) {                                       \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            if ((VAL(FLAGS) & FB) == FV) {                              \
                cpu->m2t3[OP_C ## CC] = i8080_state_CALL_M2T3;          \
                cpu->m3t3[OP_C ## CC] = i8080_state_CALL_M3T3;          \
                ASET(IDAL, VAL(SP));                                    \
                cpu->state_next = i8080_state_CALL_M1T5;                \
            } else {                                                    \
                cpu->m2t3[OP_C ## CC] = i8080_state_Cnot_M2T3;          \
                cpu->m3t3[OP_C ## CC] = i8080_state_Cnot_M3T3;          \
                cpu->state_next = i8080_state_Cnot_M1T5;                \
            }                                                           \
            break;                                                      \
        case PHI2_FALL:                                                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_C_M1T4(CC)

IMPL_C_M1T4(NZ, FLAGS_Z, 0);
IMPL_C_M1T4(Z, FLAGS_Z, FLAGS_Z);

IMPL_C_M1T4(NC, FLAGS_CY, 0);
IMPL_C_M1T4(C, FLAGS_CY, FLAGS_CY);

IMPL_C_M1T4(PO, FLAGS_P, 0);
IMPL_C_M1T4(PE, FLAGS_P, FLAGS_P);

IMPL_C_M1T4(P, FLAGS_S, 0);
IMPL_C_M1T4(M, FLAGS_S, FLAGS_S);

// i8080_state_CALL_M2T3: receive low byte of call target

static void i8080_state_CALL_M2T3(i8080 cpu, int phase)
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
          cpu->state_next = cpu->state_3bops_t1;
          break;
    }
}

// i8080_state_CALL_M3T3: receive high byte of call target

static void i8080_state_CALL_M3T3(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_CALL_M4T1;
          break;
    }
}

// i8080_state_CALL_M4T1: ADDR/SYNC cycle for push PC_HI

static void i8080_state_CALL_M4T1(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_CALL_M4T2;
          break;
    }
}

// i8080_state_CALL_M4T2: DEC SP cycle for push PC_HI

static void i8080_state_CALL_M4T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DSET(DATA, VAL(PC) >> 8);
          ASET(SP, DEC(IDAL));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_CALL_M4T3;
          break;
    }
}

// i8080_state_CALL_M4T3: WR cycle for push PC_HI

static void i8080_state_CALL_M4T3(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_CALL_M5T1;
          break;
    }
}

// i8080_state_CALL_M4T2: ADDR/SYNC cycle for push PC_LO

static void i8080_state_CALL_M5T1(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_CALL_M5T2;
          break;
    }
}

// i8080_state_CALL_M5T2: DEC SP cycle for push PC_LO

static void i8080_state_CALL_M5T2(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DSET(DATA, VAL(PC));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_CALL_M5T3;
          break;
    }
}

// i8080_state_CALL_M5T3: WR cycle for push PC_LO
// NOTE: commits WZ to PC to simplify M1T1.

static void i8080_state_CALL_M5T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          LOWER(WR_);
          break;
      case PHI2_RISE:
          ASET(PC, RP(W, Z));
          DTRI(W);
          DTRI(Z);
          break;
      case PHI2_FALL:
          RAISE(WR_);
          ATRI(ADDR);
          break;
    }
}

// === === === === === === === === === === === === === === === ===
// Support for shortened instruction cycle for "not taken"
// === === === === === === === === === === === === === === === ===

// i8080_state_Cnot_M1T5: finish up M1 for call-not-taken

static void i8080_state_Cnot_M1T5(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_2bops_t1;
          break;
    }
}

// i8080_state_Cnot_M2T3: receive target address low byte

static void i8080_state_Cnot_M2T3(i8080 cpu, int phase)
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
          cpu->state_next = cpu->state_3bops_t1;
          break;
    }
}

// i8080_state_Cnot_M3T3: receive target address high byte
// then discard WZ and continue execution normally.

static void i8080_state_Cnot_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          DSET(W, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          // since the condition was false, do NOT copy WZ into PC.
          DTRI(W);
          DTRI(Z);
          break;
    }
}
