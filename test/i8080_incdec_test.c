#include "clock.h"
#include "i8080_impl.h"

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         i8080_incdec_bist_inr(CpuTestSys ts);
static void         i8080_incdec_bist_dcr(CpuTestSys ts);
static void         i8080_incdec_bist_inx(CpuTestSys ts);
static void         i8080_incdec_bist_dcx(CpuTestSys ts);

static void         i8080_incdec_post_inr(CpuTestSys ts);
static void         i8080_incdec_post_dcr(CpuTestSys ts);
static void         i8080_incdec_post_inx(CpuTestSys ts);
static void         i8080_incdec_post_dcx(CpuTestSys ts);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_incdec_post(CpuTestSys ts)
{
    i8080_reset_for_testing(ts);
    i8080_incdec_post_inr(ts);

    i8080_reset_for_testing(ts);
    i8080_incdec_post_dcr(ts);

    i8080_reset_for_testing(ts);
    i8080_incdec_post_inx(ts);

    i8080_reset_for_testing(ts);
    i8080_incdec_post_dcx(ts);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_incdec_bist(CpuTestSys ts)
{
    i8080_reset_for_testing(ts);
    i8080_incdec_bist_inr(ts);

    i8080_reset_for_testing(ts);
    i8080_incdec_bist_dcr(ts);

    i8080_reset_for_testing(ts);
    i8080_incdec_bist_inx(ts);

    i8080_reset_for_testing(ts);
    i8080_incdec_bist_dcx(ts);
}

// === === === === === === === === === === === === === === === ===
//                    BUILT-IN SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static void i8080_incdec_bist_inr(CpuTestSys ts)
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
        OP_MVI__A, 0x55, OP_XRA__A, OP_NOP,
        OP_MVI__A, 0x08, OP_INR__A,
        OP_MVI__B, 0x01, OP_INR__B,
        OP_MVI__C, 0x02, OP_INR__C,
        OP_MVI__D, 0x08, OP_INR__D,
        OP_MVI__E, 0x7F, OP_INR__E,
        OP_MVI__H, 0xBF, OP_INR__H,
        OP_MVI__L, 0xFF, OP_INR__L,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    data_z(A);
    data_z(B);
    data_z(C);
    data_z(D);
    data_z(E);
    data_z(H);
    data_z(L);

    i8080_one_instruction(cpu, 0);      // MVI A, 0x55
    i8080_one_instruction(cpu, 0);      // XRA A
    i8080_one_instruction(cpu, 0);      // NOP
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    t0 = TAU;

    i8080_one_instruction(cpu, 0);      // MVI A, 0x08
    i8080_one_instruction(cpu, 0);      // INR A
    ASSERT_EQ_integer(0x09, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    i8080_one_instruction(cpu, 0);      // MVI B, 0x01
    i8080_one_instruction(cpu, 0);      // INR B
    ASSERT_EQ_integer(0x02, B->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    i8080_one_instruction(cpu, 0);      // MVI C, 0x02
    i8080_one_instruction(cpu, 0);      // INR C
    ASSERT_EQ_integer(0x03, C->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    i8080_one_instruction(cpu, 0);      // MVI D, 0x08
    i8080_one_instruction(cpu, 0);      // INR D
    ASSERT_EQ_integer(0x09, D->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    i8080_one_instruction(cpu, 0);      // MVI E, 0x7F
    i8080_one_instruction(cpu, 0);      // INR E
    ASSERT_EQ_integer(0x80, E->value);
    ASSERT_EQ_integer(0x92, FLAGS->value);      // S,Ac

    i8080_one_instruction(cpu, 0);      // MVI H, 0xBF
    i8080_one_instruction(cpu, 0);      // INR H,
    ASSERT_EQ_integer(0xC0, H->value);
    ASSERT_EQ_integer(0x96, FLAGS->value);      // S,Ac,P

    i8080_one_instruction(cpu, 0);      // MVI L, 0xFF
    i8080_one_instruction(cpu, 0);      // INR L
    ASSERT_EQ_integer(0x00, L->value);
    ASSERT_EQ_integer(0x16, FLAGS->value);      // Ac,P

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_bist_alu_inr",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "INR from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

static void i8080_incdec_bist_dcr(CpuTestSys ts)
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
        OP_MVI__A, 0x55, OP_XRA__A, OP_NOP,
        OP_MVI__A, 0x08, OP_DCR__A,
        OP_MVI__B, 0x01, OP_DCR__B,
        OP_MVI__C, 0x02, OP_DCR__C,
        OP_MVI__D, 0x08, OP_DCR__D,
        OP_MVI__E, 0x80, OP_DCR__E,
        OP_MVI__H, 0xC0, OP_DCR__H,
        OP_MVI__L, 0x00, OP_DCR__L,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    data_z(A);
    data_z(B);
    data_z(C);
    data_z(D);
    data_z(E);
    data_z(H);
    data_z(L);

    // when counting states, remember:
    // ROM fetches add one TW state.

    i8080_one_instruction(cpu, 0);      // MVI A, 0x55
    i8080_one_instruction(cpu, 0);      // XRA A
    i8080_one_instruction(cpu, 0);      // NOP
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    t0 = TAU;

    i8080_one_instruction(cpu, 0);      // MVI A, 0x08
    i8080_one_instruction(cpu, 0);      // DCR A
    ASSERT_EQ_integer(0x07, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    i8080_one_instruction(cpu, 0);      // MVI B, 0x01
    i8080_one_instruction(cpu, 0);      // DCR B
    ASSERT_EQ_integer(0x00, B->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI C, 0x02
    i8080_one_instruction(cpu, 0);      // DCR C
    ASSERT_EQ_integer(0x01, C->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    i8080_one_instruction(cpu, 0);      // MVI D, 0x08
    i8080_one_instruction(cpu, 0);      // DCR D
    ASSERT_EQ_integer(0x07, D->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    i8080_one_instruction(cpu, 0);      // MVI E, 0x80
    i8080_one_instruction(cpu, 0);      // DCR E
    ASSERT_EQ_integer(0x7F, E->value);
    ASSERT_EQ_integer(0x12, FLAGS->value);      // Ac

    i8080_one_instruction(cpu, 0);      // MVI H, 0xC0
    i8080_one_instruction(cpu, 0);      // DCR H
    ASSERT_EQ_integer(0xBF, H->value);
    ASSERT_EQ_integer(0x92, FLAGS->value);      // S,Ac

    i8080_one_instruction(cpu, 0);      // MVI L, 0x00
    i8080_one_instruction(cpu, 0);      // DCR L
    ASSERT_EQ_integer(0xFF, L->value);
    ASSERT_EQ_integer(0x96, FLAGS->value);      // S,Ac,P

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_bist_alu_dcr",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "DCR from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

static void i8080_incdec_bist_inx(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    const pAddr         SP = cpu->SP;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_INX__SP, OP_INX__SP,
        OP_INX__BC, OP_INX__BC,
        OP_INX__DE, OP_INX__DE,
        OP_INX__HL, OP_INX__HL,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    data_z(B);
    data_z(C);
    data_z(D);
    data_z(E);
    data_z(H);
    data_z(L);

    // initial conditions to save time.
    addr_to(SP, 0xAAFE);
    data_to(B, 0xAA);
    data_to(C, 0xFE);
    data_to(D, 0xAA);
    data_to(E, 0xFE);
    data_to(H, 0xAA);
    data_to(L, 0xFE);

    // when counting states, remember:
    // ROM fetches add one TW state.

    t0 = TAU;

    i8080_one_instruction(cpu, 0);      // INX SP
    ASSERT_EQ_integer(0xAAFF, SP->value);

    i8080_one_instruction(cpu, 0);      // INX SP
    ASSERT_EQ_integer(0xAB00, SP->value);

    i8080_one_instruction(cpu, 0);      // INX BC
    ASSERT_EQ_integer(0xAA, B->value);
    ASSERT_EQ_integer(0xFF, C->value);

    i8080_one_instruction(cpu, 0);      // INX BC
    ASSERT_EQ_integer(0xAB, B->value);
    ASSERT_EQ_integer(0x00, C->value);

    i8080_one_instruction(cpu, 0);      // INX DE
    ASSERT_EQ_integer(0xAA, D->value);
    ASSERT_EQ_integer(0xFF, E->value);

    i8080_one_instruction(cpu, 0);      // INX DE
    ASSERT_EQ_integer(0xAB, D->value);
    ASSERT_EQ_integer(0x00, E->value);

    i8080_one_instruction(cpu, 0);      // INX HL
    ASSERT_EQ_integer(0xAA, H->value);
    ASSERT_EQ_integer(0xFF, L->value);

    i8080_one_instruction(cpu, 0);      // INX HL
    ASSERT_EQ_integer(0xAB, H->value);
    ASSERT_EQ_integer(0x00, L->value);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_bist_alu_inx",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "INX from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

static void i8080_incdec_bist_dcx(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    const pAddr         SP = cpu->SP;

    pSigSess            ss = ts->ss;

    Byte                test_prog[] = {
        OP_DCX__SP, OP_DCX__SP,
        OP_DCX__BC, OP_DCX__BC,
        OP_DCX__DE, OP_DCX__DE,
        OP_DCX__HL, OP_DCX__HL,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    data_z(B);
    data_z(C);
    data_z(D);
    data_z(E);
    data_z(H);
    data_z(L);

    // initial conditions to save time.
    addr_to(SP, 0xAB01);
    data_to(B, 0xAB);
    data_to(C, 0x01);
    data_to(D, 0xAB);
    data_to(E, 0x01);
    data_to(H, 0xAB);
    data_to(L, 0x01);

    // when counting states, remember:
    // ROM fetches add one TW state.

    t0 = TAU;

    i8080_one_instruction(cpu, 0);      // DCX SP
    ASSERT_EQ_integer(0xAB00, SP->value);

    i8080_one_instruction(cpu, 0);      // DCX SP
    ASSERT_EQ_integer(0xAAFF, SP->value);

    i8080_one_instruction(cpu, 0);      // DCX BC
    ASSERT_EQ_integer(0xAB, B->value);
    ASSERT_EQ_integer(0x00, C->value);

    i8080_one_instruction(cpu, 0);      // DCX BC
    ASSERT_EQ_integer(0xAA, B->value);
    ASSERT_EQ_integer(0xFF, C->value);

    i8080_one_instruction(cpu, 0);      // DCX DE
    ASSERT_EQ_integer(0xAB, D->value);
    ASSERT_EQ_integer(0x00, E->value);

    i8080_one_instruction(cpu, 0);      // DCX DE
    ASSERT_EQ_integer(0xAA, D->value);
    ASSERT_EQ_integer(0xFF, E->value);

    i8080_one_instruction(cpu, 0);      // DCX HL
    ASSERT_EQ_integer(0xAB, H->value);
    ASSERT_EQ_integer(0x00, L->value);

    i8080_one_instruction(cpu, 0);      // DCX HL
    ASSERT_EQ_integer(0xAA, H->value);
    ASSERT_EQ_integer(0xFF, L->value);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_bist_alu_dcx",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "DCX from each register", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static void i8080_incdec_post_inr(CpuTestSys ts)
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
        OP_MVI__A, 0x55, OP_XRA__A, OP_NOP,
        OP_MVI__A, 0x08, OP_INR__A,
        OP_MVI__B, 0x01, OP_INR__B,
        OP_MVI__C, 0x02, OP_INR__C,
        OP_MVI__D, 0x08, OP_INR__D,
        OP_MVI__E, 0x7F, OP_INR__E,
        OP_MVI__H, 0xBF, OP_INR__H,
        OP_MVI__L, 0xFF, OP_INR__L,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    data_z(A);
    data_z(B);
    data_z(C);
    data_z(D);
    data_z(E);
    data_z(H);
    data_z(L);

    i8080_one_instruction(cpu, 0);      // MVI A, 0x55
    i8080_one_instruction(cpu, 0);      // XRA A
    i8080_one_instruction(cpu, 0);      // NOP
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A, 0x08
    i8080_one_instruction(cpu, 0);      // INR A
    ASSERT_EQ_integer(0x09, A->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    i8080_one_instruction(cpu, 0);      // MVI B, 0x01
    i8080_one_instruction(cpu, 0);      // INR B
    ASSERT_EQ_integer(0x02, B->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    i8080_one_instruction(cpu, 0);      // MVI C, 0x02
    i8080_one_instruction(cpu, 0);      // INR C
    ASSERT_EQ_integer(0x03, C->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    i8080_one_instruction(cpu, 0);      // MVI D, 0x08
    i8080_one_instruction(cpu, 0);      // INR D
    ASSERT_EQ_integer(0x09, D->value);
    ASSERT_EQ_integer(0x06, FLAGS->value);      // P

    i8080_one_instruction(cpu, 0);      // MVI E, 0x7F
    i8080_one_instruction(cpu, 0);      // INR E
    ASSERT_EQ_integer(0x80, E->value);
    ASSERT_EQ_integer(0x92, FLAGS->value);      // S,Ac

    i8080_one_instruction(cpu, 0);      // MVI H, 0xBF
    i8080_one_instruction(cpu, 0);      // INR H,
    ASSERT_EQ_integer(0xC0, H->value);
    ASSERT_EQ_integer(0x96, FLAGS->value);      // S,Ac,P

    i8080_one_instruction(cpu, 0);      // MVI L, 0xFF
    i8080_one_instruction(cpu, 0);      // INR L
    ASSERT_EQ_integer(0x00, L->value);
    ASSERT_EQ_integer(0x16, FLAGS->value);      // Ac,P

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

static void i8080_incdec_post_dcr(CpuTestSys ts)
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
        OP_MVI__A, 0x55, OP_XRA__A, OP_NOP,
        OP_MVI__A, 0x08, OP_DCR__A,
        OP_MVI__B, 0x01, OP_DCR__B,
        OP_MVI__C, 0x02, OP_DCR__C,
        OP_MVI__D, 0x08, OP_DCR__D,
        OP_MVI__E, 0x80, OP_DCR__E,
        OP_MVI__H, 0xC0, OP_DCR__H,
        OP_MVI__L, 0x00, OP_DCR__L,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    data_z(A);
    data_z(B);
    data_z(C);
    data_z(D);
    data_z(E);
    data_z(H);
    data_z(L);

    // when counting states, remember:
    // ROM fetches add one TW state.

    i8080_one_instruction(cpu, 0);      // MVI A, 0x55
    i8080_one_instruction(cpu, 0);      // XRA A
    i8080_one_instruction(cpu, 0);      // NOP
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI A, 0x08
    i8080_one_instruction(cpu, 0);      // DCR A
    ASSERT_EQ_integer(0x07, A->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    i8080_one_instruction(cpu, 0);      // MVI B, 0x01
    i8080_one_instruction(cpu, 0);      // DCR B
    ASSERT_EQ_integer(0x00, B->value);
    ASSERT_EQ_integer(0x46, FLAGS->value);      // Z,P

    i8080_one_instruction(cpu, 0);      // MVI C, 0x02
    i8080_one_instruction(cpu, 0);      // DCR C
    ASSERT_EQ_integer(0x01, C->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    i8080_one_instruction(cpu, 0);      // MVI D, 0x08
    i8080_one_instruction(cpu, 0);      // DCR D
    ASSERT_EQ_integer(0x07, D->value);
    ASSERT_EQ_integer(0x02, FLAGS->value);      // no flags set

    i8080_one_instruction(cpu, 0);      // MVI E, 0x80
    i8080_one_instruction(cpu, 0);      // DCR E
    ASSERT_EQ_integer(0x7F, E->value);
    ASSERT_EQ_integer(0x12, FLAGS->value);      // Ac

    i8080_one_instruction(cpu, 0);      // MVI H, 0xC0
    i8080_one_instruction(cpu, 0);      // DCR H
    ASSERT_EQ_integer(0xBF, H->value);
    ASSERT_EQ_integer(0x92, FLAGS->value);      // S,Ac

    i8080_one_instruction(cpu, 0);      // MVI L, 0x00
    i8080_one_instruction(cpu, 0);      // DCR L
    ASSERT_EQ_integer(0xFF, L->value);
    ASSERT_EQ_integer(0x96, FLAGS->value);      // S,Ac,P

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

static void i8080_incdec_post_inx(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    const pAddr         SP = cpu->SP;

    Byte                test_prog[] = {
        OP_INX__SP, OP_INX__SP,
        OP_INX__BC, OP_INX__BC,
        OP_INX__DE, OP_INX__DE,
        OP_INX__HL, OP_INX__HL,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    data_z(B);
    data_z(C);
    data_z(D);
    data_z(E);
    data_z(H);
    data_z(L);

    // initial conditions to save time.
    addr_to(SP, 0xAAFE);
    data_to(B, 0xAA);
    data_to(C, 0xFE);
    data_to(D, 0xAA);
    data_to(E, 0xFE);
    data_to(H, 0xAA);
    data_to(L, 0xFE);

    // when counting states, remember:
    // ROM fetches add one TW state.

    i8080_one_instruction(cpu, 0);      // INX SP
    ASSERT_EQ_integer(0xAAFF, SP->value);

    i8080_one_instruction(cpu, 0);      // INX SP
    ASSERT_EQ_integer(0xAB00, SP->value);

    i8080_one_instruction(cpu, 0);      // INX BC
    ASSERT_EQ_integer(0xAA, B->value);
    ASSERT_EQ_integer(0xFF, C->value);

    i8080_one_instruction(cpu, 0);      // INX BC
    ASSERT_EQ_integer(0xAB, B->value);
    ASSERT_EQ_integer(0x00, C->value);

    i8080_one_instruction(cpu, 0);      // INX DE
    ASSERT_EQ_integer(0xAA, D->value);
    ASSERT_EQ_integer(0xFF, E->value);

    i8080_one_instruction(cpu, 0);      // INX DE
    ASSERT_EQ_integer(0xAB, D->value);
    ASSERT_EQ_integer(0x00, E->value);

    i8080_one_instruction(cpu, 0);      // INX HL
    ASSERT_EQ_integer(0xAA, H->value);
    ASSERT_EQ_integer(0xFF, L->value);

    i8080_one_instruction(cpu, 0);      // INX HL
    ASSERT_EQ_integer(0xAB, H->value);
    ASSERT_EQ_integer(0x00, L->value);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

static void i8080_incdec_post_dcx(CpuTestSys ts)
{

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    pByte               cells = rom[0]->cells;

    const pData         B = cpu->B;
    const pData         C = cpu->C;
    const pData         D = cpu->D;
    const pData         E = cpu->E;
    const pData         H = cpu->H;
    const pData         L = cpu->L;

    const pAddr         SP = cpu->SP;

    Byte                test_prog[] = {
        OP_DCX__SP, OP_DCX__SP,
        OP_DCX__BC, OP_DCX__BC,
        OP_DCX__DE, OP_DCX__DE,
        OP_DCX__HL, OP_DCX__HL,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    // test timing is a bit odd,
    // as the results of the OP_ADD are not
    // written to the destination until
    // the T2 state of the next instruction.

    data_z(B);
    data_z(C);
    data_z(D);
    data_z(E);
    data_z(H);
    data_z(L);

    // initial conditions to save time.
    addr_to(SP, 0xAB01);
    data_to(B, 0xAB);
    data_to(C, 0x01);
    data_to(D, 0xAB);
    data_to(E, 0x01);
    data_to(H, 0xAB);
    data_to(L, 0x01);

    // when counting states, remember:
    // ROM fetches add one TW state.

    i8080_one_instruction(cpu, 0);      // DCX SP
    ASSERT_EQ_integer(0xAB00, SP->value);

    i8080_one_instruction(cpu, 0);      // DCX SP
    ASSERT_EQ_integer(0xAAFF, SP->value);

    i8080_one_instruction(cpu, 0);      // DCX BC
    ASSERT_EQ_integer(0xAB, B->value);
    ASSERT_EQ_integer(0x00, C->value);

    i8080_one_instruction(cpu, 0);      // DCX BC
    ASSERT_EQ_integer(0xAA, B->value);
    ASSERT_EQ_integer(0xFF, C->value);

    i8080_one_instruction(cpu, 0);      // DCX DE
    ASSERT_EQ_integer(0xAB, D->value);
    ASSERT_EQ_integer(0x00, E->value);

    i8080_one_instruction(cpu, 0);      // DCX DE
    ASSERT_EQ_integer(0xAA, D->value);
    ASSERT_EQ_integer(0xFF, E->value);

    i8080_one_instruction(cpu, 0);      // DCX HL
    ASSERT_EQ_integer(0xAB, H->value);
    ASSERT_EQ_integer(0x00, L->value);

    i8080_one_instruction(cpu, 0);      // DCX HL
    ASSERT_EQ_integer(0xAA, H->value);
    ASSERT_EQ_integer(0xFF, L->value);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}
