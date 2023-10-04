#include "i8080_test.h"

// i8080_reset: manage the RESET machine cycles When RESET is active,
// the cpu jumps into a hole and pulls the hole in behind it. When
// RESET is released, it climbs out carefully and starts running with
// PC set to 0.

static Byte         i8080_reset_program[] = {
    OP_NOP,
    OP_NOP,
    0xFF,               // make this look like uninitialized memory
};

void i8080_reset_post(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;

    memcpy(ts->rom[0]->cells, i8080_reset_program, sizeof(i8080_reset_program));

    Tau                 t0 = TAU;

    clock_run_until(t0 + 35);
    edge_hi(ts->RESIN_);
    clock_run_until(t0 + 9 * 5 * 3 + 18);

    ASSERT_EQ_integer(0, cpu->RESET->value);
    ASSERT_EQ_integer(1, cpu->READY->value);

    ASSERT_EQ_integer(0, cpu->RESET_INT->value);

    ASSERT_EQ_integer(0x0002, cpu->PC->value);
    ASSERT_EQ_integer(1, cpu->SYNC->value);
    ASSERT_EQ_integer(0, cpu->DBIN->value);
    ASSERT_EQ_integer(0, cpu->WAIT->value);
}

void i8080_reset_bist(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;

    pSigSess            ss = ts->ss;

    memcpy(ts->rom[0]->cells, i8080_reset_program, sizeof(i8080_reset_program));

    Tau                 t0 = TAU;

    clock_run_until(t0 + 35);
    edge_hi(ts->RESIN_);
    clock_run_until(t0 + 9 * 5 * 3 + 18);

    SigPlot             sp;
    sigplot_init(sp, ss, "i8080_bist_reset",
                 "Intel 8080 Single Chip 8-bit Microprocessor",
                 "Power-on RESET, NOP, NOP", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    ASSERT_EQ_integer(0, cpu->RESET->value);
    ASSERT_EQ_integer(1, cpu->READY->value);

    ASSERT_EQ_integer(0, cpu->RESET_INT->value);

    ASSERT_EQ_integer(0x0002, cpu->PC->value);
    ASSERT_EQ_integer(1, cpu->SYNC->value);
    ASSERT_EQ_integer(0, cpu->DBIN->value);
    ASSERT_EQ_integer(0, cpu->WAIT->value);
}
