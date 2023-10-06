#include "i8080_flags.h"
#include "i8080_impl.h"

// i8080_ret: transfer of control

#define NAME_R_M1T4(CC)	i8080_state_R ## CC ## _M1T4
#define DEFN_R_M1T4(CC)	static f8080State NAME_R_M1T4(CC)

DEFN_R_M1T4(NZ);
DEFN_R_M1T4(Z);
DEFN_R_M1T4(NC);
DEFN_R_M1T4(C);
DEFN_R_M1T4(PO);
DEFN_R_M1T4(PE);
DEFN_R_M1T4(P);
DEFN_R_M1T4(M);

static f8080State   i8080_state_RET_M1T4;

// M2 pops PC_LO from the stack (if ret taken)

static f8080State   i8080_state_RET_M2T1;
static f8080State   i8080_state_RET_M2T2;
static f8080State   i8080_state_RET_M2TW;
static f8080State   i8080_state_RET_M2T3;

// M3 pops PC_HI from the stack (if ret taken)

static f8080State   i8080_state_RET_M3T1;
static f8080State   i8080_state_RET_M3T2;
static f8080State   i8080_state_RET_M3TW;
static f8080State   i8080_state_RET_M3T3;

// Conditional RET taken uses RET_M1T4 during T5
// Conditional RET not taken uses Rnot_M1T5 which asserts RET_M1

static f8080State   i8080_state_Rnot_M1T5;

// i8080_ret_init: link T-state handlers for RET into the CPU state

void i8080_ret_init(i8080 cpu)
{

    cpu->m1t4[OP_RET] = i8080_state_RET_M1T4;

    cpu->m1t4[OP_RNZ] = i8080_state_RNZ_M1T4;
    cpu->m1t4[OP_RZ] = i8080_state_RZ_M1T4;
    cpu->m1t4[OP_RNC] = i8080_state_RNC_M1T4;
    cpu->m1t4[OP_RC] = i8080_state_RC_M1T4;
    cpu->m1t4[OP_RPO] = i8080_state_RPO_M1T4;
    cpu->m1t4[OP_RPE] = i8080_state_RPE_M1T4;
    cpu->m1t4[OP_RP] = i8080_state_RP_M1T4;
    cpu->m1t4[OP_RM] = i8080_state_RM_M1T4;
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

// i8080_state_RET_M1T4: finish decode for RET
// starts predecrement of SP

static void i8080_state_RET_M1T4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_RET_M2T1;
          break;
    }
}

#define DECL_R_M1T4(CC) NAME_R_M1T4(CC) (i8080 cpu, int phase)

// i8080_state_R<<cc>>_M1T4: finish M1 conditional ret work
// evaluate condition, diverting to the "not" path if the
// ret will not be taken. Otherwise, start predecrement of
// sp and continue with ret logic.

#define IMPL_R_M1T4(CC,FB,FV)                                           \
    static void DECL_R_M1T4(CC) {                                       \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            break;                                                      \
        case PHI2_RISE:                                                 \
            break;                                                      \
        case PHI2_FALL:                                                 \
            if ((VAL(FLAGS) & FB) == FV) {                              \
                cpu->state_next = i8080_state_RET_M1T4;                 \
            } else {                                                    \
                cpu->state_next = i8080_state_Rnot_M1T5;                \
            }                                                           \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_R_M1T4(CC)

IMPL_R_M1T4(NZ, FLAGS_Z, 0);
IMPL_R_M1T4(Z, FLAGS_Z, FLAGS_Z);

IMPL_R_M1T4(NC, FLAGS_CY, 0);
IMPL_R_M1T4(C, FLAGS_CY, FLAGS_CY);

IMPL_R_M1T4(PO, FLAGS_P, 0);
IMPL_R_M1T4(PE, FLAGS_P, FLAGS_P);

IMPL_R_M1T4(P, FLAGS_S, 0);
IMPL_R_M1T4(M, FLAGS_S, FLAGS_S);

// i8080_state_RET_M2T1: ADDR/SYNC state for pop PC_LO

static void i8080_state_RET_M2T1(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_RET_M2T2;
          break;
    }
}

// i8080_state_RET_M2T2: INC SP state for pop PC_LO

static void i8080_state_RET_M2T2(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_RET_M2T3;
          else
              cpu->state_next = i8080_state_RET_M2TW;
          break;
    }
}

// i8080_state_RET_M2TW: WAIT state for pop PC_LO

static void i8080_state_RET_M2TW(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_RET_M2T3;
          else
              cpu->state_next = i8080_state_RET_M2TW;
          break;
    }
}

// i8080_state_RET_M2TW: READ state for pop PC_LO

static void i8080_state_RET_M2T3(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_RET_M3T1;
          break;
    }
}

// i8080_state_RET_M3T1: ADDR/SYNC state for pop PC_HI

static void i8080_state_RET_M3T1(i8080 cpu, int phase)
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
          cpu->state_next = i8080_state_RET_M3T2;
          break;
    }
}

// i8080_state_RET_M3T2: INC SP state for pop PC_HI

static void i8080_state_RET_M3T2(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_RET_M3T3;
          else
              cpu->state_next = i8080_state_RET_M3TW;
          break;
    }
}

// i8080_state_RET_M3TW: WAIT state for pop PC_HI

static void i8080_state_RET_M3TW(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_RET_M3T3;
          else
              cpu->state_next = i8080_state_RET_M3TW;
          break;
    }
}

// i8080_state_RET_M3TW: READ state for pop PC_HI

static void i8080_state_RET_M3T3(i8080 cpu, int phase)
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
          ASET(PC, RP(W, Z));
          DTRI(W);
          DTRI(Z);
          break;
    }
}

// === === === === === === === === === === === === === === === ===
// Support for shortened instruction cycle for "not taken"
// === === === === === === === === === === === === === === === ===

// i8080_state_Rnot_M1T5: finish up M1 for ret-not-taken

static void i8080_state_Rnot_M1T5(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          break;
    }
}
