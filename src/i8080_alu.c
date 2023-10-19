#include "chip/i8080_flags.h"
#include "chip/i8080_impl.h"
#include "common/support.h"

// i8080_alu: manage many ALU instructions

static unsigned     i8080_alu_flags(unsigned alu, unsigned hc);

static f8080State   i8080_state_daaT4;
static f8080State   i8080_state_rlcT4;
static f8080State   i8080_state_rrcT4;
static f8080State   i8080_state_ralT4;
static f8080State   i8080_state_rarT4;

static f8080State   i8080_state_cmaT4;
static f8080State   i8080_state_cmcT4;
static f8080State   i8080_state_stcT4;

static f8080State   i8080_state_rlcT2;
static f8080State   i8080_state_rrcT2;
static f8080State   i8080_state_ralT2;
static f8080State   i8080_state_rarT2;

// i8080_alu uses a collection of t-state functions to handle
// the distinct M1T4 operations, which for "ALU R" operations,
// comprises one function per (second ALU operand) register.
//
// The NAME_M1T4 macro uses token-pasting to fuse the name of
// the register with the generic function name.
//
// The DEFN_M1T4 macro generates the forward declaration for
// the function, using the "f8080State" type which provides
// the return type and parameter types.

#define NAME_M1T4(R)	i8080_state_aluT4 ## R
#define DEFN_M1T4(R)	static f8080State NAME_M1T4(R)

// Enumerate the functions: one per 8-bit register.

DEFN_M1T4(B);
DEFN_M1T4(C);
DEFN_M1T4(D);
DEFN_M1T4(E);
DEFN_M1T4(H);
DEFN_M1T4(L);
DEFN_M1T4(A);

static f8080State   i8080_state_aluimmM1T4;
static f8080State   i8080_state_aluimmM2T3;
static f8080State   i8080_state_aluT4rdM;

// Table of functions: indexed by the "sss" bits of the
// opcode, giving the proper function.

static p8080State   i8080_state_aluT4[8] = {
    NAME_M1T4(B),
    NAME_M1T4(C),
    NAME_M1T4(D),
    NAME_M1T4(E),
    NAME_M1T4(H),
    NAME_M1T4(L),
    i8080_state_aluT4rdM,
    NAME_M1T4(A),
};

// ALU operations taking M as an operand need to do a machine
// cycle that reads from (HL) into TMP.

static f8080State   i8080_state_aluM2T1rdM;
static f8080State   i8080_state_aluM2T2rdM;
static f8080State   i8080_state_aluM2TWrdM;
static f8080State   i8080_state_aluM2T3rdM;

// i8080_alu uses a collection of t-state functions to handle the
// distinct M1T2 operations which are used to compute the ALU result
// and store it back into the A register. 
//
// The NAME_M1T2 macro uses token-pasting to fuse the name of
// the register with the generic function name.
//
// The DEFN_M1T2 macro generates the forward declaration for
// the function, using the "f8080State" type which provides
// the return type and parameter types.
//
// These functions are only called during PHI2_RISE phase,
// at least for now.

#define NAME_M1T2(R)	i8080_state_aluT2 ## R
#define DEFN_M1T2(R)	static f8080State NAME_M1T2(R)

// Enumerate the ALU operations, to declare the functions.

DEFN_M1T2(ADD);
DEFN_M1T2(ADC);
DEFN_M1T2(SUB);
DEFN_M1T2(SBB);
DEFN_M1T2(ANA);
DEFN_M1T2(XRA);
DEFN_M1T2(ORA);
DEFN_M1T2(CMP);

// Table of functions: indexed by the "ALU OP" bits of
// the opcode.

static p8080State   i8080_state_aluT2[8] = {
    NAME_M1T2(ADD),
    NAME_M1T2(ADC),
    NAME_M1T2(SUB),
    NAME_M1T2(SBB),
    NAME_M1T2(ANA),
    NAME_M1T2(XRA),
    NAME_M1T2(ORA),
    NAME_M1T2(CMP),
};

