#include "i8080_impl.h"

// i8080_jmp: transfer of control

#define NAME_J_M1T4(CC)	i8080_state_J ## CC ## _M1T4
#define DEFN_J_M1T4(CC)	static f8080State NAME_J_M1T4(CC)

DEFN_J_M1T4(NZ);
DEFN_J_M1T4(Z);
DEFN_J_M1T4(NC);
DEFN_J_M1T4(C);
DEFN_J_M1T4(PO);
DEFN_J_M1T4(PE);
DEFN_J_M1T4(P);
DEFN_J_M1T4(M);

static f8080State   i8080_state_j_M1T5;

static f8080State   i8080_state_JMP_M2T3;
static f8080State   i8080_state_JMP_M3T3;
static f8080State   i8080_state_Jnot_M2T3;
static f8080State   i8080_state_Jnot_M3T3;

void i8080_jmp_init(i8080 cpu)
{

    cpu->m1t4[OP_JMP] = cpu->state_2bops;

    cpu->m1t4[OP_JNZ] = i8080_state_JNZ_M1T4;
    cpu->m1t4[OP_JZ] = i8080_state_JZ_M1T4;
    cpu->m1t4[OP_JNC] = i8080_state_JNC_M1T4;
    cpu->m1t4[OP_JC] = i8080_state_JC_M1T4;
    cpu->m1t4[OP_JPO] = i8080_state_JPO_M1T4;
    cpu->m1t4[OP_JPE] = i8080_state_JPE_M1T4;
    cpu->m1t4[OP_JP] = i8080_state_JP_M1T4;
    cpu->m1t4[OP_JM] = i8080_state_JM_M1T4;

    cpu->m2t3[OP_JMP] = i8080_state_JMP_M2T3;
    cpu->m2t3[OP_JNZ] = i8080_state_Jnot_M2T3;
    cpu->m2t3[OP_JZ] = i8080_state_Jnot_M2T3;
    cpu->m2t3[OP_JNC] = i8080_state_Jnot_M2T3;
    cpu->m2t3[OP_JC] = i8080_state_Jnot_M2T3;
    cpu->m2t3[OP_JPO] = i8080_state_Jnot_M2T3;
    cpu->m2t3[OP_JPE] = i8080_state_Jnot_M2T3;
    cpu->m2t3[OP_JP] = i8080_state_Jnot_M2T3;
    cpu->m2t3[OP_JM] = i8080_state_Jnot_M2T3;

    cpu->m3t3[OP_JMP] = i8080_state_JMP_M3T3;
    cpu->m3t3[OP_JNZ] = i8080_state_Jnot_M3T3;
    cpu->m3t3[OP_JZ] = i8080_state_Jnot_M3T3;
    cpu->m3t3[OP_JNC] = i8080_state_Jnot_M3T3;
    cpu->m3t3[OP_JC] = i8080_state_Jnot_M3T3;
    cpu->m3t3[OP_JPO] = i8080_state_Jnot_M3T3;
    cpu->m3t3[OP_JPE] = i8080_state_Jnot_M3T3;
    cpu->m3t3[OP_JP] = i8080_state_Jnot_M3T3;
    cpu->m3t3[OP_JM] = i8080_state_Jnot_M3T3;
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

#define DECL_J_M1T4(CC) NAME_J_M1T4(CC) (i8080 cpu, int phase)

#define IMPL_J_M1T4(CC,FB,FV)                                    \
    static void DECL_J_M1T4(CC) {                                \
        switch (phase) {                                         \
        case PHI1_RISE:                                          \
            break;                                               \
        case PHI2_RISE:                                          \
            if ((cpu->FLAGS->value & FB) == FV) {                \
                cpu->m2t3[OP_J ## CC] = i8080_state_JMP_M2T3;    \
                cpu->m3t3[OP_J ## CC] = i8080_state_JMP_M3T3;    \
            } else {                                             \
                cpu->m2t3[OP_J ## CC] = i8080_state_Jnot_M2T3;   \
                cpu->m3t3[OP_J ## CC] = i8080_state_Jnot_M3T3;   \
            }                                                    \
            break;                                               \
        case PHI2_FALL:                                          \
            cpu->state_next = i8080_state_j_M1T5;                \
            break;                                               \
        }                                                        \
    }                                                            \
    DEFN_J_M1T4(CC)

IMPL_J_M1T4(NZ, FLAGS_Z, 0);
IMPL_J_M1T4(Z, FLAGS_Z, FLAGS_Z);

IMPL_J_M1T4(NC, FLAGS_CY, 0);
IMPL_J_M1T4(C, FLAGS_CY, FLAGS_CY);

IMPL_J_M1T4(PO, FLAGS_P, 0);
IMPL_J_M1T4(PE, FLAGS_P, FLAGS_P);

IMPL_J_M1T4(P, FLAGS_S, 0);
IMPL_J_M1T4(M, FLAGS_S, FLAGS_S);

static void i8080_state_j_M1T5(i8080 cpu, int phase)
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

static void i8080_state_JMP_M2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:
          data_to(cpu->Z, cpu->DATA->value);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_3bops_t1;
          break;
    }
}

static void i8080_state_JMP_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->W, cpu->DATA->value);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          addr_to(cpu->PC, (cpu->W->value << 8) | cpu->Z->value);
          data_z(cpu->W);
          data_z(cpu->Z);
          break;
    }
}

// "jf" is a conditional jump when condition is FALSE

static void i8080_state_Jnot_M2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:
          data_to(cpu->Z, cpu->DATA->value);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_3bops_t1;
          break;
    }
}

static void i8080_state_Jnot_M3T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_lo(cpu->WAIT);
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          data_to(cpu->W, cpu->DATA->value);
          edge_lo(cpu->DBIN);
          addr_z(cpu->ADDR);
          break;
      case PHI2_FALL:
          // since the condition was false, do NOT copy WZ into PC.
          data_z(cpu->W);
          data_z(cpu->Z);
          break;
    }
}
