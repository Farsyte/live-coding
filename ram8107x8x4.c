#include "ram8107x8x4.h"

static void         ram8107x8x4_rd(Ram8107x8x4);
static void         ram8107x8x4_wr(Ram8107x8x4);

// ram8107x8x4: 16 KiB memory for 8080-like microcomputer system
//
// This module expects to connect to the Address and Data bus,
// and attach /RD and /WR signals to the decoder, to provide
// 16 KiB of memory to the simulated system.
//
// It is notionally modelled after a memory board making use of eight
// 8107B-4 (4096x1 bit) Dynamic Memory chips wide, by four rows, for a
// total of 16 KiB of memory.
//
// This module does not (yet) model the effects of failure to refresh
// each row of the memory within 

// ram8107x8x4_invar: verify the invariants for a ram8107x8x4.
// - valid pointer, valid name, not empty name.
// - valid signals attached, in a non-invalid configuration.

void ram8107x8x4_invar(Ram8107x8x4 ram)
{
    assert(ram);
    assert(ram->name);
    assert(ram->name[0]);

    assert(ram->ADDR);
    assert(ram->DATA);

    edge_invar(ram->RD_);
    edge_invar(ram->WR_);

    ASSERT_EQ_integer(1, ram->RD_->value || ram->WR_->value);
}

// ram8107x8x4_init(s): initialize the given ram8107x8x4 to have this
// name and an initial state.

void ram8107x8x4_init(Ram8107x8x4 ram, Cstr name)
{
    assert(ram);
    assert(name);
    assert(name[0]);

    assert(sizeof(ram->cells) > sizeof(pByte));
    assert(sizeof(ram->cells) == RAM8107x8x4_SIZE);
    assert(ram->cells != NULL);

    ram->name = name;
    edge_init(ram->RD_, format("%s:/RD", name));
    edge_hi(ram->RD_);
    edge_init(ram->WR_, format("%s:/WR", name));
    edge_hi(ram->WR_);

    memset(ram->cells, 0377, sizeof(ram->cells));

}

// ram8107x8x4_linked(s): finish initialization, called after pEdge
// fields are set.

void ram8107x8x4_linked(Ram8107x8x4 ram)
{
    assert(ram);

    EDGE_ON_FALL(ram->RD_, ram8107x8x4_rd, ram);
    EDGE_ON_FALL(ram->WR_, ram8107x8x4_wr, ram);

    ram8107x8x4_invar(ram);
}

static void ram8107x8x4_rd(Ram8107x8x4 ram)
{
    *ram->DATA = ram->cells[RAM8107x8x4_MASK & *ram->ADDR];
}

static void ram8107x8x4_wr(Ram8107x8x4 ram)
{
    ram->cells[RAM8107x8x4_MASK & *ram->ADDR] = *ram->DATA;
}

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

    ram8107x8x4_test_wr(0, 0);
    ram8107x8x4_test_wr(~0, 0377);
    ram8107x8x4_test_rd(0, 0);
    ram8107x8x4_test_rd(~0, 0377);

    ram8107x8x4_test_wr(0, 0377);
    ram8107x8x4_test_wr(~0, 0);
    ram8107x8x4_test_rd(0, 0377);
    ram8107x8x4_test_rd(~0, 0);
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
        ram8107x8x4_test_wr(a, 0125 ^ (a & 0377) ^ ((a >> 8) ^ 0377));
    for (unsigned a = 0; a < RAM8107x8x4_SIZE; ++a)
        ram8107x8x4_test_rd(a, 0125 ^ (a & 0377) ^ ((a >> 8) ^ 0377));

    for (unsigned a = 0; a < RAM8107x8x4_SIZE; ++a)
        ram8107x8x4_test_wr(a, 0252 ^ (a & 0377) ^ ((a >> 8) ^ 0377));
    for (unsigned a = 0; a < RAM8107x8x4_SIZE; ++a)
        ram8107x8x4_test_rd(a, 0252 ^ (a & 0377) ^ ((a >> 8) ^ 0377));
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
    RD_ = ram->RD_;
    WR_ = ram->WR_;
    ram->ADDR = ADDR;
    ram->DATA = DATA;
    ram8107x8x4_linked(ram);
}

static void ram8107x8x4_test_wr(Word addr, Byte data)
{
    TAU += 1;
    *ADDR = addr;
    *DATA = data;
    edge_lo(WR_);

    TAU += 1;
    *DATA ^= 0125;
    edge_hi(WR_);
    ASSERT_NE_integer(data, *DATA);
}

static void ram8107x8x4_test_rd(Word addr, Byte data)
{
    TAU += 1;
    *ADDR = addr;
    *DATA = data ^ 0125;
    edge_lo(RD_);
    ASSERT_EQ_integer(data, *DATA);

    TAU += 1;
    edge_hi(RD_);
}

static void ram8107x8x4_wr_for_bench(Word addr, Byte data)
{
    TAU += 1;
    *ADDR = addr;
    *DATA = data;
    edge_lo(WR_);

    TAU += 1;
    edge_hi(WR_);
}

static Byte ram8107x8x4_rd_for_bench(Word addr)
{
    Byte                data;

    TAU += 1;
    *ADDR = addr;
    edge_lo(RD_);
    data = *DATA;

    TAU += 1;
    edge_hi(RD_);

    return data;
}
