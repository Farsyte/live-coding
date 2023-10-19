#include "chip/i8080_impl.h"
#include "common/clock.h"
#include "common/sigtrace.h"

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

    if (VAL(RESET))
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

    ATRI(PC);
    ATRI(IDAL);
    ATRI(ADDR);

    DTRI(DATA);
    DTRI(IR);
    DTRI(ACT);
    DTRI(TMP);
    DTRI(ALU);
    DTRI(FLAGS);

    // Update state of the edges, but
    // DO NOT trigger callbacks for
    // the falling edges!

    VAL(SYNC) = 0;
    VAL(DBIN) = 0;
    VAL(WAIT) = 0;

}

// i8080_state_reset: T-State handler for RESET states

static void i8080_state_reset(i8080 cpu, int phase)
{
    // only look at RESET at the very start of the T-state.
    if (phase != PHI1_RISE)
        return;

    // If RESET is still high, assure RESET_INT
    // is asserted and nothing more.
    if (VAL(RESET)) {
        RAISE(RESET_INT);
        return;
    }
    // We get here when RESET is finally released. Reset the PC to
    // zero and deliver control to the STATUS_FETCH machine cycle.

    ASET(PC, 0);
    cpu->state_m1t1 = cpu->state_fetch;

    // Drop the internal reset signal and activate "RETURN M1"
    // so we make our way around to the M1T1 set above at the
    // appropriate time.

    LOWER(RESET_INT);
    RAISE(RETM1_INT);
}
