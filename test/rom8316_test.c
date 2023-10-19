#include "chip/rom8316.h"
#include "common/hex.h"

// === === === === === === === === === === === === === === === ===
//                           TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static Rom8316      rom;

static Addr         ADDR;
static Data         DATA;
static Edge         RDYIN;
static pEdge        RD_;

static void         rom8316_test_init();
static void         rom8316_test_rd(Word addr, Byte data);
static Byte         rom8316_rd_for_bench(Word addr);

// === === === === === === === === === === === === === === === ===
//                        POWER-ON SELF TEST
// === === === === === === === === === === === === === === === ===

// rom8316_post: Power-On Self Test for the rom8316 code
//
// This function should be called every time the program starts
// before any code relies on rom8316 not being completely broken.
//
// spot checks that we can retrieve test data from the start and
// end of the ROM.

void rom8316_post()
{
    unsigned            e = ROM8316_SIZE - 1;

    rom8316_test_init();
    rom8316_invar(rom);

    rom->cells[0] = 0x55;
    rom->cells[e] = 0xAA;

    rom8316_test_rd(0, 0x55);
    rom8316_test_rd(e, 0xAA);

    rom->cells[0] = 0xAA;
    rom->cells[e] = 0x55;

    rom8316_test_rd(0, 0xAA);
    rom8316_test_rd(e, 0x55);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

// rom8316_bist: Power-On Self Test for the rom8316 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the rom8316 code is
// not working correctly.

void rom8316_bist()
{
    rom8316_test_init();
    rom8316_invar(rom);

    // load the data for the 2nd rom for fig-forth.
    // test pass or fail based on comparison of output
    // text versus saved output text.

    ROM8316_LOAD(rom, ROM8316_SIZE, "FIG-FORTH-8080");
    rom8316_dump(rom);

    // two "barber pole" tests: set the ROM to contain
    // a fixed pattern, then check it; repeat with a
    // different pattern.

    for (unsigned a = 0; a < ROM8316_SIZE; ++a)
        rom->cells[a] = 0x55 ^ (a & 0xFF) ^ ((a >> 8) ^ 0xFF);

    for (unsigned a = 0; a < ROM8316_SIZE; ++a)
        rom8316_test_rd(a, 0x55 ^ (a & 0xFF) ^ ((a >> 8) ^ 0xFF));

    for (unsigned a = 0; a < ROM8316_SIZE; ++a)
        rom->cells[a] = 0xAA ^ (a & 0xFF) ^ ((a >> 8) ^ 0xFF);

    for (unsigned a = 0; a < ROM8316_SIZE; ++a)
        rom8316_test_rd(a, 0xAA ^ (a & 0xFF) ^ ((a >> 8) ^ 0xFF));
}

// === === === === === === === === === === === === === === === ===
//                           BENCHMARKING
// === === === === === === === === === === === === === === === ===

Tau                 bench_rom8316_work = 0;

// bench_rom8316_rd_for_bench(a): do many rom accesses

static void bench_rom8316_rd_for_bench(void *arg)
{
    (void)arg;
    for (int reps = 0; reps < 8; ++reps)
        for (unsigned a = 0; a < ROM8316_SIZE; ++a)
            (void)rom8316_rd_for_bench(a);

    bench_rom8316_work = 8 * ROM8316_SIZE;
}

// rom8316_bench: performance verification for the rom8316 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of rom8316.

void rom8316_bench()
{
    double              dt;
    rom8316_test_init();

    BENCH_TOP("rom8316");

    dt = RTC_ELAPSED(bench_rom8316_rd_for_bench, 0);
    BENCH_VAL(dt / bench_rom8316_work);

    BENCH_END();
}

// === === === === === === === === === === === === === === === ===
//                           TEST SUPPORT
// === === === === === === === === === === === === === === === ===

// rom8316_test_init(): set up file-static data for post/bist/bench

static void rom8316_test_init()
{
    ROM8316_INIT(rom);

    EDGE_INIT(RDYIN, 0);
    ADDR_INIT(ADDR);
    DATA_INIT(DATA);

    RD_ = rom->RD_;

    rom->RDYIN = RDYIN;
    rom->ADDR = ADDR;
    rom->DATA = DATA;
    rom8316_linked(rom);
}

// rom8316_test_rd(addr, data): do a test read cycle
// read data using the environment-owned signals,
// honoring the environment-owned READY IN signal.
// Verify that the results are as expected.

static void rom8316_test_rd(Word addr, Byte data)
{
    TAU += 1;
    addr_to(ADDR, addr);
    data_z(DATA);
    edge_lo(RD_);

    while (0 == RDYIN->value) {
        TAU += 1;
        RD_->value = 1;
        edge_lo(RD_);
    }

    ASSERT_EQ_integer(data, DATA->value);
    TAU += 1;
    edge_hi(RD_);
}

// rom8316_rd_for_bench(addr): do a read cycle as part of
// benchmarking. Uses the environment owned signals, honors RDYIN, and
// returns the data recovered.

static Byte rom8316_rd_for_bench(Word addr)
{
    Byte                data;

    TAU += 1;
    addr_to(ADDR, addr);
    edge_lo(RD_);

    while (0 == RDYIN->value) {
        TAU += 1;
        RD_->value = 1;
        edge_lo(RD_);
    }

    data = DATA->value;
    TAU += 1;
    edge_hi(RD_);

    return data;
}
