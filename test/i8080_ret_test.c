#include "i8080_test.h"

// i8080_ret: test the RET and Conditional Return instructions.
//
// set the ram to a known non-matching pattern
// run the test program in "rettest.hex"
// then check that (only) appropriate ram bytes are changed

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         i8080_ret_test_init(CpuTestSys);
static void         i8080_ret_test_fini(CpuTestSys);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_ret_post(CpuTestSys ts)
{
    i8080_ret_test_init(ts);
    i8080_asm_post(ts, "hex/rettest.hex");
    i8080_ret_test_fini(ts);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_ret_bist(CpuTestSys ts)
{
    i8080_ret_test_init(ts);
    i8080_asm_bist(ts, "hex/rettest.hex", "ret");
    i8080_ret_test_fini(ts);
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

#define	RAM_TEST_AREA_SIZE 256
#define RAM_START_VAL	0x5A

// i8080_ret_test_init: set initial conditions for BIST and POST for RET

static void i8080_ret_test_init(CpuTestSys ts)
{
    Byte               *ram = ts->ram[0]->cells;
    memset(ram, RAM_START_VAL, RAM_TEST_AREA_SIZE);
}

// i8080_ret_test_fini: check final state for BIST and POST for RET

static void i8080_ret_test_fini(CpuTestSys ts)
{
    const Byte         *ram = ts->ram[0]->cells;

    // No changes to RAM before the LOG area.

    for (int i = 0; i < 192; ++i) {
        ASSERT_EQ_integer(RAM_START_VAL, ram[i]);
    }

    // Expect 9 bytes to be modified to contain L of their HL.

    for (int i = 192; i < 201; ++i) {
        ASSERT_EQ_integer(i, ram[i]);
    }

    // Expect rest of RAM to be unmodified.

    for (int i = 201; i < RAM_TEST_AREA_SIZE; ++i) {
        ASSERT_EQ_integer(RAM_START_VAL, ram[i]);
    }
}
