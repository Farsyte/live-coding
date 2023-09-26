#include "i8080_impl.h"
#include "support.h"

// i8080_alu: manage many ALU instructions

static unsigned     i8080_alu_flags(unsigned alu, unsigned hc);

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

// Table of functions: indexed by the "sss" bits of the
// opcode, giving the proper function.

static p8080State   i8080_state_aluT4[8] = {
    NAME_M1T4(B),
    NAME_M1T4(C),
    NAME_M1T4(D),
    NAME_M1T4(E),
    NAME_M1T4(H),
    NAME_M1T4(L),
    0,
    NAME_M1T4(A),
};

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
    for (int ooo = 0; ooo < 8; ++ooo) {
        for (int sss = 0; sss < 8; ++sss) {
            p8080State          m1t4 = i8080_state_aluT4[sss];
            if (NULL == m1t4)
                continue;
            Byte                op = 0x80 | (ooo << 3) | sss;
            cpu->m1t4[op] = m1t4;
            cpu->m1t2[op] = i8080_state_aluT2[ooo];
            // yes, we are getting here.
            // STUB("i8080_alu_init: op %02X m1t2 %p", op, (void *)(cpu->m1t2[op]));
        }
    }

    unsigned            flags = cpu->FLAGS->value;
    flags |= 0x02;
    flags &= ~0x28;

    cpu->FLAGS->value = flags;
}

// The IMPL_M1T4(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M1T4(R)                                            \
    static void NAME_M1T4(R) (i8080 cpu, int phase) {           \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            edge_hi(cpu->RETM1_INT);                            \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->TMP, cpu->R->value);                   \
            data_to(cpu->ACT, cpu->A->value);                   \
            break;                                              \
        case PHI2_FALL:                                         \
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

// There is no good IMPL_M1T2 macro: too much varies between
// the functions for it to be a win.

// i8080_state_aluT2ADD: trigger ALU ADD on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Needs bespoke logic for the Alternate Carry flag,
// but can use shared logic for the other flags, as long as we send it
// the result including the carry in bit 8.

static void i8080_state_aluT2ADD(i8080 cpu, int phase)
{
    (void)phase;

    unsigned            act = cpu->ACT->value;
    unsigned            tmp = cpu->TMP->value;
    unsigned            alu = act + tmp;
    unsigned            ac = ((act & 15) + (tmp & 15)) & 16;

    data_to(cpu->ALU, alu);
    data_to(cpu->A, alu);
    data_to(cpu->FLAGS, i8080_alu_flags(alu, ac));
}

// i8080_state_aluT2ADC: trigger ALU ADC on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Needs bespoke logic for the Alternate Carry flag,
// but can use shared logic for the other flags, as long as we send it
// the result including the carry in bit 8.

static void i8080_state_aluT2ADC(i8080 cpu, int phase)
{
    (void)phase;

    unsigned            act = cpu->ACT->value;
    unsigned            tmp = cpu->TMP->value;
    unsigned            cy = cpu->FLAGS->value & FLAGS_CY;
    unsigned            alu = act + tmp + cy;
    unsigned            ac = ((act & 15) + (tmp & 15) + cy) & 16;

    data_to(cpu->ALU, alu);
    data_to(cpu->A, alu);
    data_to(cpu->FLAGS, i8080_alu_flags(alu, ac));
}

// i8080_state_aluT2SUB: trigger ALU SUB on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Needs bespoke logic for the Alternate Carry flag,
// but can use shared logic for the other flags, as long as we send it
// the result including the borrow in bit 8.

static void i8080_state_aluT2SUB(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = cpu->ACT->value;
    unsigned            tmp = cpu->TMP->value;
    unsigned            alu = act - tmp;
    unsigned            ac = ((act & 15) - (tmp & 15)) & 16;

    data_to(cpu->ALU, alu);
    data_to(cpu->A, alu);
    data_to(cpu->FLAGS, i8080_alu_flags(alu, ac));
}

// i8080_state_aluT2SBB: trigger ALU SBB on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Needs bespoke logic for the Alternate Carry flag,
// but can use shared logic for the other flags, as long as we send it
// the result including the borrow in bit 8.

static void i8080_state_aluT2SBB(i8080 cpu, int phase)
{
    (void)phase;

    unsigned            act = cpu->ACT->value;
    unsigned            tmp = cpu->TMP->value;
    unsigned            cy = cpu->FLAGS->value & FLAGS_CY;
    unsigned            ac = ((act & 15) - (tmp & 15) - cy) & 16;
    unsigned            alu = act - tmp - cy;

    data_to(cpu->ALU, alu);
    data_to(cpu->A, alu);
    data_to(cpu->FLAGS, i8080_alu_flags(alu, ac));
}

// i8080_state_aluT2ANA: trigger ALU ANA on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Always clears the Cy and AC flags, and use shared
// logic for the other flags.

static void i8080_state_aluT2ANA(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = cpu->ACT->value;
    unsigned            tmp = cpu->TMP->value;
    unsigned            alu = act & tmp;

    data_to(cpu->ALU, alu);
    data_to(cpu->A, alu);
    data_to(cpu->FLAGS, i8080_alu_flags(alu, 0));
}

// i8080_state_aluT2XRA: trigger ALU XRA on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Always clears the Cy and AC flags, and use shared
// logic for the other flags.

static void i8080_state_aluT2XRA(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = cpu->ACT->value;
    unsigned            tmp = cpu->TMP->value;
    unsigned            alu = act ^ tmp;

    data_to(cpu->ALU, alu);
    data_to(cpu->A, alu);
    data_to(cpu->FLAGS, i8080_alu_flags(alu, 0));
}

// i8080_state_aluT2ORA: trigger ALU ORA on ACT and TMP, storing the
// result back into the accumulator and updating the flags
// appropriately. Always clears the Cy and AC flags, and use shared
// logic for the other flags.

static void i8080_state_aluT2ORA(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = cpu->ACT->value;
    unsigned            tmp = cpu->TMP->value;
    unsigned            alu = act | tmp;

    data_to(cpu->ALU, alu);
    data_to(cpu->A, alu);
    data_to(cpu->FLAGS, i8080_alu_flags(alu, 0));
}

// i8080_state_aluT2CMP: trigger ALU CMP on ACT and TMP, which
// updates the flags but does NOT modify the accumulator. Sets
// all flags the same as SUB does.

static void i8080_state_aluT2CMP(i8080 cpu, int phase)
{

    (void)phase;

    unsigned            act = cpu->ACT->value;
    unsigned            tmp = cpu->TMP->value;
    unsigned            alu = act - tmp;
    unsigned            ac = ((act & 15) - (tmp & 15)) & 16;

    data_to(cpu->ALU, alu);

    data_to(cpu->FLAGS, i8080_alu_flags(alu, ac));
}

// i8080_alu_flags: update the flags based on the extended
// result of the ALU operation, with the aux carry provided
// separately.

static unsigned i8080_alu_flags(unsigned alu, unsigned ac)
{

    unsigned            cy = alu >> 8;
    cy &= FLAGS_CY;

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