#include <ctype.h>
#include "chip/i8080_test.h"

// i8080_rst: test the RST instructions.
//
// set the ram to a known non-matching pattern
// run the test program in "rsttest.hex"
// then check that (only) appropriate ram bytes are changed

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         i8080_rst_test_init(CpuTestSys);
static void         i8080_rst_test_fini(CpuTestSys);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_rst_post(CpuTestSys ts)
{
    (void)ts;           // TODO implement
//    i8080_rst_test_init(ts);
//    i8080_asm_post(ts, "hex/rsttest.hex");
//    i8080_rst_test_fini(ts);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_rst_bist(CpuTestSys ts)
{
    i8080_rst_test_init(ts);
    i8080_asm_bist(ts, "hex/rsttest.hex", "rst");
    i8080_rst_test_fini(ts);
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

#define	RAM_TEST_AREA_SIZE 256
#define RAM_START_VAL	0x5A

// i8080_rst_test_init: set initial conditions for BIST and POST for RST

static void i8080_rst_test_init(CpuTestSys ts)
{
    Byte               *ram = ts->ram[0]->cells;
    memset(ram, RAM_START_VAL, RAM_TEST_AREA_SIZE);
}

// i8080_rst_test_fini: check final state for BIST and POST for RST

static void i8080_rst_test_fini(CpuTestSys ts)
{
    const Byte         *ram = ts->ram[0]->cells;

    const int           rst_end = 0x0040;
    const int           stack_top = 0x0080;
    const int           stack_min = stack_top - 2 * 2;
    const int           log_at = 0x0080;
    const int           log_end = log_at + 8;

    // The test writes data to low ram that will be
    // executed by the RST instructions. each group of
    // eight bytes should be the same.

    for (int i = 8; i < rst_end; ++i) {
        ASSERT_EQ_integer(ram[i - 8], ram[i]);
    }

    // The test uses a stack growing down from 0080H,
    // and it should only use two words.
    for (int i = rst_end; i < stack_min; ++i) {
        ASSERT_EQ_integer(RAM_START_VAL, ram[i]);
    }

    for (int i = log_at; i < log_end; i++) {
        const int           retaddr = (i - log_at) * 8 + 4;
        ASSERT_EQ_integer(retaddr, ram[i]);
    }

    for (int i = log_end; i < RAM_TEST_AREA_SIZE; ++i) {
        ASSERT_EQ_integer(RAM_START_VAL, ram[i]);
    }
}
