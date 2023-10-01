#include "clock.h"
#include "i8080_impl.h"

// i8080_lxi_test: test the LXI instruction

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_lxi_post(CpuTestSys ts)
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

    // REMINDER: operands for LXI are low byte first.

    Byte                test_prog[] = {
        OP_NOP,
        OP_LXI__BC, 0x22, 0x11,
        OP_LXI__DE, 0x44, 0x33,
        OP_LXI__HL, 0x66, 0x55,
        OP_LXI__SP, 0x88, 0x77,
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
    addr_z(SP);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // LXI__BC, 0x1122
    ASSERT_EQ_integer(0x11, cpu->B->value);
    ASSERT_EQ_integer(0x22, cpu->C->value);
    i8080_one_instruction(cpu, 0);      // LXI__DE, 0x3344
    ASSERT_EQ_integer(0x33, cpu->D->value);
    ASSERT_EQ_integer(0x44, cpu->E->value);
    i8080_one_instruction(cpu, 0);      // LXI__HL, 0x5566
    ASSERT_EQ_integer(0x55, cpu->H->value);
    ASSERT_EQ_integer(0x66, cpu->L->value);
    i8080_one_instruction(cpu, 0);      // LXI__SP, 0x7788
    ASSERT_EQ_integer(0x7788, cpu->SP->value);
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_lxi_bist(CpuTestSys ts)
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

    // REMINDER: operands for LXI are low byte first.

    Byte                test_prog[] = {
        OP_NOP,
        OP_LXI__BC, 0x22, 0x11,
        OP_LXI__DE, 0x44, 0x33,
        OP_LXI__HL, 0x66, 0x55,
        OP_LXI__SP, 0x88, 0x77,
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
    addr_z(SP);

    t0 = TAU;

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // LXI__BC, 0x1122
    ASSERT_EQ_integer(0x11, cpu->B->value);
    ASSERT_EQ_integer(0x22, cpu->C->value);
    i8080_one_instruction(cpu, 0);      // LXI__DE, 0x3344
    ASSERT_EQ_integer(0x33, cpu->D->value);
    ASSERT_EQ_integer(0x44, cpu->E->value);
    i8080_one_instruction(cpu, 0);      // LXI__HL, 0x5566
    ASSERT_EQ_integer(0x55, cpu->H->value);
    ASSERT_EQ_integer(0x66, cpu->L->value);
    i8080_one_instruction(cpu, 0);      // LXI__SP, 0x7788
    ASSERT_EQ_integer(0x7788, cpu->SP->value);
    i8080_one_instruction(cpu, 0);      // NOP

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_lxi",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "LXI instructions", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}