// i8080_alu_init: set up decoding for ALU instructions
// operating purely on our eight-bit registers.

void i8080_alu_init(i8080 cpu)
{

    cpu->m1t4[OP_DAA] = i8080_state_daaT4;

    cpu->m1t4[OP_RLC] = i8080_state_rlcT4;
    cpu->m1t2[OP_RLC] = i8080_state_rlcT2;

    cpu->m1t4[OP_RRC] = i8080_state_rrcT4;
    cpu->m1t2[OP_RRC] = i8080_state_rrcT2;

    cpu->m1t4[OP_RAL] = i8080_state_ralT4;
    cpu->m1t2[OP_RAL] = i8080_state_ralT2;

    cpu->m1t4[OP_RAR] = i8080_state_rarT4;
    cpu->m1t2[OP_RAR] = i8080_state_rarT2;

    cpu->m1t4[OP_CMA] = i8080_state_cmaT4;
    cpu->m1t4[OP_CMC] = i8080_state_cmcT4;
    cpu->m1t4[OP_STC] = i8080_state_stcT4;

    for (int ooo = 0; ooo < 8; ++ooo) {
        for (int sss = 0; sss < 8; ++sss) {
            p8080State          m1t4 = i8080_state_aluT4[sss];
            Byte                op = 0x80 | (ooo << 3) | sss;
            cpu->m1t4[op] = m1t4;
            cpu->m1t2[op] = i8080_state_aluT2[ooo];
        }
    }

    for (int ooo = 0; ooo < 8; ++ooo) {
        Byte                op = 0xC0 | (ooo << 3) | 6;
        cpu->m1t4[op] = i8080_state_aluimmM1T4;
        cpu->m2t3[op] = i8080_state_aluimmM2T3;
        cpu->m1t2[op] = i8080_state_aluT2[ooo];
    }

    unsigned            flags = VAL(FLAGS);
    flags |= 0x02;
    flags &= ~0x28;

    VAL(FLAGS) = flags;
}

// i8080_state_daaT4: implement Decimal Adjust It is possible that the
// block in 8080 called DAA is actually logic or a latch that
// maintains the decimal-adjusted result of the prior ALU operation.
// We do not, however, want to take the performance hit implied when
// simulating this logic in every arithmetic operation, so the math
// is actually implemented here.

