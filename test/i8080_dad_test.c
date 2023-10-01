#include "clock.h"
#include "i8080_impl.h"

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_dad_post(CpuTestSys ts)
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

    // REMINDER: operands for DAD are low byte first.

    Byte                test_prog[] = {
        OP_XRA__A,
        OP_DAD__BC,
        OP_DAD__DE,
        OP_DAD__HL,
        OP_DAD__SP,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    data_to(B, 0x10);
    data_to(C, 0x80);   // BC is 0x1008
    data_to(D, 0x20);
    data_to(E, 0x10);   // DE is 0x2010
    data_to(H, 0x40);
    data_to(L, 0x20);   // HL is 0x4020
    addr_to(SP, 0x8080);

    i8080_one_instruction(cpu, 18);     // XRA__A
    ASSERT_EQ_integer(0x00, cpu->A->value);
    ASSERT_EQ_integer(0x40, cpu->H->value);
    ASSERT_EQ_integer(0x20, cpu->L->value);
    ASSERT_EQ_integer(0x46, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // DAD__BC
    ASSERT_EQ_integer(0x50, cpu->H->value);
    ASSERT_EQ_integer(0xA0, cpu->L->value);
    ASSERT_EQ_integer(0x46, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // DAD__DE
    ASSERT_EQ_integer(0x70, cpu->H->value);
    ASSERT_EQ_integer(0xB0, cpu->L->value);
    ASSERT_EQ_integer(0x46, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // DAD__HL
    ASSERT_EQ_integer(0xE1, cpu->H->value);
    ASSERT_EQ_integer(0x60, cpu->L->value);
    ASSERT_EQ_integer(0x46, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // DAD__SP
    ASSERT_EQ_integer(0x61, cpu->H->value);
    ASSERT_EQ_integer(0xE0, cpu->L->value);
    ASSERT_EQ_integer(0x47, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_dad_bist(CpuTestSys ts)
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

    // REMINDER: operands for DAD are low byte first.

    Byte                test_prog[] = {
        OP_XRA__A,
        OP_DAD__BC,
        OP_DAD__DE,
        OP_DAD__HL,
        OP_DAD__SP,
        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));

    data_to(B, 0x10);
    data_to(C, 0x80);   // BC is 0x1008
    data_to(D, 0x20);
    data_to(E, 0x10);   // DE is 0x2010
    data_to(H, 0x40);
    data_to(L, 0x20);   // HL is 0x4020
    addr_to(SP, 0x8080);

    t0 = TAU;

    i8080_one_instruction(cpu, 18);     // XRA__A
    ASSERT_EQ_integer(0x00, cpu->A->value);
    ASSERT_EQ_integer(0x40, cpu->H->value);
    ASSERT_EQ_integer(0x20, cpu->L->value);
    ASSERT_EQ_integer(0x46, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // DAD__BC
    ASSERT_EQ_integer(0x50, cpu->H->value);
    ASSERT_EQ_integer(0xA0, cpu->L->value);
    ASSERT_EQ_integer(0x46, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // DAD__DE
    ASSERT_EQ_integer(0x70, cpu->H->value);
    ASSERT_EQ_integer(0xB0, cpu->L->value);
    ASSERT_EQ_integer(0x46, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // DAD__HL
    ASSERT_EQ_integer(0xE1, cpu->H->value);
    ASSERT_EQ_integer(0x60, cpu->L->value);
    ASSERT_EQ_integer(0x46, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // DAD__SP
    ASSERT_EQ_integer(0x61, cpu->H->value);
    ASSERT_EQ_integer(0xE0, cpu->L->value);
    ASSERT_EQ_integer(0x47, cpu->FLAGS->value);

    i8080_one_instruction(cpu, 0);      // NOP

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_dad",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "DAD instructions", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}
