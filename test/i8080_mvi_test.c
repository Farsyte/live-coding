#include "clock.h"
#include "i8080_impl.h"

// i8080_mvi: manage most MVI instructions

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static Byte         i8080_mvi_program[] = {
    OP_NOP,             // give us a moment
    OP_MVI_A, 0x11,
    OP_MVI_B, 0x22,
    OP_MVI_C, 0x33,
    OP_MVI_D, 0x44,
    OP_MVI_E, 0x55,
    OP_MVI_H, 0x66,
    OP_MVI_L, 0x77,
    0xFF,               // make this look like uninitialized memory
};

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_mvi_post(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;

    ASSERT_EQ_integer(sizeof(ts->rom[0]->cells[0]), sizeof(i8080_mvi_program[0]));

    memcpy(ts->rom[0]->cells + ts->cpu->PC->value,
           i8080_mvi_program, sizeof(i8080_mvi_program));

    clock_run_until(TAU + 9 * 5);       // NOP

    clock_run_until(TAU + 9 * 9);       // ..., A
    ASSERT_EQ_integer(0x11, cpu->A->value);
    clock_run_until(TAU + 9 * 9);       // ..., B
    ASSERT_EQ_integer(0x22, cpu->B->value);
    clock_run_until(TAU + 9 * 9);       // ..., C
    ASSERT_EQ_integer(0x33, cpu->C->value);

    clock_run_until(TAU + 9 * 9);       // ..., D
    ASSERT_EQ_integer(0x44, cpu->D->value);
    clock_run_until(TAU + 9 * 9);       // ..., E
    ASSERT_EQ_integer(0x55, cpu->E->value);
    clock_run_until(TAU + 9 * 9);       // ..., H
    ASSERT_EQ_integer(0x66, cpu->H->value);
    clock_run_until(TAU + 9 * 9);       // ..., L
    ASSERT_EQ_integer(0x77, cpu->L->value);

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

    memcpy(ts->rom[0]->cells + ts->cpu->PC->value,
           i8080_mvi_program, sizeof(i8080_mvi_program));

    t0 = TAU;
    clock_run_until(TAU + 9 * 5);       // NOP

    clock_run_until(TAU + 9 * 9);       // ..., A
    ASSERT_EQ_integer(0x11, cpu->A->value);
    clock_run_until(TAU + 9 * 9);       // ..., B
    ASSERT_EQ_integer(0x22, cpu->B->value);
    clock_run_until(TAU + 9 * 9);       // ..., C
    ASSERT_EQ_integer(0x33, cpu->C->value);

    sigplot_init(sp, ss, "i8080_bist_mvi_abc", "Intel 8080 Single Chip 8-bit Microprocessor",
                 "MVI test: MVI A, MVI B, MVI C", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    t0 = TAU;
    clock_run_until(TAU + 9 * 9);       // ..., D
    ASSERT_EQ_integer(0x44, cpu->D->value);
    clock_run_until(TAU + 9 * 9);       // ..., E
    ASSERT_EQ_integer(0x55, cpu->E->value);
    clock_run_until(TAU + 9 * 9);       // ..., H
    ASSERT_EQ_integer(0x66, cpu->H->value);
    clock_run_until(TAU + 9 * 9);       // ..., L
    ASSERT_EQ_integer(0x77, cpu->L->value);
    sigplot_init(sp, ss, "i8080_bist_mvi_dehl", "Intel 8080 Single Chip 8-bit Microprocessor",
                 "MVI test: MVI D, MVI E, MVI H, MVI L", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

}
