#include "chip/i8080_test.h"

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
    OP_MVI__B, 0x22, OP_MOV__A_B,
    OP_MVI__C, 0x33, OP_MOV__B_C,
    OP_MVI__D, 0x44, OP_MOV__C_D,
    OP_MVI__E, 0x55, OP_MOV__D_E,
    OP_MVI__H, 0x66, OP_MOV__E_H,
    OP_MVI__L, 0x77, OP_MOV__H_L,
    OP_MVI__A, 0x11, OP_MOV__L_A,
    OP_NOP, OP_NOP, OP_NOP,
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

    i8080_one_instruction(cpu, 0);      // NOP

    i8080_one_instruction(cpu, 0);      // MVI B, 0x22
    i8080_one_instruction(cpu, 0);      // MOV A,B
    ASSERT_EQ_integer(0x22, cpu->A->value);

    i8080_one_instruction(cpu, 0);      // MVI C, 0x33
    i8080_one_instruction(cpu, 0);      // MOV B,C
    ASSERT_EQ_integer(0x33, cpu->B->value);

    i8080_one_instruction(cpu, 0);      // MVI D, 0x44
    i8080_one_instruction(cpu, 0);      // MOV C,D
    ASSERT_EQ_integer(0x44, cpu->C->value);

    i8080_one_instruction(cpu, 0);      // MVI E, 0x55
    i8080_one_instruction(cpu, 0);      // MOV D,E
    ASSERT_EQ_integer(0x55, cpu->D->value);

    i8080_one_instruction(cpu, 0);      // MVI H, 0x66
    i8080_one_instruction(cpu, 0);      // MOV E,H
    ASSERT_EQ_integer(0x66, cpu->E->value);

    i8080_one_instruction(cpu, 0);      // MVI L, 0x77
    i8080_one_instruction(cpu, 0);      // MOV H,L
    ASSERT_EQ_integer(0x77, cpu->H->value);

    i8080_one_instruction(cpu, 0);      // MVI A, 0x11
    i8080_one_instruction(cpu, 0);      // MOV L,A
    ASSERT_EQ_integer(0x11, cpu->L->value);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
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

    memcpy(ts->rom[0]->cells + cpu->PC->value, i8080_mov_program, sizeof(i8080_mov_program));

    data_z(cpu->A);
    data_z(cpu->B);
    data_z(cpu->C);
    data_z(cpu->D);
    data_z(cpu->E);
    data_z(cpu->H);
    data_z(cpu->L);

    i8080_one_instruction(cpu, 0);      // NOP

    t0 = TAU;
    i8080_one_instruction(cpu, 0);      // MVI B, 0x22
    i8080_one_instruction(cpu, 0);      // MOV A,B
    i8080_one_instruction(cpu, 0);      // MVI C, 0x33
    i8080_one_instruction(cpu, 0);      // MOV B,C
    sigplot_init(sp, ss, "i8080_bist_mov_bc", "Intel 8080 Single Chip 8-bit Microprocessor",
                 "MOV test for imm->B->A, imm->C->B", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
    ASSERT_EQ_integer(0x22, cpu->A->value);
    ASSERT_EQ_integer(0x33, cpu->B->value);

    t0 = TAU;
    i8080_one_instruction(cpu, 0);      // MVI D, 0x44
    i8080_one_instruction(cpu, 0);      // MOV C,D
    i8080_one_instruction(cpu, 0);      // MVI E, 0x55
    i8080_one_instruction(cpu, 0);      // MOV D,E
    sigplot_init(sp, ss, "i8080_bist_mov_de", "Intel 8080 Single Ehip 8-bit Microprocessor",
                 "MOV test for imm->D->C, imm->E->D", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
    ASSERT_EQ_integer(0x44, cpu->C->value);
    ASSERT_EQ_integer(0x55, cpu->D->value);

    t0 = TAU;
    i8080_one_instruction(cpu, 0);      // MVI H, 0x66
    i8080_one_instruction(cpu, 0);      // MOV E,H
    i8080_one_instruction(cpu, 0);      // MVI L, 0x77
    i8080_one_instruction(cpu, 0);      // MOV H,L
    sigplot_init(sp, ss, "i8080_bist_mov_hl", "Intel 8080 Single Ehip 8-bit Microprocessor",
                 "MOV test for imm->H->E, imm->L->H", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
    ASSERT_EQ_integer(0x66, cpu->E->value);
    ASSERT_EQ_integer(0x77, cpu->H->value);

    t0 = TAU;
    i8080_one_instruction(cpu, 0);      // MVI A, 0x11
    i8080_one_instruction(cpu, 0);      // MOV L,A
    sigplot_init(sp, ss, "i8080_bist_mov_a", "Intel 8080 Single Ehip 8-bit Microprocessor",
                 "MOV test for imm->A->L", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);
    ASSERT_EQ_integer(0x11, cpu->L->value);

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // NOP
}
