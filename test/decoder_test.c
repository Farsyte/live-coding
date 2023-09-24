#include "decoder.h"

// decoder: efficient enables for mem pages and i/o ports
//
// This module monitors the control bus and the address bus,
// triggering edges registered for each page of memory and
// each I/O port, separately for read and write, to avoid the
// performance issues around having every memory and device
// monitoring DBIN and/or /WR.

// === === === === === === === === === === === === === === === ===
//                           TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static Addr         ADDR;

static Edge         MEMR_;
static Edge         MEMW_;
static Edge         IOR_;
static Edge         IOW_;

static Edge         mem_rd_first;
static Edge         mem_rd_final;
static Edge         mem_wr_first;
static Edge         mem_wr_final;
static Edge         dev_rd_first;
static Edge         dev_rd_final;
static Edge         dev_wr_first;
static Edge         dev_wr_final;
static Edge         rom_shadow;

static Decoder      dec;

static void         decoder_test_init();
static void         check_outputs(pEdge e, Bit expected);
static void         dec_test_trigger(pEdge e, Word a, pEdge o);
static void         dec_bench_trigger(pEdge e, Word a);

// === === === === === === === === === === === === === === === ===
//                        POWER-ON SELF TEST
// === === === === === === === === === === === === === === === ===

// decoder_post: Power-On Self Test for the decoder code
//
// This function should be called every time the program starts
// before any code relies on decoder not being completely broken.

