#include "decoder.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero

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
            edge_invar(dev_rd[dev]);    // no bist coverage
        if (dev_wr[dev])
            edge_invar(dev_wr[dev]);    // no bist coverage
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
    if (!e) {
        Word                addr = dec->ADDR->value;
        Word                page = MEM_PAGE(addr);
        e = dec->mem_rd[page];
    }
    if (!e)
        return;
    edge_hi(e);
}

static void dec_memr_fall(Decoder dec)
{
    pEdge               e = dec->shadow;
    if (!e) {
        Word                addr = dec->ADDR->value;
        Word                page = MEM_PAGE(addr);
        e = dec->mem_rd[page];
    }
    if (!e)
        return;

    // force a falling edge,
    // needed during WAIT states.
    e->value = 1;
    edge_lo(e);
}

static void dec_memw_rise(Decoder dec)
{
    Word                addr = dec->ADDR->value;
    Word                page = MEM_PAGE(addr);
    pEdge               e = dec->mem_wr[page];

    if (!e)
        return;
    edge_hi(e);
}

static void dec_memw_fall(Decoder dec)
{
    Word                addr = dec->ADDR->value;
    Word                page = MEM_PAGE(addr);
    pEdge               e = dec->mem_wr[page];
    if (!e)
        return;
    // force a falling edge,
    // needed during WAIT states.
    e->value = 1;
    edge_lo(e);
}

static void dec_ior_rise(Decoder dec)
{
    Word                addr = dec->ADDR->value;
    Byte                port = DEV_PORT(addr);
    pEdge               e = dec->dev_rd[port];
    if (!e)
        return;
    edge_hi(e);
}

static void dec_ior_fall(Decoder dec)
{
    Word                addr = dec->ADDR->value;
    Byte                port = DEV_PORT(addr);
    pEdge               e = dec->dev_rd[port];
    if (!e)
        return;
    // force a falling edge,
    // needed during WAIT states.
    e->value = 1;
    edge_lo(e);
}

static void dec_iow_rise(Decoder dec)
{
    Word                addr = dec->ADDR->value;
    Byte                port = DEV_PORT(addr);
    pEdge               e = dec->dev_wr[port];
    if (!e)
        return;
    edge_hi(e);
}

static void dec_iow_fall(Decoder dec)
{
    Word                addr = dec->ADDR->value;
    Byte                port = DEV_PORT(addr);
    pEdge               e = dec->dev_wr[port];
    if (!e)
        return;
    // force a falling edge,
    // needed during WAIT states.
    e->value = 1;
    edge_lo(e);
}
