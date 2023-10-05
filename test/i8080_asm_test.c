#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "i8080_test.h"

// i8080_asm_test: run an ASM test program

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static f8080State   i8080_state_asm_test_fail;
static f8080State   i8080_state_asm_test_pass;
static f8080State   i8080_state_asm_test_page;

static int          i8080_asm_test_done;
static int          i8080_asm_test_page;

static p8080State   saved_op08;
static p8080State   saved_op10;
static p8080State   saved_op18;

static Cstr         i8080_asm_hexfile;

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_asm_post(CpuTestSys ts, Cstr hexfile)
{
    i8080_asm_hexfile = hexfile;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;

    pAddr               PC = cpu->PC;

    ASSERT_EQ_integer(0x0000, PC->value);

    addr_z(cpu->ADDR);
    addr_z(cpu->SP);
    addr_z(cpu->IDAL);

    data_z(cpu->DATA);
    data_z(cpu->A);
    data_z(cpu->B);
    data_z(cpu->C);
    data_z(cpu->D);
    data_z(cpu->E);
    data_z(cpu->H);
    data_z(cpu->L);
    data_z(cpu->IR);
    data_z(cpu->TMP);
    data_z(cpu->ACT);
    data_z(cpu->ALU);
    data_z(cpu->W);
    data_z(cpu->Z);
    data_z(cpu->FLAGS);

    // override the OP 0x08, 0x10, and 0x18 instruction decodes
    // so we can safely use them as fenceposts.

    saved_op08 = cpu->m1t4[OP__08];
    saved_op10 = cpu->m1t4[OP__10];
    saved_op18 = cpu->m1t4[OP__18];

    cpu->m1t4[OP__08] = i8080_state_asm_test_fail;
    cpu->m1t4[OP__10] = i8080_state_asm_test_pass;
    cpu->m1t4[OP__18] = i8080_state_asm_test_page;

    i8080_asm_test_done = 0;

    rom8316_load(rom[0], 0x0000, hexfile);

    while (i8080_asm_test_done == 0) {
        clock_run_one();
        // ignore i8080_asm_test_page during POST
        // TODO provide a timeout here.
    }

    ASSERT_EQ_integer(1, i8080_asm_test_done);

    cpu->m1t4[OP__08] = saved_op08;
    cpu->m1t4[OP__10] = saved_op10;
    cpu->m1t4[OP__18] = saved_op18;
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_asm_bist(CpuTestSys ts, Cstr hexfile, Cstr plotname)
{
    i8080_asm_hexfile = hexfile;

    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;

    pSigSess            ss = ts->ss;
    SigPlot             sp;

    pAddr               PC = cpu->PC;

    ASSERT_EQ_integer(0x0000, PC->value);

    addr_z(cpu->ADDR);
    addr_z(cpu->SP);
    addr_z(cpu->IDAL);

    data_z(cpu->DATA);
    data_z(cpu->A);
    data_z(cpu->B);
    data_z(cpu->C);
    data_z(cpu->D);
    data_z(cpu->E);
    data_z(cpu->H);
    data_z(cpu->L);
    data_z(cpu->IR);
    data_z(cpu->TMP);
    data_z(cpu->ACT);
    data_z(cpu->ALU);
    data_z(cpu->W);
    data_z(cpu->Z);
    data_z(cpu->FLAGS);

    // override the OP 0x08, 0x10, and 0x18 instruction decodes
    // so we can safely use them as fenceposts.

    saved_op08 = cpu->m1t4[OP__08];
    saved_op10 = cpu->m1t4[OP__10];
    saved_op18 = cpu->m1t4[OP__18];

    cpu->m1t4[OP__08] = i8080_state_asm_test_fail;
    cpu->m1t4[OP__10] = i8080_state_asm_test_pass;
    cpu->m1t4[OP__18] = i8080_state_asm_test_page;

    i8080_asm_test_done = 0;
    i8080_asm_test_page = 0;

    rom8316_load(rom[0], 0x0000, hexfile);

    t0 = TAU;

    Cstr                fn;
    Cstr                cap;

    int                 pgno = 0;

    while (i8080_asm_test_done == 0) {
        clock_run_one();
        if (i8080_asm_test_page) {
            if (TAU > t0) {
                ++pgno;
                fn = format("i8080_bist_%s_%d", plotname, pgno);
                cap = format("8080 '%s' BIST plot %d", plotname, pgno);

                sigplot_init(sp, ss, fn,
                             "Intel 8080 Single Chip 8-bit Microprocessor", cap, t0, TAU - t0);
                i8080_plot_sigs(sp);
                sigplot_fini(sp);

                t0 = TAU;
            }
            i8080_asm_test_page = 0;
        }
    }

    if (TAU > t0) {
        ++pgno;
        fn = format("i8080_bist_%s_%d", plotname, pgno);
        cap = format("8080 '%s' BIST plot %d", plotname, pgno);
        sigplot_init(sp, ss, fn,
                     "Intel 8080 Single Chip 8-bit Microprocessor", cap, t0, TAU - t0);
        i8080_plot_sigs(sp);
        sigplot_fini(sp);
        t0 = TAU;
    }

    ASSERT_EQ_integer(1, i8080_asm_test_done);

    cpu->m1t4[OP__08] = saved_op08;
    cpu->m1t4[OP__10] = saved_op10;
    cpu->m1t4[OP__18] = saved_op18;

}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void i8080_state_asm_test_fail(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_asm_test_done = -1;
          STUB("ASM '%s' FAIL hit at at PC=%04Xh", i8080_asm_hexfile, cpu->PC->value - 1);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_asm_test_pass(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_asm_test_done = 1;
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_asm_test_page(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_asm_test_page = 1;
          break;
      case PHI2_FALL:
          break;
    }

}
