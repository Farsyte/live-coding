#include "clock.h"
#include "i8080_impl.h"

// i8080_movm: test the MOV M,Rs and MOV Rd,M instructions
//
// set the ram to a known non-matching pattern
// run the test program in "movmtest.hex"
// then check that 5 bytes are copied from rom to ram

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static f8080State   i8080_state_movm_test_fail;
static f8080State   i8080_state_movm_test_pass;
static f8080State   i8080_state_movm_test_page;

static int          i8080_movm_test_done;
static int          i8080_movm_test_page;

static p8080State   saved_rst0;
static p8080State   saved_rst1;
static p8080State   saved_rst2;

#define	RAM_TEST_AREA_SIZE 256
#define RAM_START_VAL	0x5A

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_movm_post(CpuTestSys ts)
{
    (void)ts;           // TODO copy bist and prune
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_movm_bist(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    Rom8316            *rom = ts->rom;
    Ram8107x8x4        *ram = ts->ram;

    pSigSess            ss = ts->ss;
    SigPlot             sp;

    ASSERT_EQ_integer(0x0000, cpu->PC->value);

    // override the RST 0 and RST 1 instruction decodes
    // so we can safely use them as fenceposts.

    saved_rst0 = cpu->m1t4[OP_RST__0];
    saved_rst1 = cpu->m1t4[OP_RST__1];
    saved_rst2 = cpu->m1t4[OP_RST__2];

    cpu->m1t4[OP_RST__0] = i8080_state_movm_test_fail;
    cpu->m1t4[OP_RST__1] = i8080_state_movm_test_pass;
    cpu->m1t4[OP_RST__2] = i8080_state_movm_test_page;

    i8080_movm_test_done = 0;
    i8080_movm_test_page = 0;

    int                 pgno = 0;

    rom8316_load(rom[0], 0x0000, "hex/movmtest.hex");

    memset(ram[0]->cells, RAM_START_VAL, RAM_TEST_AREA_SIZE);

    t0 = TAU;

    Cstr                fn;

    while (i8080_movm_test_done == 0) {
        clock_run_one();
        if (i8080_movm_test_page) {
            if (TAU > t0) {
                fn = format("i8080_movm_%d", ++pgno);

                sigplot_init(sp, ss, fn,
                             "Intel 8080 Single Chip 8-bit Microprocessor",
                             "MOV M instructions", t0, TAU - t0);
                i8080_plot_sigs(sp);
                sigplot_fini(sp);

                t0 = TAU;
            }
            i8080_movm_test_page = 0;
        }
    }

    if (TAU > t0) {
        fn = format("i8080_movm_%d", ++pgno);
        sigplot_init(sp, ss, fn,
                     "Intel 8080 Single Chip 8-bit Microprocessor",
                     "MOV M instructions", t0, TAU - t0);
        i8080_plot_sigs(sp);
        sigplot_fini(sp);
        t0 = TAU;
    }

    cpu->m1t4[OP_RST__0] = saved_rst0;
    cpu->m1t4[OP_RST__1] = saved_rst1;
    cpu->m1t4[OP_RST__2] = saved_rst2;

    assert(i8080_movm_test_done > 0);

    for (int i = 0; i < RAM_TEST_AREA_SIZE; ++i) {
        if (i < 5) {
            ASSERT_EQ_integer(rom[0]->cells[i], ram[0]->cells[i]);
        } else if (i == 5) {
            ASSERT_EQ_integer(0x55, ram[0]->cells[i]);
        } else if (i == 6) {
            ASSERT_EQ_integer(0xAA, ram[0]->cells[i]);
        } else if (i == 7) {
            ASSERT_EQ_integer(rom[0]->cells[i] + 1, ram[0]->cells[i]);
        } else if (i == 8) {
            ASSERT_EQ_integer(255 & (rom[0]->cells[i] - 1), ram[0]->cells[i]);
        } else if (i == 9) {
            ASSERT_EQ_integer(255 & (0x55 + rom[0]->cells[i]), ram[0]->cells[i]);
        } else if (i == 10) {
            ASSERT_EQ_integer(255 & (0x55 + 1 + rom[0]->cells[i]), ram[0]->cells[i]);
        } else if (i == 11) {
            ASSERT_EQ_integer(255 & (0x55 - rom[0]->cells[i]), ram[0]->cells[i]);
        } else if (i == 12) {
            ASSERT_EQ_integer(255 & (0x55 - 1 - rom[0]->cells[i]), ram[0]->cells[i]);
        } else if (i == 13) {
            ASSERT_EQ_integer(255 & (0x55 & rom[0]->cells[i]), ram[0]->cells[i]);
        } else if (i == 14) {
            ASSERT_EQ_integer(255 & (0x55 ^ rom[0]->cells[i]), ram[0]->cells[i]);
        } else if (i == 15) {
            ASSERT_EQ_integer(255 & (0x55 | rom[0]->cells[i]), ram[0]->cells[i]);
        } else if (i == 16) {
            ASSERT_EQ_integer(rom[0]->cells[i], ram[0]->cells[i]);
        } else {
            ASSERT_EQ_integer(RAM_START_VAL, ram[0]->cells[i]);
        }
    }
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void i8080_state_movm_test_fail(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_movm_test_done = -1;
          STUB("MOV M test FAIL hit at at PC=%04Xh", cpu->PC->value - 1);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_movm_test_pass(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_movm_test_done = 1;
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_movm_test_page(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_movm_test_page = 1;
          break;
      case PHI2_FALL:
          break;
    }

}
