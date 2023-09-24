#include "clock.h"
#include "i8080_impl.h"

// i8080_mvi: manage most MVI instructions

// This is one way to not write nearly the same function
// over and over.

// NAME_M2T3(R): use token-pasting to construct the name.

#define NAME_M2T3(R)	i8080_state_mviM2T3 ## R
#define DEFN_M2T3(R)	static f8080State NAME_M2T3(R)

DEFN_M2T3(A);
DEFN_M2T3(B);
DEFN_M2T3(C);
DEFN_M2T3(D);
DEFN_M2T3(E);
DEFN_M2T3(H);
DEFN_M2T3(L);

static p8080State   i8080_state_mviM2T3[8] = {
    NAME_M2T3(B),
    NAME_M2T3(C),
    NAME_M2T3(D),
    NAME_M2T3(E),
    NAME_M2T3(H),
    NAME_M2T3(L),
    0,
    NAME_M2T3(A),
};

// i8080_mvi_init: set up decoding for MVI instructions

void i8080_mvi_init(i8080 cpu)
{
    for (int ddd = 0; ddd < 8; ++ddd) {
        p8080State          m2t3 = i8080_state_mviM2T3[ddd];
        if (!m2t3)
            continue;
        Byte                op = 0x06 | (ddd << 3);
        cpu->m1t4[op] = cpu->state_2bops;
        cpu->m2t3[op] = i8080_state_mviM2T3[ddd];
    }
}

// The IMPL_M2T3(R) macro is followed by a semicolon at the call-site.
// Because we do not want a semicolon after the function definition,
// repeat the declaration of the function which allows (and needs) it.

#define IMPL_M2T3(R)                                            \
    static void NAME_M2T3(R) (i8080 cpu, int phase) {           \
        switch (phase) {                                        \
        case PHI1_RISE:                                         \
            edge_lo(cpu->WAIT);                                 \
            edge_hi(cpu->RETM1_INT);                            \
            break;                                              \
        case PHI2_RISE:                                         \
            data_to(cpu->R, cpu->DATA->value);                  \
            edge_lo(cpu->DBIN);                                 \
            addr_z(cpu->ADDR);                                  \
            break;                                              \
        case PHI2_FALL:                                         \
            break;                                              \
        }                                                       \
    }                                                           \
    DEFN_M2T3(R)

IMPL_M2T3(A);
IMPL_M2T3(B);
IMPL_M2T3(C);
IMPL_M2T3(D);
IMPL_M2T3(E);
IMPL_M2T3(H);
IMPL_M2T3(L);

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static Byte         i8080_mvi_program[] = {
    OP_NOP,             // give us a moment
    MVI_A, 0x11,
    MVI_B, 0x22,
    MVI_C, 0x33,
    MVI_D, 0x44,
    MVI_E, 0x55,
    MVI_H, 0x66,
    MVI_L, 0x77,
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

    data_to(cpu->A, 0xFF);
    data_to(cpu->B, 0xFF);
    data_to(cpu->C, 0xFF);
    data_to(cpu->D, 0xFF);
    data_to(cpu->E, 0xFF);
    data_to(cpu->H, 0xFF);
    data_to(cpu->L, 0xFF);

    // run the NOP up to but not including the last cycle.
    clock_run_until(TAU + 9 * 4);       // nop, and leave us 1 clock be

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

    // run the NOP up to but not including the last cycle.
    clock_run_until(TAU + 9 * 4);       // nop, and leave us 1 clock be

    t0 = TAU;
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