static void i8080_state_daaT4(i8080 cpu, int phase)
{
    unsigned            alu;
    unsigned            daa;
    unsigned            ac;

    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:

          // pretend we had a DAA latch that was set with the results
          // of this computation, in EVERY arithmetic instruction.

          daa = alu = VAL(A);
          if (((daa & 15) > 9) || (VAL(FLAGS) & FLAGS_AC))
              daa += 0x06;
          if ((((daa >> 4) & 15) > 9) || (VAL(FLAGS) & FLAGS_CY))
              daa += 0x60;

          ac = (daa & 15) < (alu & 15) ? FLAGS_AC : 0;;
          DSET(A, daa);
          DSET(FLAGS, i8080_alu_flags(daa, ac));

          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_rlcT4(): rotate left circular
// barrel rotate A left, retain input for T2 ops

static void i8080_state_rlcT4(i8080 cpu, int phase)
{
    unsigned            act;
    unsigned            alu;

    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          act = VAL(A);
          alu = (act << 1) | (act >> 7);
          DSET(ACT, act);
          DSET(ALU, alu);
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_rlcT2(): finish RLC
// copy MSBit into CY, publish A and Flags results.

static void i8080_state_rlcT2(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            alu = VAL(ALU);
    unsigned            f = VAL(FLAGS);

    if (act & 0x80)
        f |= FLAGS_CY;
    else
        f &= ~FLAGS_CY;

    DSET(A, alu);
    DSET(FLAGS, f);
}

// i8080_state_rrcT4(): rotate left circular
// barrel rotate A right, retain input for T2 ops

static void i8080_state_rrcT4(i8080 cpu, int phase)
{
    unsigned            act;
    unsigned            alu;

    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          act = VAL(A);
          alu = (act >> 1) | (act << 7);
          DSET(ACT, act);
          DSET(ALU, alu);
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_rrcT2(): finish RRC
// copy LSBit into CY, publish A and Flags results.

static void i8080_state_rrcT2(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            alu = VAL(ALU);
    unsigned            f = VAL(FLAGS);

    if (act & 0x01)
        f |= FLAGS_CY;
    else
        f &= ~FLAGS_CY;

    DSET(A, alu);
    DSET(FLAGS, f);
}

// i8080_state_ralT4(): rotate left through carry
// rotate left including carry: carry goes into LSBit

static void i8080_state_ralT4(i8080 cpu, int phase)
{
    unsigned            act;
    unsigned            alu;

    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          act = VAL(A);
          alu = (act << 1) | (VAL(FLAGS) & FLAGS_CY);
          DSET(ACT, act);
          DSET(ALU, alu);
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_ralT2(): finish RAL
// copy MSBit of input into CY, commit results to A and Flags

static void i8080_state_ralT2(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            alu = VAL(ALU);
    unsigned            f = VAL(FLAGS);

    if (act & 0x80)
        f |= FLAGS_CY;
    else
        f &= ~FLAGS_CY;

    DSET(A, alu);
    DSET(FLAGS, f);
}

// i8080_state_rarT4(): rotate right through carry
// rotate right including carry: carry goes into MSBit

static void i8080_state_rarT4(i8080 cpu, int phase)
{
    unsigned            act;
    unsigned            alu;

    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          act = VAL(A);
          alu = ((VAL(FLAGS) & FLAGS_CY) << 7) | (act >> 1);
          DSET(ACT, act);
          DSET(ALU, alu);
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_ralT2(): finish RAR
// copy LSBit of input into CY, commit results to A and Flags

static void i8080_state_rarT2(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            alu = VAL(ALU);
    unsigned            f = VAL(FLAGS);

    if (act & 0x01)
        f |= FLAGS_CY;
    else
        f &= ~FLAGS_CY;

    DSET(A, alu);
    DSET(FLAGS, f);
}

// i8080_state_cmaT4: Complement Accumulator

static void i8080_state_cmaT4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          DSET(A, ~VAL(A));
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_cmaT4: Complement Carry Flag

static void i8080_state_cmcT4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          DSET(FLAGS, FLAGS_CY ^ VAL(FLAGS));
          break;
      case PHI2_FALL:
          break;
    }
}

// i8080_state_cmaT4: Set Carry Flag

static void i8080_state_stcT4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          DSET(FLAGS, FLAGS_CY | VAL(FLAGS));
          break;
      case PHI2_FALL:
          break;
    }
}

// The IMPL_M1T4(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M1T4(R)                                                    \
    static void NAME_M1T4(R) (i8080 cpu, int phase) {                   \
        switch (phase) {                                                \
        case PHI1_RISE:                                                 \
            RAISE(RETM1_INT);                                           \
            break;                                                      \
        case PHI2_RISE:                                                 \
            DSET(TMP, VAL(R));                                          \
            DSET(ACT, VAL(A));                                          \
            break;                                                      \
        case PHI2_FALL:                                                 \
            break;                                                      \
        }                                                               \
    }                                                                   \
    DEFN_M1T4(R)

IMPL_M1T4(A);
IMPL_M1T4(B);
IMPL_M1T4(C);
IMPL_M1T4(D);
IMPL_M1T4(E);
IMPL_M1T4(H);
IMPL_M1T4(L);

// This is the M1T4 cycle shared by the eight ALU ops
// when the 2nd operand is the 2nd byte of the opcode.
static void i8080_state_aluimmM1T4(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          DSET(ACT, VAL(A));
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->state_2bops_t1;
          break;
    }
}

// This is the M2T3 cycle shared by the eight ALU ops
// when the 2nd operand is the 2nd byte of the opcode.
static void i8080_state_aluimmM2T3(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          DSET(TMP, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          break;
    }
}

