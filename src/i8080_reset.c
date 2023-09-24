#include "clock.h"
#include "i8080_impl.h"
#include "sigtrace.h"

// i8080_reset: manage the RESET machine cycles When RESET is active,
// the cpu jumps into a hole and pulls the hole in behind it. When
// RESET is released, it climbs out carefully and starts running with
// PC set to 0.

// f8080State is the type fo the state functions, which we can use for
// declaring the function, giving us a nice check of the type beyond
// just the one at the assignment.

static f8080State   i8080_state_reset;

// We need to capture rising edges of RESET.

static void         i8080_reset_rise(i8080 cpu);

// Hook the reset response up to the RESET signal
// and publish our T-State management functions.

void i8080_reset_init(i8080 cpu)
{
    cpu->state_reset = i8080_state_reset;

    EDGE_ON_RISE(cpu->RESET, i8080_reset_rise, cpu);

    if (cpu->RESET->value)
        i8080_reset_rise(cpu);
}

// i8080_reset_rise: notice RESET rising edge
//
// slam the T-State management function for RESET cycles into the
// active state pointers. Other bits of code are setting state,
// state_next, state_m1t1 and we want to be sure to end up in
// i8080_state_reset.

static void i8080_reset_rise(i8080 cpu)
{
    cpu->state = i8080_state_reset;
    cpu->state_next = i8080_state_reset;
    cpu->state_m1t1 = i8080_state_reset;
    addr_z(cpu->PC);
    addr_z(cpu->ADDR);
    data_z(cpu->DATA);
    data_z(cpu->IR);
}

// i8080_state_reset: T-State handler for RESET states

static void i8080_state_reset(i8080 cpu, int phase)
{
    // only look at RESET at the very start of the T-state.
    if (phase != PHI1_RISE)
        return;

    // If RESET is still high, assure RESET_INT
    // is asserted and nothing more.
    if (cpu->RESET->value) {
        edge_hi(cpu->RESET_INT);
        return;
    }
    // We get here when RESET is finally released. Reset the PC to
    // zero and deliver control to the STATUS_FETCH machine cycle.

    addr_to(cpu->PC, 0);
    cpu->state_m1t1 = cpu->state_fetch;

    // Drop the internal reset signal and activate "RETURN M1"
    // so we make our way around to the M1T1 set above at the
    // appropriate time.

    edge_lo(cpu->RESET_INT);
    edge_hi(cpu->RETM1_INT);
}

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
