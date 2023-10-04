#include "i8080_test.h"

// i8080_stack: test the MOV M,Rs and MOV Rd,M instructions
//
// set the ram to a known non-matching pattern
// run the test program in "stacktest.hex"
// then check that 5 bytes are copied from rom to ram

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

static void         i8080_stack_test_init(CpuTestSys);
static void         i8080_stack_test_fini(CpuTestSys);

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_stack_post(CpuTestSys ts)
{
    i8080_stack_test_init(ts);
    i8080_asm_post(ts, "hex/stacktest.hex");
    i8080_stack_test_fini(ts);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_stack_bist(CpuTestSys ts)
{
    i8080_stack_test_init(ts);
    i8080_asm_bist(ts, "hex/stacktest.hex", "stack");
    i8080_stack_test_fini(ts);
}

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

#define	RAM_TEST_AREA_SIZE 256
#define RAM_START_VAL	0x5A

static void i8080_stack_test_init(CpuTestSys ts)
{
    Byte               *ram = ts->ram[0]->cells;
    memset(ram, RAM_START_VAL, RAM_TEST_AREA_SIZE);
}

static void i8080_stack_test_fini(CpuTestSys ts)
{
    Byte               *rom = ts->rom[0]->cells;
    Byte               *ram = ts->ram[0]->cells;

    for (int i = 0; i < RAM_TEST_AREA_SIZE; ++i) {
        if (i < 128) {
            ASSERT_EQ_integer(RAM_START_VAL, ram[i]);
        } else if (i == 128) {
            ASSERT_EQ_integer(rom[i], ram[i]);
        } else if (i == 129) {
            ASSERT_EQ_integer(rom[i], ram[i]);
        } else if (i == 130) {
            ASSERT_EQ_integer(rom[i], ram[i]);
        } else if (i == 131) {
            ASSERT_EQ_integer(rom[i], ram[i]);
        } else if (i == 132) {
            ASSERT_EQ_integer(rom[i], ram[i]);
        } else if (i == 133) {
            ASSERT_EQ_integer(rom[i], ram[i]);
        } else if (i == 134) {
            ASSERT_EQ_integer(0x02, ram[i]);
        } else if (i == 135) {
            ASSERT_EQ_integer(rom[i], ram[i]);
        } else {
            ASSERT_EQ_integer(RAM_START_VAL, ram[i]);
        }
    }
}
