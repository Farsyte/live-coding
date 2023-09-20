#include "decoder.h"

// decoder: efficient enables for mem pages and i/o ports
//
// This module monitors the control bus and the address bus,
// triggering edges registered for each page of memory and
// each I/O port, separately for read and write, to avoid the
// performance issues around having every memory and device
// monitoring DBIN and/or /WR.

static void         dec_memr_rise(Decoder dec);
static void         dec_memr_fall(Decoder dec);
static void         dec_memw_rise(Decoder dec);
static void         dec_memw_fall(Decoder dec);
static void         dec_ior_rise(Decoder dec);
static void         dec_ior_fall(Decoder dec);
static void         dec_iow_rise(Decoder dec);
static void         dec_iow_fall(Decoder dec);

// decoder_invar: verify the invariants for a decoder.
// - as usual, not null, has a name, is not empty.
// - connected to ADDR and the control bus
// - any non-null edge pointers are valid edges

void decoder_invar(Decoder dec)
{
    assert(dec);

    Cstr                name = dec->name;

    pEdge               MEMR_ = dec->MEMR_;
    pEdge               MEMW_ = dec->MEMW_;
    pEdge               IOR_ = dec->IOR_;
    pEdge               IOW_ = dec->IOW_;

    pEdge              *mem_rd = dec->mem_rd;
    pEdge              *mem_wr = dec->mem_wr;
    pEdge              *dev_rd = dec->dev_rd;
    pEdge              *dev_wr = dec->dev_wr;

    assert(name);
    assert(name[0]);

    assert(dec->ADDR);

    edge_invar(MEMR_);
    edge_invar(MEMW_);
    edge_invar(IOR_);
    edge_invar(IOW_);

    for (unsigned page = 0; page < DEC_MEM_PAGES; ++page) {
        if (mem_rd[page])
            edge_invar(mem_rd[page]);
        if (mem_wr[page])
            edge_invar(mem_wr[page]);
    }

    for (unsigned dev = 0; dev < DEC_IO_PORTS; ++dev) {
        if (dev_rd[dev])
            edge_invar(dev_rd[dev]);
        if (dev_wr[dev])
            edge_invar(dev_wr[dev]);
    }
}

// decoder_init(s): initialize the given decoder to have this name
// and an initial state.

void decoder_init(Decoder dec, Cstr name)
{
    assert(dec);
    assert(name);
    assert(name[0]);

    pEdge              *mem_rd = dec->mem_rd;
    pEdge              *mem_wr = dec->mem_wr;
    pEdge              *dev_rd = dec->dev_rd;
    pEdge              *dev_wr = dec->dev_wr;

    dec->name = name;

    for (unsigned page = 0; page < DEC_MEM_PAGES; ++page)
        mem_rd[page] = mem_wr[page] = 0;

    for (unsigned dev = 0; dev < DEC_IO_PORTS; ++dev)
        dev_rd[dev] = dev_wr[dev] = 0;

    dec->shadow = 0;
}

// DECODER_INIT(s): initialize the given decoder to have this name
// and an initial state.

#define DECODER_INIT(gen) decoder_init(gen, #gen)

// decoder_linked(s): finish initialization, called after pEdge fields are set.
// - requires that the control signals start high (not asserted).

void decoder_linked(Decoder dec)
{
    decoder_invar(dec);

    pEdge               MEMR_ = dec->MEMR_;
    pEdge               MEMW_ = dec->MEMW_;
    pEdge               IOR_ = dec->IOR_;
    pEdge               IOW_ = dec->IOW_;

    ASSERT_EQ_integer(1, MEMR_->value);
    ASSERT_EQ_integer(1, MEMW_->value);
    ASSERT_EQ_integer(1, IOR_->value);
    ASSERT_EQ_integer(1, IOW_->value);

    EDGE_ON_FALL(MEMR_, dec_memr_fall, dec);
    EDGE_ON_FALL(MEMW_, dec_memw_fall, dec);
    EDGE_ON_FALL(IOR_, dec_ior_fall, dec);
    EDGE_ON_FALL(IOW_, dec_iow_fall, dec);

    EDGE_ON_RISE(MEMR_, dec_memr_rise, dec);
    EDGE_ON_RISE(MEMW_, dec_memw_rise, dec);
    EDGE_ON_RISE(IOR_, dec_ior_rise, dec);
    EDGE_ON_RISE(IOW_, dec_iow_rise, dec);
}

