#include "i8080_test.h"

// i8080_mvi: manage most MVI instructions

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static Byte         i8080_mvi_program[] = {
    OP_NOP,
    OP_MVI__A, 0x11,
    OP_MVI__B, 0x22,
    OP_MVI__C, 0x33,
    OP_MVI__D, 0x44,
    OP_MVI__E, 0x55,
    OP_MVI__H, 0x66,
    OP_MVI__L, 0x77,
    OP_NOP,
};

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_mvi_post(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;

    ASSERT_EQ_integer(sizeof(ts->rom[0]->cells[0]), sizeof(i8080_mvi_program[0]));

    memcpy(ts->rom[0]->cells + cpu->PC->value, i8080_mvi_program, sizeof(i8080_mvi_program));

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 0);      // MVI A, 0x11
    ASSERT_EQ_integer(0x11, cpu->A->value);
    i8080_one_instruction(cpu, 0);      // MVI B, 0x22
    ASSERT_EQ_integer(0x22, cpu->B->value);
    i8080_one_instruction(cpu, 0);      // MVI C, 0x33
    ASSERT_EQ_integer(0x33, cpu->C->value);

    i8080_one_instruction(cpu, 0);      // MVI D, 0x44
    ASSERT_EQ_integer(0x44, cpu->D->value);
    i8080_one_instruction(cpu, 0);      // MVI E, 0x55
    ASSERT_EQ_integer(0x55, cpu->E->value);
    i8080_one_instruction(cpu, 0);      // MVI H, 0x66
    ASSERT_EQ_integer(0x66, cpu->H->value);
    i8080_one_instruction(cpu, 0);      // MVI L, 0x77
    ASSERT_EQ_integer(0x77, cpu->L->value);

    i8080_one_instruction(cpu, 0);      // NOP
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_mvi_bist(CpuTestSys ts)
{
    pSigSess            ss = ts->ss;
    SigPlot             sp;
    Tau                 t0;

    p8080               cpu = ts->cpu;

    ASSERT_EQ_integer(sizeof(ts->rom[0]->cells[0]), sizeof(i8080_mvi_program[0]));

    memcpy(ts->rom[0]->cells + cpu->PC->value, i8080_mvi_program, sizeof(i8080_mvi_program));

    t0 = TAU;
    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 0);      // MVI A, 0x11
    ASSERT_EQ_integer(0x11, cpu->A->value);
    i8080_one_instruction(cpu, 0);      // MVI B, 0x22
    ASSERT_EQ_integer(0x22, cpu->B->value);
    i8080_one_instruction(cpu, 0);      // MVI C, 0x33
    ASSERT_EQ_integer(0x33, cpu->C->value);

    sigplot_init(sp, ss, "i8080_bist_mvi_abc", "Intel 8080 Single Chip 8-bit Microprocessor",
                 "MVI test: MVI A, MVI B, MVI C", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    t0 = TAU;
    i8080_one_instruction(cpu, 0);      // MVI D, 0x44
    ASSERT_EQ_integer(0x44, cpu->D->value);
    i8080_one_instruction(cpu, 0);      // MVI E, 0x55
    ASSERT_EQ_integer(0x55, cpu->E->value);
    i8080_one_instruction(cpu, 0);      // MVI H, 0x66
    ASSERT_EQ_integer(0x66, cpu->H->value);
    i8080_one_instruction(cpu, 0);      // MVI L, 0x77
    ASSERT_EQ_integer(0x77, cpu->L->value);

    sigplot_init(sp, ss, "i8080_bist_mvi_dehl", "Intel 8080 Single Chip 8-bit Microprocessor",
                 "MVI test: MVI D, MVI E, MVI H, MVI L", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    i8080_one_instruction(cpu, 0);      // NOP
}
