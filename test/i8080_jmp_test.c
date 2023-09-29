#include "clock.h"
#include "i8080_impl.h"

// i8080_jmp_test: TODO write good comments

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_jmp_post(CpuTestSys ts)
{
    (void)ts;
    // TODO refine BIST into POST
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

static f8080State   i8080_state_jmp_test_fail;
static f8080State   i8080_state_jmp_test_pass;
static f8080State   i8080_state_jmp_test_page;

static int          i8080_jmp_test_done;
static int          i8080_jmp_test_page;

static void i8080_jmp_read(i8080 cpu)
{
    if (data_is_z(cpu->DATA))
        return;
    // STUB("%9lld: Addr=%04Xh Data=%02Xh", (long long)TAU, cpu->ADDR->value, cpu->DATA->value);
}

static p8080State   saved_rst0;
static p8080State   saved_rst1;
static p8080State   saved_rst2;

void i8080_jmp_bist(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;

    pSigSess            ss = ts->ss;
    SigPlot             sp;

    pAddr               PC = cpu->PC;

    data_z(cpu->A);

    ASSERT_EQ_integer(0x0000, PC->value);

    // override the RST 0 and RST 1 instruction decodes
    // so we can safely use them as fenceposts.

    saved_rst0 = cpu->m1t4[OP_RST__0];
    saved_rst1 = cpu->m1t4[OP_RST__1];
    saved_rst2 = cpu->m1t4[OP_RST__2];

    cpu->m1t4[OP_RST__0] = i8080_state_jmp_test_fail;
    cpu->m1t4[OP_RST__1] = i8080_state_jmp_test_pass;
    cpu->m1t4[OP_RST__2] = i8080_state_jmp_test_page;

    i8080_jmp_test_done = 0;
    i8080_jmp_test_page = 0;

    int                 pgno = 0;

    rom8316_load(rom[0], 0x0000, "hex/jmptest.hex");

    t0 = TAU;

    EDGE_ON_RISE(cpu->DBIN, i8080_jmp_read, cpu);

    Cstr                fn;

    while (i8080_jmp_test_done == 0) {
        clock_run_one();
        if (i8080_jmp_test_page) {
            if (TAU > t0) {
                fn = format("i8080_jmp_%d", ++pgno);

                sigplot_init(sp, ss, fn,
                             "Intel 8080 Single Chip 8-bit Microprocessor",
                             "JMP instructions", t0, TAU - t0);
                i8080_plot_sigs(sp);
                sigplot_fini(sp);

                t0 = TAU;
            }
            i8080_jmp_test_page = 0;
        }
    }

    if (TAU > t0) {
        fn = format("i8080_jmp_%d", ++pgno);
        sigplot_init(sp, ss, fn,
                     "Intel 8080 Single Chip 8-bit Microprocessor",
                     "JMP instructions", t0, TAU - t0);
        i8080_plot_sigs(sp);
        sigplot_fini(sp);
        t0 = TAU;
    }

    cpu->m1t4[OP_RST__0] = saved_rst0;
    cpu->m1t4[OP_RST__1] = saved_rst1;
    cpu->m1t4[OP_RST__2] = saved_rst2;
}

static void i8080_state_jmp_test_fail(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_jmp_test_done = -1;
          STUB("JMP test FAIL hit at at PC=%04Xh", cpu->PC->value - 1);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_jmp_test_pass(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_jmp_test_done = 1;
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_jmp_test_page(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_jmp_test_page = 1;
          break;
      case PHI2_FALL:
          break;
    }

}
