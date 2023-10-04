#include "i8080_test.h"

// i8080_misc: test misc instructions

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_misc_post(CpuTestSys ts)
{
    (void)ts;
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_misc_bist(CpuTestSys ts)
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
    SigPlot             sigp;

    Byte                test_prog[] = {
        OP_NOP,
        OP_MVI__H, 0x29,
        OP_MVI__L, 0x00,
        OP_SPHL,

        OP_NOP,
        OP_MVI__D, 0x34,
        OP_MVI__E, 0x80,
        OP_XCHG,

        OP_NOP,
        OP_MVI__H, 0x01,
        OP_MVI__L, 0x03,
        OP_PCHL,

        OP_NOP, OP_NOP, OP_NOP,
    };

    Byte                test_prog_0100[] = {
        OP_NOP, OP_NOP, OP_NOP,
        OP_NOP, OP_NOP, OP_NOP,

        OP_NOP, OP_NOP, OP_NOP,
    };

    pByte               into = cells + cpu->PC->value;

    memcpy(into, test_prog, sizeof(test_prog));
    memcpy(cells + 0x0100, test_prog_0100, sizeof(test_prog_0100));

    data_z(B);
    data_z(C);
    data_z(D);
    data_z(E);
    data_z(H);
    data_z(L);

    addr_z(SP);

    // test the SPHL instruction
    // - force initial state into H and L

    t0 = TAU;

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // MVI H,0x29
    ASSERT_EQ_integer(0x29, cpu->H->value);
    i8080_one_instruction(cpu, 0);      // MVI L,0x00
    ASSERT_EQ_integer(0x00, cpu->L->value);
    i8080_one_instruction(cpu, 0);      // SPHL
    ASSERT_EQ_integer(0x2900, cpu->SP->value);

    sigplot_init(sigp, ss, "i8080_bist_sphl",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "MVI, MVI, SPHL", t0, TAU - t0);
    i8080_plot_sigs(sigp);
    sigplot_fini(sigp);

    t0 = TAU;

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // MVI D,0x34
    ASSERT_EQ_integer(0x34, cpu->D->value);
    i8080_one_instruction(cpu, 0);      // MVI E,0x80
    ASSERT_EQ_integer(0x80, cpu->E->value);
    i8080_one_instruction(cpu, 0);      // XCHG

    ASSERT_EQ_integer(0x34, cpu->H->value);
    ASSERT_EQ_integer(0x80, cpu->L->value);
    ASSERT_EQ_integer(0x29, cpu->D->value);
    ASSERT_EQ_integer(0x00, cpu->E->value);

    sigplot_init(sigp, ss, "i8080_bist_xchg",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "MVI, MVI, XCHG", t0, TAU - t0);
    i8080_plot_sigs(sigp);
    sigplot_fini(sigp);

    t0 = TAU;

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // MVI H,0x01
    ASSERT_EQ_integer(0x01, cpu->H->value);
    i8080_one_instruction(cpu, 0);      // MVI L,0x03
    ASSERT_EQ_integer(0x03, cpu->L->value);
    i8080_one_instruction(cpu, 0);      // PCHL
    ASSERT_EQ_integer(0x0103, cpu->PC->value);
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP

    sigplot_init(sigp, ss, "i8080_bist_pchl",
                 "Intel 8080 Single Chip 8-bit Microprocessor", "MVI, MVI, PCHL", t0, TAU - t0);
    i8080_plot_sigs(sigp);
    sigplot_fini(sigp);

}
