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

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_alu_post(CpuTestSys ts)
{
    (void)ts;           // TODO
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_alu_bist(CpuTestSys ts)
{
    i8080_alu_bist_add(ts);
    i8080_alu_bist_sub(ts);
    i8080_alu_bist_adc(ts);
    i8080_alu_bist_sbb(ts);

    // TODO check ANA, XRA, ORA
    // TODO test CMP
}

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
        OP_MVI_A, 0x40, OP_ADD_A,
        OP_MVI_B, 0x01, OP_ADD_B,
        OP_MVI_C, 0x02, OP_ADD_C,
        OP_MVI_D, 0x04, OP_ADD_D,
        OP_MVI_E, 0x08, OP_ADD_E,
        OP_MVI_H, 0x10, OP_ADD_H,
        OP_MVI_L, 0x20, OP_ADD_L,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + ts->cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    clock_run_until(TAU + 9 * 5);

    // MVI_A and imm8 40h
    clock_run_until(TAU + 9 * 9);
    ASSERT_EQ_integer(0x40, A->value);

    // ADD_A (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x80, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_B and imm8 01h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x01, B->value);

    // ADD_B (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x81, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_C and imm8 02h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x02, C->value);

    // ADD_C (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x83, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_D and imm8 04h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x04, D->value);

    // ADD_D (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x87, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_E and imm8 08h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x08, E->value);

    // ADD_E (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x8F, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_H and imm8 10h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x10, H->value);

    // ADD_H (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x9F, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_L and imm8 20h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x20, L->value);

    // ADD_L (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xBF, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) NOP; NOP; NOP
    clock_run_until(TAU + 9 * 13);

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
        OP_MVI_A, 0x40, OP_SUB_A,
        OP_MVI_B, 0x01, OP_SUB_B,
        OP_MVI_C, 0x02, OP_SUB_C,
        OP_MVI_D, 0x04, OP_SUB_D,
        OP_MVI_E, 0x08, OP_SUB_E,
        OP_MVI_H, 0x10, OP_SUB_H,
        OP_MVI_L, 0x20, OP_SUB_L,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + ts->cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SUB are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    clock_run_until(TAU + 9 * 5);

    // MVI_A and imm8 40h
    clock_run_until(TAU + 9 * 9);
    ASSERT_EQ_integer(0x40, A->value);

    // SUB_A (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x00, A->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    // (starting with T3) MVI_B and imm8 01h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x01, B->value);

    // SUB_B (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xFF, A->value);
    ASSERT_EQ_integer(0x97, FLAGS->value);      // S,Ac,P,Cy

    // (starting with T3) MVI_C and imm8 02h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x02, C->value);

    // SUB_C (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xFD, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_D and imm8 04h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x04, D->value);

    // SUB_D (through next T2)
    clock_run_until(TAU + 9 * 7);

    // (starting with T3) MVI_E and imm8 08h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x08, E->value);

    // SUB_E (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xF1, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_H and imm8 10h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x10, H->value);

    // SUB_H (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xE1, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_L and imm8 20h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x20, L->value);

    // SUB_L (through next T2)
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xC1, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) NOP; NOP; NOP
    clock_run_until(TAU + 9 * 13);

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
        OP_MVI_A, 0x40, OP_ADC_A,
        OP_MVI_B, 0x01, OP_ADC_B,
        OP_MVI_C, 0x02, OP_ADC_C,
        OP_MVI_D, 0x04, OP_ADC_D,
        OP_MVI_E, 0x08, OP_ADC_E,
        OP_MVI_H, 0x10, OP_ADC_H,
        OP_MVI_L, 0x20, OP_ADC_L,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + ts->cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADC are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    clock_run_until(TAU + 9 * 5);

    // MVI_A and imm8 40h
    clock_run_until(TAU + 9 * 9);
    ASSERT_EQ_integer(0x40, A->value);

    // ADC_A (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x81, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_B and imm8 01h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x01, B->value);

    // ADC_B (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x83, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_C and imm8 02h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x02, C->value);

    // ADC_C (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x86, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_D and imm8 04h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x04, D->value);

    // ADC_D (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x8B, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_E and imm8 08h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x08, E->value);

    // ADC_E (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x94, A->value);
    ASSERT_EQ_integer(0x92, FLAGS->value);      // S,Ac

    // (starting with T3) MVI_H and imm8 10h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x10, H->value);

    // ADC_H (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xA5, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_L and imm8 20h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x20, L->value);

    // ADC_L (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the carry bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xC6, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) NOP; NOP; NOP
    clock_run_until(TAU + 9 * 13);

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
        OP_MVI_A, 0x40, OP_SBB_A,
        OP_MVI_B, 0x01, OP_SBB_B,
        OP_MVI_C, 0x02, OP_SBB_C,
        OP_MVI_D, 0x04, OP_SBB_D,
        OP_MVI_E, 0x08, OP_SBB_E,
        OP_MVI_H, 0x10, OP_SBB_H,
        OP_MVI_L, 0x20, OP_SBB_L,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + ts->cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_SBB are not
    // written to the destination until
    // the T2 state of the next instruction.

    t0 = TAU;

    // NOP
    clock_run_until(TAU + 9 * 5);

    // MVI_A and imm8 40h
    clock_run_until(TAU + 9 * 9);
    ASSERT_EQ_integer(0x40, A->value);

    // SBB_A (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xFF, A->value);
    ASSERT_EQ_integer(0x97, FLAGS->value);      // S,Ac,P,Cy

    // (starting with T3) MVI_B and imm8 01h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x01, B->value);

    // SBB_B (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xFD, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) MVI_C and imm8 02h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x02, C->value);

    // SBB_C (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xFA, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_D and imm8 04h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x04, D->value);

    // SBB_D (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xF5, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_E and imm8 08h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x08, E->value);

    // SBB_E (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xEC, A->value);
    ASSERT_EQ_integer(0x92, FLAGS->value);      // S,Ac

    // (starting with T3) MVI_H and imm8 10h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x10, H->value);

    // SBB_H (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xDB, A->value);
    ASSERT_EQ_integer(0x86, FLAGS->value);      // S,P

    // (starting with T3) MVI_L and imm8 20h
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0x20, L->value);

    // SBB_L (through next T2)
    cpu->FLAGS->value |= FLAGS_CY;      // fake the borrow bit on for the test
    clock_run_until(TAU + 9 * 7);
    ASSERT_EQ_integer(0xBA, A->value);
    ASSERT_EQ_integer(0x82, FLAGS->value);      // S

    // (starting with T3) NOP; NOP; NOP
    clock_run_until(TAU + 9 * 13);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_alu_sbb",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "SBB from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
}
