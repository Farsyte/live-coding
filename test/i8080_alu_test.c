#include "clock.h"
#include "i8080_impl.h"

// i8080_alu: manage most ALU instructions
//
// This implementation uses one individual T-state management function
// for each source register, and one for each destination; each of the
// instructions will mix-and-match the correct source and destination.

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         i8080_alu_bist_add(CpuTestSys ts);
static void         i8080_alu_bist_sub(CpuTestSys ts);
static void         i8080_alu_bist_adc(CpuTestSys ts);
static void         i8080_alu_bist_sbb(CpuTestSys ts);
static void         i8080_alu_bist_ana(CpuTestSys ts);
static void         i8080_alu_bist_xra(CpuTestSys ts);
static void         i8080_alu_bist_ora(CpuTestSys ts);
static void         i8080_alu_bist_cmp(CpuTestSys ts);
static void         i8080_alu_bist_daa(CpuTestSys ts);

static void         i8080_alu_bist_rot(CpuTestSys ts);

static void         i8080_alu_post_add(CpuTestSys ts);
static void         i8080_alu_post_sub(CpuTestSys ts);
static void         i8080_alu_post_adc(CpuTestSys ts);
static void         i8080_alu_post_sbb(CpuTestSys ts);
static void         i8080_alu_post_ana(CpuTestSys ts);
static void         i8080_alu_post_xra(CpuTestSys ts);
static void         i8080_alu_post_ora(CpuTestSys ts);
static void         i8080_alu_post_cmp(CpuTestSys ts);
static void         i8080_alu_post_daa(CpuTestSys ts);

static void         i8080_alu_post_rot(CpuTestSys ts);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

// i8080_alu_post: like bist but no output, no sigtrace

