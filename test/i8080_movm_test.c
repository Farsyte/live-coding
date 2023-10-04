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

#define	RAM_TEST_AREA_SIZE 256
#define RAM_START_VAL	0x5A

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_movm_post(CpuTestSys ts)
{
    Ram8107x8x4        *ram = ts->ram;
    Rom8316            *rom = ts->rom;
    memset(ram[0]->cells, RAM_START_VAL, RAM_TEST_AREA_SIZE);

    i8080_asm_post(ts, "hex/movmtest.hex");

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
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

void i8080_movm_bist(CpuTestSys ts)
{

    Ram8107x8x4        *ram = ts->ram;
    Rom8316            *rom = ts->rom;
    memset(ram[0]->cells, RAM_START_VAL, RAM_TEST_AREA_SIZE);

    i8080_asm_bist(ts, "hex/movmtest.hex", "movm");

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
