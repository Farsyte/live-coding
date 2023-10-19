#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "chip/ram8107x8x4.h"

// === === === === === === === === === === === === === === === ===
//                           TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static Ram8107x8x4  ram;

static Addr         ADDR;
static Data         DATA;
static pEdge        RD_;
static pEdge        WR_;

static void         ram8107x8x4_test_init();
static void         ram8107x8x4_test_wr(Word addr, Byte data);
static void         ram8107x8x4_test_rd(Word addr, Byte data);
static void         ram8107x8x4_wr_for_bench(Word addr, Byte data);
static Byte         ram8107x8x4_rd_for_bench(Word addr);

// === === === === === === === === === === === === === === === ===
//                        POWER-ON SELF TEST
// === === === === === === === === === === === === === === === ===

// ram8107x8x4_post: Power-On Self Test for the ram8107x8x4 code
//
// This function should be called every time the program starts
// before any code relies on ram8107x8x4 not being completely broken.

void ram8107x8x4_post()
{
    ram8107x8x4_test_init();
    ram8107x8x4_invar(ram);

    ram8107x8x4_test_wr(0x0000, 0x00);
    ram8107x8x4_test_wr(0xFFFF, 0xFF);
    ram8107x8x4_test_rd(0x0000, 0x00);
    ram8107x8x4_test_rd(0xFFFF, 0xFF);

    ram8107x8x4_test_wr(0x0000, 0xFF);
    ram8107x8x4_test_wr(0xFFFF, 0x00);
    ram8107x8x4_test_rd(0x0000, 0xFF);
    ram8107x8x4_test_rd(0xFFFF, 0x00);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

// ram8107x8x4_bist: Power-On Self Test for the ram8107x8x4 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the ram8107x8x4 code is
// not working correctly.

void ram8107x8x4_bist()
{
    ram8107x8x4_test_init();
    ram8107x8x4_invar(ram);

    for (unsigned a = 0; a < RAM8107x8x4_SIZE; ++a)
        ram8107x8x4_test_wr(a, 0x55 ^ (a & 0xFF) ^ ((a >> 8) ^ 0xFF));
    for (unsigned a = 0; a < RAM8107x8x4_SIZE; ++a)
        ram8107x8x4_test_rd(a, 0x55 ^ (a & 0xFF) ^ ((a >> 8) ^ 0xFF));

    for (unsigned a = 0; a < RAM8107x8x4_SIZE; ++a)
        ram8107x8x4_test_wr(a, 0xAA ^ (a & 0xFF) ^ ((a >> 8) ^ 0xFF));
    for (unsigned a = 0; a < RAM8107x8x4_SIZE; ++a)
        ram8107x8x4_test_rd(a, 0xAA ^ (a & 0xFF) ^ ((a >> 8) ^ 0xFF));
}

// === === === === === === === === === === === === === === === ===
//                           BENCHMARKING
// === === === === === === === === === === === === === === === ===

Tau                 bench_ram8107x8x4_work = 0;

static void bench_ram8107x8x4_rd_for_bench(void *arg)
{
    (void)arg;
    for (int reps = 0; reps < 8; ++reps)
        for (unsigned a = 0; a < RAM8107x8x4_SIZE; ++a)
            (void)ram8107x8x4_rd_for_bench(a);

    bench_ram8107x8x4_work = 8 * RAM8107x8x4_SIZE;
}

static void bench_ram8107x8x4_wr_for_bench(void *arg)
{
    (void)arg;
    for (int reps = 0; reps < 8; ++reps)
        for (unsigned a = 0; a < RAM8107x8x4_SIZE; ++a)
            (void)ram8107x8x4_wr_for_bench(a, 0);

    bench_ram8107x8x4_work = 8 * RAM8107x8x4_SIZE;
}

// ram8107x8x4_bench: performance verification for the ram8107x8x4 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of ram8107x8x4.

void ram8107x8x4_bench()
{
    double              dt;
    ram8107x8x4_test_init();

    BENCH_TOP("ram8107x8x4");

    dt = RTC_ELAPSED(bench_ram8107x8x4_rd_for_bench, 0);
    BENCH_VAL(dt / bench_ram8107x8x4_work);

    dt = RTC_ELAPSED(bench_ram8107x8x4_wr_for_bench, 0);
    BENCH_VAL(dt / bench_ram8107x8x4_work);

    BENCH_END();

}

// === === === === === === === === === === === === === === === ===
//                           TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static void ram8107x8x4_test_init()
{
    RAM8107X8X4_INIT(ram);

    ADDR_INIT(ADDR);
    DATA_INIT(DATA);

    RD_ = ram->RD_;
    WR_ = ram->WR_;
    ram->ADDR = ADDR;
    ram->DATA = DATA;
    ram8107x8x4_linked(ram);
}

static void ram8107x8x4_test_wr(Word addr, Byte data)
{
    TAU += 1;
    addr_to(ADDR, addr);
    data_to(DATA, data);
    edge_lo(WR_);

    TAU += 1;
    edge_hi(WR_);
    addr_z(ADDR);
    data_z(DATA);
    assert(data_is_z(DATA));
}

static void ram8107x8x4_test_rd(Word addr, Byte data)
{
    TAU += 1;
    addr_to(ADDR, addr);
    data_z(DATA);
    edge_lo(RD_);

    ASSERT_EQ_integer(data, DATA->value);

    TAU += 1;
    edge_hi(RD_);
    addr_z(ADDR);
}

static void ram8107x8x4_wr_for_bench(Word addr, Byte data)
{
    TAU += 1;
    addr_to(ADDR, addr);
    data_to(DATA, data);
    edge_lo(WR_);

    TAU += 1;
    edge_hi(WR_);
}

static Byte ram8107x8x4_rd_for_bench(Word addr)
{
    Byte                data;

    TAU += 1;
    addr_to(ADDR, addr);
    edge_lo(RD_);
    data = DATA->value;

    TAU += 1;
    edge_hi(RD_);

    return data;
}