// ALU operations taking M as an operand need to do a machine
// cycle that reads from (HL) into TMP.

// The M1T4 cycle for ALU ops with M as an operand
// latch ACC into ACT as above, then turn control over
// to the logic to drive an MREAD cycle.

static void i8080_state_aluT4rdM(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          DSET(ACT, VAL(A));
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_aluM2T1rdM;
          break;
    }
}

// The M2T1 state for ALU ops taking M initiates a memory read cycle:
// put HL into IDAL and ADDR, put MREAD status onto DATA, raise SYNC,
// and route control to the M2T2 handler.

static void i8080_state_aluM2T1rdM(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          ASET(IDAL, RP(H, L));
          ASET(ADDR, VAL(IDAL));
          DSET(DATA, STATUS_MREAD);
          RAISE(SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = i8080_state_aluM2T2rdM;
          break;
    }
}

// The M2T2 state for ALU ops taking M continues a memory read cycle:
// drop SYNC, release DATA, and raise DBIN. Check READY to determine
// whether or not to insert a WAIT state; if so, use M2TW; otherwise
// proceed to M2T3.

static void i8080_state_aluM2T2rdM(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          LOWER(SYNC);
          DTRI(DATA);
          RAISE(DBIN);
          break;
      case PHI2_FALL:
          if (VAL(READY))
              cpu->state_next = i8080_state_aluM2T3rdM;
          else
              cpu->state_next = i8080_state_aluM2TWrdM;
          break;
    }
}

// The M2TW state for ALU ops taking M continues a memory read cycle:
// raise WAIT at the start of the cycle, re-issue the DBIN rising edge
// notification (allowing the memory to reconsider READY), then Check
// READY to determine whether or not to insert another WAIT state; if
// so, use M2TW; otherwise proceed to M2T3.

static void i8080_state_aluM2TWrdM(i8080 cpu, int phase)
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
              cpu->state_next = i8080_state_aluM2T3rdM;
          else
              cpu->state_next = i8080_state_aluM2TWrdM;
          break;
    }
}

// The M2TW state for ALU ops taking M completes a memory read cycle:
// lower WAIT at the start of the cycle (in case it was raise), copy
// the DATA bus into the TMP register, lower DBIN, and tristate ADDR.
// Raise RETM1_INT at the start of the state to trigger the logic
// leading into the next opcode fetch (or interrupt) cycle; the ALU
// operation and writeback will happen in the T2 state of the next M1
// machine cycle.

static void i8080_state_aluM2T3rdM(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          LOWER(WAIT);
          RAISE(RETM1_INT);
          break;
      case PHI2_RISE:
          DSET(TMP, VAL(DATA));
          LOWER(DBIN);
          ATRI(ADDR);
          break;
      case PHI2_FALL:
          break;
    }
}

// During M1T2 after an ALU operation, the 8080 writes the ALU result
// back into ACC and updates FLAGS. Support for the Aux Carry forces
// us to perform some ALU operations separately on just the low four
// bits; strategies to make this "lazy" might some day be found that
// do not impose a steady runtime cost.

// i8080_state_aluT2ADD: trigger ALU ADD on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Needs bespoke logic for the Alternate Carry flag,
// but can use shared logic for the other flags, as long as we send it
// the result including the carry in bit 8.

static void i8080_state_aluT2ADD(i8080 cpu, int phase)
{
    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            tmp = VAL(TMP);
    unsigned            alu = act + tmp;
    unsigned            ac = ((act & 15) + (tmp & 15)) & 16;

    DSET(ALU, alu);
    DSET(A, alu);
    DSET(FLAGS, i8080_alu_flags(alu, ac));
}

// i8080_state_aluT2ADC: trigger ALU ADC on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Needs bespoke logic for the Alternate Carry flag,
// but can use shared logic for the other flags, as long as we send it
// the result including the carry in bit 8.