void i8080_alu_post(CpuTestSys ts)
{
    i8080_reset_for_testing(ts);
    i8080_alu_post_add(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post_sub(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post_adc(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post_sbb(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post_ana(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post_xra(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post_ora(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post_cmp(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post_daa(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post_rot(ts);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_alu_bist(CpuTestSys ts)
{
    i8080_reset_for_testing(ts);
    i8080_alu_bist_add(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist_sub(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist_adc(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist_sbb(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist_ana(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist_xra(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist_ora(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist_cmp(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist_daa(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist_rot(ts);
}

// === === === === === === === === === === === === === === === ===
//                    BUILT-IN SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static void i8080_alu_bist_add(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_ADD__A,
        OP_MVI__B, 0x01, OP_ADD__B,
        OP_MVI__C, 0x02, OP_ADD__C,
        OP_MVI__D, 0x04, OP_ADD__D,
        OP_MVI__E, 0x10, OP_ADD__E,
        OP_MVI__H, 0x20, OP_ADD__H,
        OP_MVI__L, 0x40, OP_ADD__L,
        OP_ADI, 0x0C,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // ADD_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, A->value);
    ASSERT_EQ_integer(0x12, FLAGS->value);      // Ac

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // ADD_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x11, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // ADD_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x13, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // ADD_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x17, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // ADD_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x27, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // ADD_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x47, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // ADD_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x87, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) ADI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x93, A->value);
    ASSERT_EQ_integer(0x96, FLAGS->value);      // S,Ac,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_add",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "ADD from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}

static void i8080_alu_bist_sub(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_SUB__A,
        OP_MVI__B, 0x01, OP_SUB__B,
        OP_MVI__C, 0x02, OP_SUB__C,
        OP_MVI__D, 0x04, OP_SUB__D,
        OP_MVI__E, 0x10, OP_SUB__E,
        OP_MVI__H, 0x20, OP_SUB__H,
        OP_MVI__L, 0x40, OP_SUB__L,
        OP_SUI, 0x0C,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SUB are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // SUB_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // SUB_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFF, A->value);
    ASSERT_EQ_integer(0x97, FLAGS->value);      // S,P,Cy

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // SUB_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFD, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // SUB_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF9, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // SUB_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xE9, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // SUB_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xC9, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // SUB_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x89, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) SUI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x7D, A->value);
    ASSERT_EQ_integer(0x16, FLAGS->value);      // Ac,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_sub",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "SUB from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}

static void i8080_alu_bist_adc(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_ADC__A,
        OP_MVI__B, 0x01, OP_ADC__B,
        OP_MVI__C, 0x02, OP_ADC__C,
        OP_MVI__D, 0x04, OP_ADC__D,
        OP_MVI__E, 0x10, OP_ADC__E,
        OP_MVI__H, 0x20, OP_ADC__H,
        OP_MVI__L, 0x40, OP_ADC__L,
        OP_ACI, 0x0C,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADC are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // ADC_A (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x11, A->value);
    ASSERT_EQ_integer(0x16, FLAGS->value);      // Ac,P

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // ADC_B (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x13, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // ADC_C (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x16, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // ADC_D (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x1B, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // ADC_E (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x2C, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // ADC_H (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x4D, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // ADC_L (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x8E, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) ACI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x9A, A->value);
    ASSERT_EQ_integer(0x96, FLAGS->value);      // S,Ac,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_adc",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "ADC from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}

static void i8080_alu_bist_sbb(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_SBB__A,
        OP_MVI__B, 0x01, OP_SBB__B,
        OP_MVI__C, 0x02, OP_SBB__C,
        OP_MVI__D, 0x04, OP_SBB__D,
        OP_MVI__E, 0x10, OP_SBB__E,
        OP_MVI__H, 0x20, OP_SBB__H,
        OP_MVI__L, 0x40, OP_SBB__L,
        OP_SBI, 0x0C,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SBB are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // SBB_A (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFF, A->value);
    ASSERT_EQ_integer(0x97, FLAGS->value);      // S,Ac,P,Cy

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // SBB_B (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFD, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // SBB_C (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFA, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // SBB_D (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF5, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // SBB_E (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xE4, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // SBB_H (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xC3, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // SBB_L (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x82, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) SBI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x76, A->value);
    ASSERT_EQ_integer(0x12, FLAGS->value);      // Ac

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_sbb",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "SBB from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}

static void i8080_alu_bist_ana(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0xF7, OP_ANA__A,
        OP_MVI__B, 0xFE, OP_ANA__B,
        OP_MVI__C, 0xFD, OP_ANA__C,
        OP_MVI__D, 0xFB, OP_ANA__D,
        OP_MVI__E, 0xF5, OP_ANA__E,
        OP_MVI__H, 0xEB, OP_ANA__H,
        OP_MVI__L, 0xD7, OP_ANA__L,
        OP_ANI, 0x0F,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ANA are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF7, A->value);

    // ANA_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF7, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFE, B->value);

    // ANA_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF6, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFD, C->value);

    // ANA_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF4, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFB, D->value);

    // ANA_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF0, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF5, E->value);

    // ANA_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF0, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xEB, H->value);

    // ANA_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xE0, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xD7, L->value);

    // ANA_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xC0, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) ANI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_ana",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "ANA from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}

static void i8080_alu_bist_ora(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_ORA__A,
        OP_MVI__B, 0x01, OP_ORA__B,
        OP_MVI__C, 0x02, OP_ORA__C,
        OP_MVI__D, 0x04, OP_ORA__D,
        OP_MVI__E, 0x10, OP_ORA__E,
        OP_MVI__H, 0x20, OP_ORA__H,
        OP_MVI__L, 0x40, OP_ORA__L,
        OP_ORI, 0x80,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ORA are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // ORA_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // ORA_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x09, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // ORA_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x0B, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // ORA_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x0F, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // ORA_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x1F, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // ORA_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x3F, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // ORA_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x7F, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) ORI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFF, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_ora",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "ORA from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}

static void i8080_alu_bist_xra(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_XRA__A,
        OP_MVI__B, 0x01, OP_XRA__B,
        OP_MVI__C, 0x02, OP_XRA__C,
        OP_MVI__D, 0x04, OP_XRA__D,
        OP_MVI__E, 0x10, OP_XRA__E,
        OP_MVI__H, 0x20, OP_XRA__H,
        OP_MVI__L, 0x40, OP_XRA__L,
        OP_XRI, 0xCC,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_XRA are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // XRA_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // XRA_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // XRA_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x03, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // XRA_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x07, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // XRA_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x17, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // XRA_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x37, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // XRA_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x77, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) XRI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xBB, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_xra",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "XRA from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}

static void i8080_alu_bist_cmp(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_CMP__A,
        OP_MVI__B, 0x01, OP_CMP__B,
        OP_MVI__C, 0x02, OP_CMP__C,
        OP_MVI__D, 0x04, OP_CMP__D,
        OP_MVI__E, 0x10, OP_CMP__E,
        OP_MVI__H, 0x20, OP_CMP__H,
        OP_MVI__L, 0x40, OP_CMP__L,
        OP_CPI, 0x08,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_CMP are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // CMP_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // CMP_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // CMP_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // CMP_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // CMP_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x83, FLAGS->value);

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // CMP_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x87, FLAGS->value);

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // CMP_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x83, FLAGS->value);

    // (starting with T3) CPI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_cmp",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "CMP from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}

static void i8080_alu_bist_daa(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         C = cpu->C;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x69,
        OP_MVI__C, 0x58,
        OP_ADD__C,
        OP_DAA,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SUB are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // MVI A,0x69
    ASSERT_EQ_integer(0x69, A->value);
    i8080_one_instruction(cpu, 0);      // MVI C,0x58
    ASSERT_EQ_integer(0x58, C->value);
    i8080_one_instruction(cpu, 18);     // ADD C (results in T2 of next instr)
    ASSERT_EQ_integer(0xC1, A->value);
    ASSERT_EQ_integer(0x92, FLAGS->value);      // S,Ac
    i8080_one_instruction(cpu, 0);      // DAA
    ASSERT_EQ_integer(0x27, A->value);
    ASSERT_EQ_integer(0x07, FLAGS->value);      // P,Cy
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_daa",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "DAA test", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}

static void i8080_alu_bist_rot(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;

    pSigSess            ss = ts->ss;
    SigPlot             sp;

    Byte                test_prog[] = {
        OP_XRA__A,
        OP_MVI__A, 0x5F,
        OP_RLC,
        OP_RLC,
        OP_NOP,

        OP_XRA__A,
        OP_MVI__A, 0xFA,
        OP_RRC,
        OP_RRC,
        OP_NOP,

        OP_XRA__A,
        OP_MVI__A, 0x5F,
        OP_RAL,
        OP_RAL,
        OP_RAL,
        OP_NOP,

        OP_XRA__A,
        OP_MVI__A, 0xFA,
        OP_RAR,
        OP_RAR,
        OP_RAR,
        OP_NOP,

        OP_NOP,
        OP_MVI__A, 0x5B,
        OP_CMA,
        OP_CMA,
        OP_NOP,

        OP_XRA__A,
        OP_CMC,
        OP_CMC,
        OP_NOP,

        OP_XRA__A,
        OP_STC,
        OP_STC,
        OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SUB are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    i8080_one_instruction(cpu, 18);     // XRA A (results in T2 of next instr)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A,0x5F
    ASSERT_EQ_integer(0x5F, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RLC
    ASSERT_EQ_integer(0xBE, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RLC
    ASSERT_EQ_integer(0x7D, A->value);
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // NOP

    sigplot_init(sp, ss, "i8080_alu_rlc",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "DAA test", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    t0 = TAU;

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A,0xFA
    ASSERT_EQ_integer(0xFA, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RRC
    ASSERT_EQ_integer(0x7D, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RRC
    ASSERT_EQ_integer(0xBE, A->value);
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // NOP

    sigplot_init(sp, ss, "i8080_alu_rrc",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "DAA test", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    t0 = TAU;

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A,0x5F
    ASSERT_EQ_integer(0x5F, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RAL
    ASSERT_EQ_integer(0xBE, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RAL
    ASSERT_EQ_integer(0x7C, A->value);
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 18);     // RAL
    ASSERT_EQ_integer(0xF9, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // NOP

    sigplot_init(sp, ss, "i8080_alu_ral",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "DAA test", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    t0 = TAU;

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A,0xFA
    ASSERT_EQ_integer(0xFA, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RAR
    ASSERT_EQ_integer(0x7D, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RAR
    ASSERT_EQ_integer(0x3E, A->value);
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 18);     // RAR
    ASSERT_EQ_integer(0x9F, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // NOP

    sigplot_init(sp, ss, "i8080_alu_rar",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "DAA test", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    t0 = TAU;

    // 0 1111 1010
    // 0 0111 1101
    // 1 0011 1110
    // 0 1001 1111

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 0);      // MVI A,0x5B
    ASSERT_EQ_integer(0x5B, A->value);

    i8080_one_instruction(cpu, 0);      // CMA
    ASSERT_EQ_integer(0xA4, A->value);

    i8080_one_instruction(cpu, 0);      // CMA
    ASSERT_EQ_integer(0x5B, A->value);

    i8080_one_instruction(cpu, 0);      // NOP

    sigplot_init(sp, ss, "i8080_alu_cma",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "DAA test", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    t0 = TAU;

    // 0 1111 1010
    // 0 0111 1101
    // 1 0011 1110
    // 0 1001 1111

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // CMC
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // CMC
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // NOP

    sigplot_init(sp, ss, "i8080_alu_cmc",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "DAA test", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    t0 = TAU;

    // 0 1111 1010
    // 0 0111 1101
    // 1 0011 1110
    // 0 1001 1111

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // STC
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // STC
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // NOP

    sigplot_init(sp, ss, "i8080_alu_stc",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "DAA test", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

}

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static void i8080_alu_post_add(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_ADD__A,
        OP_MVI__B, 0x01, OP_ADD__B,
        OP_MVI__C, 0x02, OP_ADD__C,
        OP_MVI__D, 0x04, OP_ADD__D,
        OP_MVI__E, 0x10, OP_ADD__E,
        OP_MVI__H, 0x20, OP_ADD__H,
        OP_MVI__L, 0x40, OP_ADD__L,
        OP_ADI, 0x0C,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // ADD_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, A->value);
    ASSERT_EQ_integer(0x12, FLAGS->value);      // Ac

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // ADD_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x11, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // ADD_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x13, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // ADD_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x17, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // ADD_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x27, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // ADD_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x47, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // ADD_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x87, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) ADI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x93, A->value);
    ASSERT_EQ_integer(0x96, FLAGS->value);      // S,Ac,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
}

static void i8080_alu_post_sub(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_SUB__A,
        OP_MVI__B, 0x01, OP_SUB__B,
        OP_MVI__C, 0x02, OP_SUB__C,
        OP_MVI__D, 0x04, OP_SUB__D,
        OP_MVI__E, 0x10, OP_SUB__E,
        OP_MVI__H, 0x20, OP_SUB__H,
        OP_MVI__L, 0x40, OP_SUB__L,
        OP_SUI, 0x0C,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SUB are not
    // written to the destination until
    // the T2 state of the next instruction.

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // SUB_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // SUB_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFF, A->value);
    ASSERT_EQ_integer(0x97, FLAGS->value);      // S,P,Cy

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // SUB_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFD, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // SUB_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF9, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // SUB_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xE9, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // SUB_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xC9, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // SUB_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x89, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) SUI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x7D, A->value);
    ASSERT_EQ_integer(0x16, FLAGS->value);      // Ac,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
}

static void i8080_alu_post_adc(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_ADC__A,
        OP_MVI__B, 0x01, OP_ADC__B,
        OP_MVI__C, 0x02, OP_ADC__C,
        OP_MVI__D, 0x04, OP_ADC__D,
        OP_MVI__E, 0x10, OP_ADC__E,
        OP_MVI__H, 0x20, OP_ADC__H,
        OP_MVI__L, 0x40, OP_ADC__L,
        OP_ACI, 0x0C,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADC are not
    // written to the destination until
    // the T2 state of the next instruction.

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // ADC_A (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x11, A->value);
    ASSERT_EQ_integer(0x16, FLAGS->value);      // Ac,P

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // ADC_B (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x13, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // ADC_C (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x16, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // ADC_D (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x1B, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // ADC_E (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x2C, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // ADC_H (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x4D, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // ADC_L (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x8E, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) ACI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x9A, A->value);
    ASSERT_EQ_integer(0x96, FLAGS->value);      // S,Ac,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
}

static void i8080_alu_post_sbb(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_SBB__A,
        OP_MVI__B, 0x01, OP_SBB__B,
        OP_MVI__C, 0x02, OP_SBB__C,
        OP_MVI__D, 0x04, OP_SBB__D,
        OP_MVI__E, 0x10, OP_SBB__E,
        OP_MVI__H, 0x20, OP_SBB__H,
        OP_MVI__L, 0x40, OP_SBB__L,
        OP_SBI, 0x0C,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SBB are not
    // written to the destination until
    // the T2 state of the next instruction.

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // SBB_A (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFF, A->value);
    ASSERT_EQ_integer(0x97, FLAGS->value);      // S,Ac,P,Cy

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // SBB_B (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFD, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // SBB_C (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFA, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // SBB_D (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF5, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // SBB_E (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xE4, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // SBB_H (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xC3, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // SBB_L (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x82, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) SBI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x76, A->value);
    ASSERT_EQ_integer(0x12, FLAGS->value);      // Ac

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
}

static void i8080_alu_post_ana(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0xF7, OP_ANA__A,
        OP_MVI__B, 0xFE, OP_ANA__B,
        OP_MVI__C, 0xFD, OP_ANA__C,
        OP_MVI__D, 0xFB, OP_ANA__D,
        OP_MVI__E, 0xF5, OP_ANA__E,
        OP_MVI__H, 0xEB, OP_ANA__H,
        OP_MVI__L, 0xD7, OP_ANA__L,
        OP_ANI, 0x0F,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ANA are not
    // written to the destination until
    // the T2 state of the next instruction.

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF7, A->value);

    // ANA_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF7, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFE, B->value);

    // ANA_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF6, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFD, C->value);

    // ANA_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF4, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFB, D->value);

    // ANA_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF0, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF5, E->value);

    // ANA_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xF0, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xEB, H->value);

    // ANA_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xE0, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xD7, L->value);

    // ANA_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xC0, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) ANI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
}

static void i8080_alu_post_ora(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_ORA__A,
        OP_MVI__B, 0x01, OP_ORA__B,
        OP_MVI__C, 0x02, OP_ORA__C,
        OP_MVI__D, 0x04, OP_ORA__D,
        OP_MVI__E, 0x10, OP_ORA__E,
        OP_MVI__H, 0x20, OP_ORA__H,
        OP_MVI__L, 0x40, OP_ORA__L,
        OP_ORI, 0x80,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ORA are not
    // written to the destination until
    // the T2 state of the next instruction.

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // ORA_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // ORA_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x09, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // ORA_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x0B, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // ORA_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x0F, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // ORA_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x1F, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // ORA_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x3F, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // ORA_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x7F, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) ORI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xFF, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
}

static void i8080_alu_post_xra(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_XRA__A,
        OP_MVI__B, 0x01, OP_XRA__B,
        OP_MVI__C, 0x02, OP_XRA__C,
        OP_MVI__D, 0x04, OP_XRA__D,
        OP_MVI__E, 0x10, OP_XRA__E,
        OP_MVI__H, 0x20, OP_XRA__H,
        OP_MVI__L, 0x40, OP_XRA__L,
        OP_XRI, 0xCC,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_XRA are not
    // written to the destination until
    // the T2 state of the next instruction.

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // XRA_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // XRA_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // XRA_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x03, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // XRA_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x07, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // XRA_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x17, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // XRA_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x37, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // XRA_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x77, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) XRI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0xBB, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
}

static void i8080_alu_post_cmp(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x08, OP_CMP__A,
        OP_MVI__B, 0x01, OP_CMP__B,
        OP_MVI__C, 0x02, OP_CMP__C,
        OP_MVI__D, 0x04, OP_CMP__D,
        OP_MVI__E, 0x10, OP_CMP__E,
        OP_MVI__H, 0x20, OP_CMP__H,
        OP_MVI__L, 0x40, OP_CMP__L,
        OP_CPI, 0x08,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_CMP are not
    // written to the destination until
    // the T2 state of the next instruction.

    // NOP
    i8080_one_instruction(cpu, 0);

    // MVI_A and imm8 40h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);

    // CMP_A (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) MVI_B and imm8 01h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x01, B->value);

    // CMP_B (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_C and imm8 02h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x02, C->value);

    // CMP_C (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    // (starting with T3) MVI_D and imm8 04h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x04, D->value);

    // CMP_D (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    // (starting with T3) MVI_E and imm8 08h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x10, E->value);

    // CMP_E (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x83, FLAGS->value);

    // (starting with T3) MVI_H and imm8 10h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x20, H->value);

    // CMP_H (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x87, FLAGS->value);

    // (starting with T3) MVI_L and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x40, L->value);

    // CMP_L (through next T2)
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x83, FLAGS->value);

    // (starting with T3) CPI and imm8 20h
    i8080_one_instruction(cpu, 18);     // results appear in T2 of next instruction
    ASSERT_EQ_integer(0x08, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) NOP; NOP; NOP
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
    i8080_one_instruction(cpu, 0);
}

static void i8080_alu_post_daa(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;
    const pData         C = cpu->C;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__A, 0x69,
        OP_MVI__C, 0x58,
        OP_ADD__C,
        OP_DAA,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SUB are not
    // written to the destination until
    // the T2 state of the next instruction.

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // MVI A,0x69
    ASSERT_EQ_integer(0x69, A->value);
    i8080_one_instruction(cpu, 0);      // MVI C,0x58
    ASSERT_EQ_integer(0x58, C->value);
    i8080_one_instruction(cpu, 18);     // ADD C (results in T2 of next instr)
    ASSERT_EQ_integer(0xC1, A->value);
    ASSERT_EQ_integer(0x92, FLAGS->value);      // S,Ac
    i8080_one_instruction(cpu, 0);      // DAA
    ASSERT_EQ_integer(0x27, A->value);
    ASSERT_EQ_integer(0x07, FLAGS->value);      // P,Cy
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

static void i8080_alu_post_rot(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         FLAGS = cpu->FLAGS;

    const pData         A = cpu->A;

    Byte                test_prog[] = {
        OP_XRA__A,
        OP_MVI__A, 0x5F,
        OP_RLC,
        OP_RLC,
        OP_NOP,

        OP_XRA__A,
        OP_MVI__A, 0xFA,
        OP_RRC,
        OP_RRC,
        OP_NOP,

        OP_XRA__A,
        OP_MVI__A, 0x5F,
        OP_RAL,
        OP_RAL,
        OP_RAL,
        OP_NOP,

        OP_XRA__A,
        OP_MVI__A, 0xFA,
        OP_RAR,
        OP_RAR,
        OP_RAR,
        OP_NOP,

        OP_NOP,
        OP_MVI__A, 0x5B,
        OP_CMA,
        OP_CMA,
        OP_NOP,

        OP_XRA__A,
        OP_CMC,
        OP_CMC,
        OP_NOP,

        OP_XRA__A,
        OP_STC,
        OP_STC,
        OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SUB are not
    // written to the destination until
    // the T2 state of the next instruction.

    i8080_one_instruction(cpu, 18);     // XRA A (results in T2 of next instr)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A,0x5F
    ASSERT_EQ_integer(0x5F, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RLC
    ASSERT_EQ_integer(0xBE, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RLC
    ASSERT_EQ_integer(0x7D, A->value);
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A,0xFA
    ASSERT_EQ_integer(0xFA, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RRC
    ASSERT_EQ_integer(0x7D, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RRC
    ASSERT_EQ_integer(0xBE, A->value);
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A,0x5F
    ASSERT_EQ_integer(0x5F, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RAL
    ASSERT_EQ_integer(0xBE, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RAL
    ASSERT_EQ_integer(0x7C, A->value);
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 18);     // RAL
    ASSERT_EQ_integer(0xF9, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A,0xFA
    ASSERT_EQ_integer(0xFA, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RAR
    ASSERT_EQ_integer(0x7D, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 18);     // RAR
    ASSERT_EQ_integer(0x3E, A->value);
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 18);     // RAR
    ASSERT_EQ_integer(0x9F, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 0);      // MVI A,0x5B
    ASSERT_EQ_integer(0x5B, A->value);

    i8080_one_instruction(cpu, 0);      // CMA
    ASSERT_EQ_integer(0xA4, A->value);

    i8080_one_instruction(cpu, 0);      // CMA
    ASSERT_EQ_integer(0x5B, A->value);

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // CMC
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // CMC
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 18);     // XRA A (to get known flag state)
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // STC
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // STC
    ASSERT_EQ_integer(0x47, FLAGS->value);      // Z,P,Cy

    i8080_one_instruction(cpu, 0);      // NOP
}
