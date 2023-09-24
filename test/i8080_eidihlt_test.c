#include "clock.h"
#include "i8080_impl.h"

// i8080_eidihlt: manage ei, di, and hlt instructions

static Byte         i8080_eidihlt_program[] = {
    OP_NOP,
    OP_EI,
    OP_DI,
    OP_EI,
    OP_HLT,
    0xFF,               // make this look like uninitialized memory
};

void i8080_eidihlt_post(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;

    Word                pc0 = ts->cpu->PC->value;

    memcpy(ts->rom[0]->cells + ts->cpu->PC->value,
           i8080_eidihlt_program, sizeof(i8080_eidihlt_program));

    Tau                 t0 = TAU;

    clock_run_until(t0 + 9 * 5 * 7);

    ASSERT_EQ_integer(1, cpu->READY->value);

    ASSERT_EQ_integer(pc0 + 5, cpu->PC->value);
    ASSERT_EQ_integer(0, cpu->SYNC->value);
    ASSERT_EQ_integer(1, cpu->INTE->value);
    ASSERT_EQ_integer(0, cpu->DBIN->value);
    ASSERT_EQ_integer(1, cpu->WAIT->value);
}

void i8080_eidihlt_bist(CpuTestSys ts)
{
    p8080               cpu = ts->cpu;
    pSigSess            ss = ts->ss;
    SigPlot             sp;

    Word                pc0 = ts->cpu->PC->value;

    memcpy(ts->rom[0]->cells + ts->cpu->PC->value,
           i8080_eidihlt_program, sizeof(i8080_eidihlt_program));

    Tau                 t0 = TAU;

    clock_run_until(t0 + 9 * 5 * 7);

    sigplot_init(sp, ss, "i8080_bist_eidihlt", "Intel 8080 Single Chip 8-bit Microprocessor",
                 // "NOP, DI, EI", t0 + 9 * 4, 9 * 12);
                 "NOP EI HLT", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    ASSERT_EQ_integer(1, cpu->READY->value);

    ASSERT_EQ_integer(pc0 + 5, cpu->PC->value);
    ASSERT_EQ_integer(0, cpu->SYNC->value);
    ASSERT_EQ_integer(1, cpu->INTE->value);
    ASSERT_EQ_integer(0, cpu->DBIN->value);
    ASSERT_EQ_integer(1, cpu->WAIT->value);
}
