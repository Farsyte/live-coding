#include "chip/i8080_test.h"

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

    Word                pc0 = cpu->PC->value;

    memcpy(ts->rom[0]->cells + cpu->PC->value,
           i8080_eidihlt_program, sizeof(i8080_eidihlt_program));

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // EI
    i8080_one_instruction(cpu, 0);      // DI
    i8080_one_instruction(cpu, 0);      // EI

    // Can't use i8080_one_instruction() for HLT as we
    // would never see the RETM1_INT signal rising.
    //
    // i8080_one_instruction(cpu, 0);      // HLT
    clock_run_until(TAU + 9 * 5 * 2);   // HLT and hold in halt for a bit

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

    memcpy(ts->rom[0]->cells, i8080_eidihlt_program, sizeof(i8080_eidihlt_program));

    Tau                 t0 = TAU;

    i8080_one_instruction(cpu, 0);      // NOP
    i8080_one_instruction(cpu, 0);      // EI
    i8080_one_instruction(cpu, 0);      // DI
    i8080_one_instruction(cpu, 0);      // EI
    // Can't use i8080_one_instruction() for HLT as we
    // would never see the RETM1_INT signal rising.
    //
    // i8080_one_instruction(cpu, 0);      // HLT
    clock_run_until(TAU + 9 * 5 * 2);   // HLT and hold in halt for a bit

    sigplot_init(sp, ss, "i8080_bist_eidihlt", "Intel 8080 Single Chip 8-bit Microprocessor",
                 "NOP EI HLT", t0, TAU - t0);
    i8080_plot_sigs(sp);
    sigplot_fini(sp);

    ASSERT_EQ_integer(1, cpu->READY->value);

    ASSERT_EQ_integer(5, cpu->PC->value);
    ASSERT_EQ_integer(0, cpu->SYNC->value);
    ASSERT_EQ_integer(1, cpu->INTE->value);
    ASSERT_EQ_integer(0, cpu->DBIN->value);
    ASSERT_EQ_integer(1, cpu->WAIT->value);
}