#define MEM_PAGE_MASK   ((1u<<DEC_MEM_AS_BITS) - (1u<<DEC_MEM_PO_BITS))
#define MEM_PAGE(a)	(((a) & MEM_PAGE_MASK) >> DEC_MEM_PO_BITS)
#define DEV_PORT(a)	((a) & ((1 << DEC_DEV_AS_BITS) - 1))

static void dec_memr_rise(Decoder dec)
{
    pEdge               e = dec->shadow;
    if (!e)
        e = dec->mem_rd[MEM_PAGE(*dec->ADDR)];
    if (!e)
        return;
    edge_hi(e);
}

static void dec_memr_fall(Decoder dec)
{
    pEdge               e = dec->shadow;
    if (!e)
        e = dec->mem_rd[MEM_PAGE(*dec->ADDR)];
    if (!e)
        return;

    // force a falling edge,
    // needed during WAIT states.
    e->value = 1;
    edge_lo(e);
}

static void dec_memw_rise(Decoder dec)
{
    pEdge               e = dec->mem_wr[MEM_PAGE(*dec->ADDR)];
    if (!e)
        return;
    edge_hi(e);
}

static void dec_memw_fall(Decoder dec)
{
    pEdge               e = dec->mem_wr[MEM_PAGE(*dec->ADDR)];
    if (!e)
        return;
    // force a falling edge,
    // needed during WAIT states.
    e->value = 1;
    edge_lo(e);
}

static void dec_ior_rise(Decoder dec)
{
    pEdge               e = dec->dev_rd[DEV_PORT(*dec->ADDR)];
    if (!e)
        return;
    edge_hi(e);
}

static void dec_ior_fall(Decoder dec)
{
    pEdge               e = dec->dev_rd[DEV_PORT(*dec->ADDR)];
    if (!e)
        return;
    // force a falling edge,
    // needed during WAIT states.
    e->value = 1;
    edge_lo(e);
}

static void dec_iow_rise(Decoder dec)
{
    pEdge               e = dec->dev_wr[DEV_PORT(*dec->ADDR)];
    if (!e)
        return;
    edge_hi(e);
}

static void dec_iow_fall(Decoder dec)
{
    pEdge               e = dec->dev_wr[DEV_PORT(*dec->ADDR)];
    if (!e)
        return;
    // force a falling edge,
    // needed during WAIT states.
    e->value = 1;
    edge_lo(e);
}

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

    dec_test_trigger(MEMR_, 0, 0);
    dec_test_trigger(MEMR_, 0177777, 0);
    dec_test_trigger(MEMW_, 0, 0);
    dec_test_trigger(MEMR_, 0177777, 0);
    dec_test_trigger(IOR_, 0, 0);
    dec_test_trigger(IOR_, 0177777, 0);
    dec_test_trigger(IOW_, 0, 0);
    dec_test_trigger(IOR_, 0177777, 0);

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

    dec_test_trigger(MEMR_, 0, 0);
    dec_test_trigger(MEMR_, 0177777, 0);
    dec_test_trigger(MEMW_, 0, 0);
    dec_test_trigger(MEMR_, 0177777, 0);
    dec_test_trigger(IOR_, 0, 0);
    dec_test_trigger(IOR_, 0177777, 0);
    dec_test_trigger(IOW_, 0, 0);
    dec_test_trigger(IOR_, 0177777, 0);

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
    EDGE_INIT(MEMR_);
    EDGE_INIT(MEMW_);
    EDGE_INIT(IOR_);
    EDGE_INIT(IOW_);

    EDGE_INIT(mem_rd_first);
    EDGE_INIT(mem_rd_final);
    EDGE_INIT(mem_wr_first);
    EDGE_INIT(mem_wr_final);
    EDGE_INIT(dev_rd_first);
    EDGE_INIT(dev_rd_final);
    EDGE_INIT(dev_wr_first);
    EDGE_INIT(dev_wr_final);
    EDGE_INIT(rom_shadow);

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
    *ADDR = a;
    edge_lo(e);
    check_outputs(o, 0);
    TAU += 1;
    edge_hi(e);
    check_outputs(o, 1);
}

static void dec_bench_trigger(pEdge e, Word a)
{
    TAU += 1;
    *ADDR = a;
    edge_lo(e);
    TAU += 1;
    edge_hi(e);
}