static void i8080_state_aluT2ADC(i8080 cpu, int phase)
{
    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            tmp = VAL(TMP);
    unsigned            cy = VAL(FLAGS) & FLAGS_CY;
    unsigned            alu = act + tmp + cy;
    unsigned            ac = ((act & 15) + (tmp & 15) + cy) & 16;

    DSET(ALU, alu);
    DSET(A, alu);
    DSET(FLAGS, i8080_alu_flags(alu, ac));
}

// i8080_state_aluT2SUB: trigger ALU SUB on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Needs bespoke logic for the Alternate Carry flag,
// but can use shared logic for the other flags, as long as we send it
// the result including the borrow in bit 8.

static void i8080_state_aluT2SUB(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            tmp = VAL(TMP);
    unsigned            alu = act - tmp;
    unsigned            ac = ((act & 15) - (tmp & 15)) & 16;

    DSET(ALU, alu);
    DSET(A, alu);
    DSET(FLAGS, i8080_alu_flags(alu, ac));
}

// i8080_state_aluT2SBB: trigger ALU SBB on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Needs bespoke logic for the Alternate Carry flag,
// but can use shared logic for the other flags, as long as we send it
// the result including the borrow in bit 8.

static void i8080_state_aluT2SBB(i8080 cpu, int phase)
{
    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            tmp = VAL(TMP);
    unsigned            cy = VAL(FLAGS) & FLAGS_CY;
    unsigned            ac = ((act & 15) - (tmp & 15) - cy) & 16;
    unsigned            alu = act - tmp - cy;

    DSET(ALU, alu);
    DSET(A, alu);
    DSET(FLAGS, i8080_alu_flags(alu, ac));
}

// i8080_state_aluT2ANA: trigger ALU ANA on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Always clears the Cy and AC flags, and use shared
// logic for the other flags.

static void i8080_state_aluT2ANA(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            tmp = VAL(TMP);
    unsigned            alu = act & tmp;

    DSET(ALU, alu);
    DSET(A, alu);
    DSET(FLAGS, i8080_alu_flags(alu, 0));
}

// i8080_state_aluT2XRA: trigger ALU XRA on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Always clears the Cy and AC flags, and use shared
// logic for the other flags.

static void i8080_state_aluT2XRA(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            tmp = VAL(TMP);
    unsigned            alu = act ^ tmp;

    DSET(ALU, alu);
    DSET(A, alu);
    DSET(FLAGS, i8080_alu_flags(alu, 0));
}

// i8080_state_aluT2ORA: trigger ALU ORA on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Always clears the Cy and AC flags, and use shared
// logic for the other flags.

static void i8080_state_aluT2ORA(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            tmp = VAL(TMP);
    unsigned            alu = act | tmp;

    DSET(ALU, alu);
    DSET(A, alu);
    DSET(FLAGS, i8080_alu_flags(alu, 0));
}

// i8080_state_aluT2CMP: trigger ALU CMP on ACT and TMP, which
// updates the flags but does NOT modify the accumulator. Sets
// all flags the same as SUB does.

static void i8080_state_aluT2CMP(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = VAL(ACT);
    unsigned            tmp = VAL(TMP);
    unsigned            alu = act - tmp;
    unsigned            ac = ((act & 15) - (tmp & 15)) & 16;

    DSET(ALU, alu);

    DSET(FLAGS, i8080_alu_flags(alu, ac));

}

// i8080_alu_flags: update the flags based on the extended
// result of the ALU operation, with the aux carry provided
// separately.

static unsigned i8080_alu_flags(unsigned alu, unsigned ac)
{
    unsigned            cy = (alu >> 8) & FLAGS_CY;
    unsigned            p = alu;
    unsigned            s = alu & 0x80;
    unsigned            z = alu ? 0 : FLAGS_Z;

    // FLAGS_P is set if there are an even number of bits
    // set in the low 8 bits of the result.

    p ^= p >> 4;
    p ^= p >> 2;
    p ^= p >> 1;
    p = (p & 1) ? 0 : FLAGS_P;

    // The 0x02 bit is always on. The 0x28 bits are always off.

    return cy | 0x02 | p | ac | z | s;

}