void decoder_post()
{
    decoder_test_init();

    pEdge              *mem_rd = dec->mem_rd;
    pEdge              *mem_wr = dec->mem_wr;
    pEdge              *dev_rd = dec->dev_rd;
    pEdge              *dev_wr = dec->dev_wr;

    (void)mem_rd;
    (void)mem_wr;
    (void)dev_rd;
    (void)dev_wr;

    // test case: access via NULL edge does not crash.

    dec_test_trigger(MEMR_, 0x0000, 0);
    dec_test_trigger(MEMR_, 0xFFFF, 0);
    dec_test_trigger(MEMW_, 0x0000, 0);
    dec_test_trigger(MEMR_, 0xFFFF, 0);
    dec_test_trigger(IOR_, 0x0000, 0);
    dec_test_trigger(IOR_, 0xFFFF, 0);
    dec_test_trigger(IOW_, 0x0000, 0);
    dec_test_trigger(IOR_, 0xFFFF, 0);

    // hook up the memories and devices

    mem_rd[0] = mem_rd_first;
    mem_rd[DEC_MEM_PAGES - 1] = mem_rd_final;
    mem_wr[0] = mem_wr_first;
    mem_wr[DEC_MEM_PAGES - 1] = mem_wr_final;
    dev_rd[0] = dev_rd_first;
    dev_rd[DEC_IO_PORTS - 1] = dev_rd_final;
    dev_wr[0] = dev_wr_first;
    dev_wr[DEC_IO_PORTS - 1] = dev_wr_final;

    // test case: can deliver read to each mem page
    dec_test_trigger(MEMR_, 0, mem_rd_first);
    dec_test_trigger(MEMR_, 0xFFFF, mem_rd_final);

    // test case: can deliver write to each mem page
    dec_test_trigger(MEMW_, 0, mem_wr_first);
    dec_test_trigger(MEMW_, 0xFFFF, mem_wr_final);

    // test case: can deliver read to each i/o port
    dec_test_trigger(IOR_, 0, dev_rd_first);
    dec_test_trigger(IOR_, 0xFFFF, dev_rd_final);

    // test case: can deliver write to each i/o port
    dec_test_trigger(IOW_, 0, dev_wr_first);
    dec_test_trigger(IOW_, 0xFFFF, dev_wr_final);

    // turn on rom shadowing

    dec->shadow = rom_shadow;

    // test case: shadow intercepts memory reads
    dec_test_trigger(MEMR_, 0, rom_shadow);
    dec_test_trigger(MEMR_, 0xFFFF, rom_shadow);

    // test case: shadow ignores memory writes
    dec_test_trigger(MEMW_, 0, mem_wr_first);
    dec_test_trigger(MEMW_, 0xFFFF, mem_wr_final);

    // test case: shadow ignores i/o port reads
    dec_test_trigger(IOR_, 0, dev_rd_first);
    dec_test_trigger(IOR_, 0xFFFF, dev_rd_final);

    // test case: shadow ignores i/o port writes
    dec_test_trigger(IOW_, 0, dev_wr_first);
    dec_test_trigger(IOW_, 0xFFFF, dev_wr_final);
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

// decoder_bist: Power-On Self Test for the decoder code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the decoder code is
// not working correctly.

void decoder_bist()
{
    decoder_test_init();

    pEdge              *mem_rd = dec->mem_rd;
    pEdge              *mem_wr = dec->mem_wr;
    pEdge              *dev_rd = dec->dev_rd;
    pEdge              *dev_wr = dec->dev_wr;

    (void)mem_rd;
    (void)mem_wr;
    (void)dev_rd;
    (void)dev_wr;

    // test case: access via NULL edge does not crash.

    dec_test_trigger(MEMR_, 0x0000, 0);
    dec_test_trigger(MEMR_, 0xFFFF, 0);
    dec_test_trigger(MEMW_, 0x0000, 0);
    dec_test_trigger(MEMR_, 0xFFFF, 0);
    dec_test_trigger(IOR_, 0x0000, 0);
    dec_test_trigger(IOR_, 0xFFFF, 0);
    dec_test_trigger(IOW_, 0x0000, 0);
    dec_test_trigger(IOR_, 0xFFFF, 0);

    // hook up the memories and devices

    mem_rd[0] = mem_rd_first;
    mem_rd[DEC_MEM_PAGES - 1] = mem_rd_final;
    mem_wr[0] = mem_wr_first;
    mem_wr[DEC_MEM_PAGES - 1] = mem_wr_final;
    dev_rd[0] = dev_rd_first;
    dev_rd[DEC_IO_PORTS - 1] = dev_rd_final;
    dev_wr[0] = dev_wr_first;
    dev_wr[DEC_IO_PORTS - 1] = dev_wr_final;

    // test case: can deliver read to each mem page
    dec_test_trigger(MEMR_, 0, mem_rd_first);
    dec_test_trigger(MEMR_, 0xFFFF, mem_rd_final);

    // test case: can deliver write to each mem page
    dec_test_trigger(MEMW_, 0, mem_wr_first);
    dec_test_trigger(MEMW_, 0xFFFF, mem_wr_final);

    // test case: can deliver read to each i/o port
    dec_test_trigger(IOR_, 0, dev_rd_first);
    dec_test_trigger(IOR_, 0xFFFF, dev_rd_final);

    // test case: can deliver write to each i/o port
    dec_test_trigger(IOW_, 0, dev_wr_first);
    dec_test_trigger(IOW_, 0xFFFF, dev_wr_final);

    // turn on rom shadowing

    dec->shadow = rom_shadow;

    // test case: shadow intercepts memory reads
    dec_test_trigger(MEMR_, 0, rom_shadow);
    dec_test_trigger(MEMR_, 0xFFFF, rom_shadow);

    // test case: shadow ignores memory writes
    dec_test_trigger(MEMW_, 0, mem_wr_first);
    dec_test_trigger(MEMW_, 0xFFFF, mem_wr_final);

    // test case: shadow ignores i/o port reads
    dec_test_trigger(IOR_, 0, dev_rd_first);
    dec_test_trigger(IOR_, 0xFFFF, dev_rd_final);

    // test case: shadow ignores i/o port writes
    dec_test_trigger(IOW_, 0, dev_wr_first);
    dec_test_trigger(IOW_, 0xFFFF, dev_wr_final);
}

// === === === === === === === === === === === === === === === ===
//                           BENCHMARKING
// === === === === === === === === === === === === === === === ===

// decoder_bench: performance verification for the decoder code
//
// This function should be called as needed to measure the performance
// of the time critical parts of decoder.

static Tau          bench_elapsed_tau;

static void bench_decoder(void *arg)
{
    Tau                 t0 = TAU;

    (void)arg;

    dec_bench_trigger(MEMR_, 0);
    dec_bench_trigger(MEMR_, ~0);

    dec_bench_trigger(MEMW_, 0);
    dec_bench_trigger(MEMW_, ~0);

    dec_bench_trigger(IOR_, 0);
    dec_bench_trigger(IOR_, ~0);

    dec_bench_trigger(IOW_, 0);
    dec_bench_trigger(IOW_, ~0);

    bench_elapsed_tau = TAU - t0;
}

void decoder_bench()
{
    decoder_test_init();

    // hook up the memories and devices

    pEdge              *mem_rd = dec->mem_rd;
    pEdge              *mem_wr = dec->mem_wr;
    pEdge              *dev_rd = dec->dev_rd;
    pEdge              *dev_wr = dec->dev_wr;

    mem_rd[0] = mem_rd_first;
    mem_rd[DEC_MEM_PAGES - 1] = mem_rd_final;
    mem_wr[0] = mem_wr_first;
    mem_wr[DEC_MEM_PAGES - 1] = mem_wr_final;
    dev_rd[0] = dev_rd_first;
    dev_rd[DEC_IO_PORTS - 1] = dev_rd_final;
    dev_wr[0] = dev_wr_first;
    dev_wr[DEC_IO_PORTS - 1] = dev_wr_final;

    BENCH_TOP("decode");
    double              dt = RTC_ELAPSED(bench_decoder, 0);
    BENCH_VAL(dt / bench_elapsed_tau);
    BENCH_END();
}

// === === === === === === === === === === === === === === === ===
//                           TEST SUPPORT
// === === === === === === === === === === === === === === === ===

static void decoder_test_init()
{
    EDGE_INIT(MEMR_, 0);
    EDGE_INIT(MEMW_, 0);
    EDGE_INIT(IOR_, 0);
    EDGE_INIT(IOW_, 0);

    EDGE_INIT(mem_rd_first, 0);
    EDGE_INIT(mem_rd_final, 0);
    EDGE_INIT(mem_wr_first, 0);
    EDGE_INIT(mem_wr_final, 0);
    EDGE_INIT(dev_rd_first, 0);
    EDGE_INIT(dev_rd_final, 0);
    EDGE_INIT(dev_wr_first, 0);
    EDGE_INIT(dev_wr_final, 0);
    EDGE_INIT(rom_shadow, 0);

    edge_hi(MEMR_);
    edge_hi(MEMW_);
    edge_hi(IOR_);
    edge_hi(IOW_);

    edge_hi(mem_rd_first);
    edge_hi(mem_rd_final);
    edge_hi(mem_wr_first);
    edge_hi(mem_wr_final);
    edge_hi(dev_rd_first);
    edge_hi(dev_rd_final);
    edge_hi(dev_wr_first);
    edge_hi(dev_wr_final);
    edge_hi(rom_shadow);

    DECODER_INIT(dec);

    dec->ADDR = ADDR;

    dec->MEMR_ = MEMR_;
    dec->MEMW_ = MEMW_;
    dec->IOR_ = IOR_;
    dec->IOW_ = IOW_;

    decoder_linked(dec);
    decoder_invar(dec);

    // pretest and prebench invariants:
    // - MEMR MEMW IOR IOW have one entry on each list.
    ASSERT_EQ_integer(1, MEMR_->rise->len);
    ASSERT_EQ_integer(1, MEMR_->fall->len);
    ASSERT_EQ_integer(1, MEMW_->rise->len);
    ASSERT_EQ_integer(1, MEMW_->fall->len);
    ASSERT_EQ_integer(1, IOR_->rise->len);
    ASSERT_EQ_integer(1, IOR_->fall->len);
    ASSERT_EQ_integer(1, IOW_->rise->len);
    ASSERT_EQ_integer(1, IOW_->fall->len);

    // outgoing signals have no subscribers
    ASSERT_EQ_integer(0, mem_rd_first->rise->len);
    ASSERT_EQ_integer(0, mem_rd_first->fall->len);
    ASSERT_EQ_integer(0, mem_rd_final->rise->len);
    ASSERT_EQ_integer(0, mem_rd_final->fall->len);
    ASSERT_EQ_integer(0, mem_wr_first->rise->len);
    ASSERT_EQ_integer(0, mem_wr_first->fall->len);
    ASSERT_EQ_integer(0, mem_wr_final->rise->len);
    ASSERT_EQ_integer(0, mem_wr_final->fall->len);

    ASSERT_EQ_integer(0, dev_rd_first->rise->len);
    ASSERT_EQ_integer(0, dev_rd_first->fall->len);
    ASSERT_EQ_integer(0, dev_rd_final->rise->len);
    ASSERT_EQ_integer(0, dev_rd_final->fall->len);
    ASSERT_EQ_integer(0, dev_wr_first->rise->len);
    ASSERT_EQ_integer(0, dev_wr_first->fall->len);
    ASSERT_EQ_integer(0, dev_wr_final->rise->len);
    ASSERT_EQ_integer(0, dev_wr_final->fall->len);

    ASSERT_EQ_integer(0, rom_shadow->rise->len);
    ASSERT_EQ_integer(0, rom_shadow->fall->len);
}

static void check_outputs(pEdge e, Bit expected)
{
    pEdge               t;

    pEdge              *mem_rd = dec->mem_rd;
    pEdge              *mem_wr = dec->mem_wr;
    pEdge              *dev_rd = dec->dev_rd;
    pEdge              *dev_wr = dec->dev_wr;

    if (e)
        ASSERT_EQ_integer(expected, e->value);

    t = dec->shadow;
    if (t && (t != e))
        ASSERT_EQ_integer(1, t->value);

    for (unsigned page = 0; page < DEC_MEM_PAGES; ++page) {
        t = mem_rd[page];
        if (t && (t != e))
            ASSERT_EQ_integer(1, t->value);
        t = mem_wr[page];
        if (t && (t != e))
            ASSERT_EQ_integer(1, t->value);
    }

    for (unsigned dev = 0; dev < DEC_IO_PORTS; ++dev) {
        t = dev_rd[dev];
        if (t && (t != e))
            ASSERT_EQ_integer(1, t->value);
        t = dev_wr[dev];
        if (t && (t != e))
            ASSERT_EQ_integer(1, t->value);
    }
}

static void dec_test_trigger(pEdge e, Word a, pEdge o)
{
    TAU += 1;
    addr_to(ADDR, a);
    edge_lo(e);
    check_outputs(o, 0);
    TAU += 1;
    edge_hi(e);
    check_outputs(o, 1);
}

static void dec_bench_trigger(pEdge e, Word a)
{
    TAU += 1;
    addr_to(ADDR, a);
    edge_lo(e);
    TAU += 1;
    edge_hi(e);
}
