#include "clock.h"
#include "i8080_impl.h"

// i8080_mov: manage most MOV instructions
//
// This implementation uses one individual T-state management function
// for each source register, and one for each destination; each of the
// instructions will mix-and-match the correct source and destination.

// It is indeed possible to use a single function for T4 and a single
// function for T5, but this means additional work within each function
// to select the register, when we already are pulling the function
// pointer from different places for each function.
//
// using 64 distinct M1T4 functions would reduce runtime by whatever it
// cost us to fetch from cpu->m1t5[cpu->IR->value], which may or may not
// be significant enough to warrent the cost of having so many more small
// functions kicking around.

#define NAME_M1T4(R)	i8080_state_movT4 ## R
#define DEFN_M1T4(R)	static f8080State NAME_M1T4(R)

#define NAME_M1T5(R)	i8080_state_movT5 ## R
#define DEFN_M1T5(R)	static f8080State NAME_M1T5(R)

DEFN_M1T4(B);
DEFN_M1T4(C);
DEFN_M1T4(D);
DEFN_M1T4(E);
DEFN_M1T4(H);
DEFN_M1T4(L);
DEFN_M1T4(A);

DEFN_M1T5(B);
DEFN_M1T5(C);
DEFN_M1T5(D);
DEFN_M1T5(E);
DEFN_M1T5(H);
DEFN_M1T5(L);
DEFN_M1T5(A);

static p8080State   i8080_state_movT4[8] = {
    NAME_M1T4(B),
    NAME_M1T4(C),
    NAME_M1T4(D),
    NAME_M1T4(E),
    NAME_M1T4(H),
    NAME_M1T4(L),
    0,
    NAME_M1T4(A),
};

static p8080State   i8080_state_movT5[8] = {
    NAME_M1T5(B),
    NAME_M1T5(C),
    NAME_M1T5(D),
    NAME_M1T5(E),
    NAME_M1T5(H),
    NAME_M1T5(L),
    0,
    NAME_M1T5(A),
};

// i8080_mov_init: set up decoding for MOV instructions
// operating purely on our eight-bit registers.

void i8080_mov_init(i8080 cpu)
{
    for (int sss = 0; sss < 8; ++sss) {
        p8080State          t4 = i8080_state_movT4[sss];
        if (!t4)
            continue;
        for (int ddd = 0; ddd < 8; ++ddd) {
            p8080State          t5 = i8080_state_movT5[ddd];
            if (!t5)
                continue;
            Byte                op = 0x40 | (ddd << 3) | sss;
            cpu->m1t4[op] = i8080_state_movT4[sss];
            cpu->m1t5[op] = i8080_state_movT5[ddd];
        }
    }
}

// The IMPL_M1T4(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M1T4(R)                                            \
    static void NAME_M1T4(R) (i8080 cpu, int phase) {           \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->TMP, cpu->R->value);                   \
            break;                                              \
        case PHI2_FALL:                                         \
            cpu->state_next = cpu->m1t5[cpu->IR->value];        \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M1T4(R)

IMPL_M1T4(A);
IMPL_M1T4(B);
IMPL_M1T4(C);
IMPL_M1T4(D);
IMPL_M1T4(E);
IMPL_M1T4(H);
IMPL_M1T4(L);

// The IMPL_M1T5(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M1T5(R)                                            \
    static void NAME_M1T5(R) (i8080 cpu, int phase) {           \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            edge_hi(cpu->RETM1_INT);                            \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->R, cpu->TMP->value);                   \
            break;                                              \
        case PHI2_FALL:                                         \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M1T5(R)

IMPL_M1T5(A);
IMPL_M1T5(B);
IMPL_M1T5(C);
IMPL_M1T5(D);
IMPL_M1T5(E);
IMPL_M1T5(H);
IMPL_M1T5(L);

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static Byte         i8080_mov_program[] = {
    OP_NOP,             // give us a moment
    MVI_B, 0x22, MOV_A_B,
    MVI_C, 0x33, MOV_B_C,
    MVI_D, 0x44, MOV_C_D,
    MVI_E, 0x55, MOV_D_E,
    MVI_H, 0x66, MOV_E_H,
    MVI_L, 0x77, MOV_H_L,
    MVI_A, 0x11, MOV_L_A,
    0xFF,               // make this look like uninitialized memory
};

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_mov_post(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;

    ASSERT_EQ_integer(sizeof(ts->rom[0]->cells[0]), sizeof(i8080_mov_program[0]));

    memcpy(ts->rom[0]->cells + ts->cpu->PC->value,
           i8080_mov_program, sizeof(i8080_mov_program));

    data_to(cpu->A, 0xFF);
    data_to(cpu->B, 0xFF);
    data_to(cpu->C, 0xFF);
    data_to(cpu->D, 0xFF);
    data_to(cpu->E, 0xFF);
    data_to(cpu->H, 0xFF);
    data_to(cpu->L, 0xFF);

    // run the NOP up to but not including the last clock of the machine cycle
    clock_run_until(TAU + 9 * 5);

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

    data_to(cpu->A, 0xFF);
    data_to(cpu->B, 0xFF);
    data_to(cpu->C, 0xFF);
    data_to(cpu->D, 0xFF);
    data_to(cpu->E, 0xFF);
    data_to(cpu->H, 0xFF);
    data_to(cpu->L, 0xFF);

    // run the NOP up to but not including the last clock of the machine cycle
    clock_run_until(TAU + 9 * 5);

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
