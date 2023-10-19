#include "chip/i8080_test.h"

// i8080_call: test the CALL and Conditional Call instructions.
//
// set the ram to a known non-matching pattern
// run the test program in "calltest.hex"
// then check that (only) appropriate ram bytes are changed

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         i8080_call_test_init(CpuTestSys);
static void         i8080_call_test_fini(CpuTestSys);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_call_post(CpuTestSys ts)
{
    i8080_call_test_init(ts);
    i8080_asm_post(ts, "hex/calltest.hex");
    i8080_call_test_fini(ts);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_call_bist(CpuTestSys ts)
{
    i8080_call_test_init(ts);
    i8080_asm_bist(ts, "hex/calltest.hex", "call");
    i8080_call_test_fini(ts);
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

#define	RAM_TEST_AREA_SIZE 256
#define RAM_START_VAL	0x5A

// i8080_call_test_init: set initial conditions for BIST and POST for CALL

static void i8080_call_test_init(CpuTestSys ts)
{
    Byte               *ram = ts->ram[0]->cells;
    memset(ram, RAM_START_VAL, RAM_TEST_AREA_SIZE);
}

// i8080_call_test_fini: check final state for BIST and POST for CALL

static void i8080_call_test_fini(CpuTestSys ts)
{
    const Byte         *rom = ts->rom[0]->cells;
    const Byte         *ram = ts->ram[0]->cells;

    // ROM+128 holds data describing our results.
    // - a byte with the number of bytes pushed
    // - word values that should have been pushed

    int                 exp_at = 128;
    int                 obs_at = 128;
    int                 chk = rom[exp_at++];

    ASSERT_EQ_integer(chk, 18);

    chk /= 2;           // convert bytes to words

    while (chk-- > 0) {
        obs_at -= 2;
        ASSERT_EQ_integer(rom[exp_at + 0], ram[obs_at + 0]);
        ASSERT_EQ_integer(rom[exp_at + 1], ram[obs_at + 1]);
        exp_at += 2;
    }

    // check that the rest of RAM was not modified.

    for (int i = 0; i < obs_at; ++i) {
        ASSERT_EQ_integer(RAM_START_VAL, ram[i]);
    }
    for (int i = 128; i < RAM_TEST_AREA_SIZE; ++i) {
        ASSERT_EQ_integer(RAM_START_VAL, ram[i]);
    }
}
