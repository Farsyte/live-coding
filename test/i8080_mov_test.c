#include "clock.h"
#include "i8080_impl.h"

// i8080_mov: manage most MOV instructions
//
// This implementation uses one individual T-state management function
// for each source register, and one for each destination; each of the
// instructions will mix-and-match the correct source and destination.

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static Byte         i8080_mov_program[] = {
    OP_NOP,             // give us a moment
    OP_MVI_B, 0x22, OP_MOV_A_B,
    OP_MVI_C, 0x33, OP_MOV_B_C,
    OP_MVI_D, 0x44, OP_MOV_C_D,
    OP_MVI_E, 0x55, OP_MOV_D_E,
    OP_MVI_H, 0x66, OP_MOV_E_H,
    OP_MVI_L, 0x77, OP_MOV_H_L,
    OP_MVI_A, 0x11, OP_MOV_L_A,
    0xFF,               // make this look like uninitialized memory
};

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_mov_post(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;

    ASSERT_EQ_integer(sizeof(ts->rom[0]->cells[0]), sizeof(i8080_mov_program[0]));

    memcpy(ts->rom[0]->cells, i8080_mov_program, sizeof(i8080_mov_program));

    data_z(cpu->A);
    data_z(cpu->B);
    data_z(cpu->C);
    data_z(cpu->D);
    data_z(cpu->E);
    data_z(cpu->H);
    data_z(cpu->L);

    clock_run_until(TAU + 9 * 5);       // the leading NOP

    clock_run_until(TAU + 9 * 15);      // MVI to B, MOV to A
    ASSERT_EQ_integer(0x22, cpu->A->value);
    clock_run_until(TAU + 9 * 15);      // MVI to C, MOV to B
    ASSERT_EQ_integer(0x33, cpu->B->value);
    clock_run_until(TAU + 9 * 15);      // MVI to D, MOV to C
    ASSERT_EQ_integer(0x44, cpu->C->value);
    clock_run_until(TAU + 9 * 15);      // MVI to E, MOV to D
    ASSERT_EQ_integer(0x55, cpu->D->value);
    clock_run_until(TAU + 9 * 15);      // MVI to H, MOV to E
    ASSERT_EQ_integer(0x66, cpu->E->value);
    clock_run_until(TAU + 9 * 15);      // MVI to L, MOV to H
    ASSERT_EQ_integer(0x77, cpu->H->value);
    clock_run_until(TAU + 9 * 15);      // MVI to A, MOV to L
    ASSERT_EQ_integer(0x11, cpu->L->value);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_mov_bist(CpuTestSys ts)
{
    pSigSess            ss = ts->ss;
    SigPlot             sp;
    Tau                 t0;

    p8080               cpu = ts->cpu;

    ASSERT_EQ_integer(sizeof(ts->rom[0]->cells[0]), sizeof(i8080_mov_program[0]));

    memcpy(ts->rom[0]->cells + ts->cpu->PC->value,
           i8080_mov_program, sizeof(i8080_mov_program));

    data_z(cpu->A);
    data_z(cpu->B);
    data_z(cpu->C);
    data_z(cpu->D);
    data_z(cpu->E);
    data_z(cpu->H);
    data_z(cpu->L);

    clock_run_until(TAU + 9 * 5);       // the leading NOP

    t0 = TAU;
    clock_run_until(TAU + 9 * 15);      // MVI to B, MOV to A
    clock_run_until(TAU + 9 * 15);      // MVI to C, MOV to B
    sigplot_init(sp, ss, "i8080_bist_mov_bc", "Intel 8080 Single Chip 8-bit Microprocessor",
                 "MOV test for imm->B->A, imm->C->B", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
    ASSERT_EQ_integer(0x22, cpu->A->value);
    ASSERT_EQ_integer(0x33, cpu->B->value);

    t0 = TAU;
    clock_run_until(TAU + 9 * 15);      // MVI to D, MOV to C
    clock_run_until(TAU + 9 * 15);      // MVI to E, MOV to D
    sigplot_init(sp, ss, "i8080_bist_mov_de", "Intel 8080 Single Ehip 8-bit Microprocessor",
                 "MOV test for imm->D->C, imm->E->D", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
    ASSERT_EQ_integer(0x44, cpu->C->value);
    ASSERT_EQ_integer(0x55, cpu->D->value);

    t0 = TAU;
    clock_run_until(TAU + 9 * 15);      // MVI to H, MOV to E
    clock_run_until(TAU + 9 * 15);      // MVI to L, MOV to H
    sigplot_init(sp, ss, "i8080_bist_mov_hl", "Intel 8080 Single Ehip 8-bit Microprocessor",
                 "MOV test for imm->H->E, imm->L->H", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
    ASSERT_EQ_integer(0x66, cpu->E->value);
    ASSERT_EQ_integer(0x77, cpu->H->value);

    t0 = TAU;
    clock_run_until(TAU + 9 * 15);      // MVI to A, MOV to L
    sigplot_init(sp, ss, "i8080_bist_mov_a", "Intel 8080 Single Ehip 8-bit Microprocessor",
                 "MOV test for imm->A->L", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
    ASSERT_EQ_integer(0x11, cpu->L->value);
}
