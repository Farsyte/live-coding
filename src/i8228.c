#include "i8228.h"
#include "sigtrace.h"

static void         latch_status(i8228 ctl);
static void         start_reads(i8228 ctl);
static void         start_writes(i8228 ctl);
static void         release_ctls(i8228 ctl);

// i8228_invar: verify the invariants for a i8228.

void i8228_invar(i8228 ctl)
{
    assert(ctl);
    assert(ctl->name);
    assert(ctl->name[0]);

    assert(ctl->DATA);

    edge_invar(ctl->STSTB_);
    edge_invar(ctl->DBIN);
    edge_invar(ctl->WR_);
    edge_invar(ctl->HLDA);

    edge_invar(ctl->MEMR_);
    edge_invar(ctl->MEMW_);
    edge_invar(ctl->IOR_);
    edge_invar(ctl->IOW_);
    edge_invar(ctl->INTA_);

}

// i8228_init(s): initialize the given i8228 to have this name
// and an initial state.

void i8228_init(i8228 ctl, Cstr name)
{
    assert(ctl);
    assert(name);
    assert(name[0]);

    ctl->name = name;

    pEdge               MEMR_ = ctl->MEMR_;
    pEdge               MEMW_ = ctl->MEMW_;
    pEdge               IOR_ = ctl->IOR_;
    pEdge               IOW_ = ctl->IOW_;
    pEdge               INTA_ = ctl->INTA_;

    edge_init(MEMR_, format("%s:/MEMR", name), 1);
    edge_init(MEMW_, format("%s:/MEMW", name), 1);
    edge_init(IOR_, format("%s:/IOR", name), 1);
    edge_init(IOW_, format("%s:/IOW", name), 1);
    edge_init(INTA_, format("%s:/INTA", name), 1);

    ctl->status = STATUS_RESET;
    ctl->activated = 0;
}

// i8228_linked(s): inputs are linked, finish set-up.

void i8228_linked(i8228 ctl)
{
    i8228_invar(ctl);

    pEdge               STSTB_ = ctl->STSTB_;
    pEdge               DBIN = ctl->DBIN;
    pEdge               WR_ = ctl->WR_;
    pEdge               HLDA = ctl->HLDA;

    assert(0 == STSTB_->value);
    assert(0 == DBIN->value);
    assert(1 == WR_->value);
    assert(0 == HLDA->value);

    EDGE_ON_FALL(STSTB_, latch_status, ctl);

    EDGE_ON_RISE(DBIN, start_reads, ctl);
    EDGE_ON_FALL(WR_, start_writes, ctl);

    EDGE_ON_FALL(DBIN, release_ctls, ctl);
    EDGE_ON_RISE(WR_, release_ctls, ctl);
    EDGE_ON_RISE(HLDA, release_ctls, ctl);
}

// latch_status(ctl): receive a new Status byte.
//
// copy the contents of the data bus into the internal latch,
// then initiate updates to the read control signals.

static void latch_status(i8228 ctl)
{
    ctl->status = ctl->DATA->value;
}

// start_reads(ctl): activate the "read" cycle controls
//
// Activate the "read" control signals that are appropriate
// based on the latched status byte.

static void start_reads(i8228 ctl)
{
    pEdge               activated = 0;
    Byte                status = ctl->status;

    if ((status & STATUS_MEMR) && !(status & STATUS_HLTA))
        activated = ctl->MEMR_;
    else if (status & STATUS_INP)
        activated = ctl->IOR_;
    else if (status & STATUS_INTA)
        activated = ctl->INTA_;
    else
        return;         // no bist coverage

    ctl->activated = activated;
    activated->value = 1;
    edge_lo(activated);
}

// start_writes(ctl): activate the "write" cycle controls
//
// Activate the "write" control signals that are appropriate
// based on the latched status byte.

static void start_writes(i8228 ctl)
{
    pEdge               activated;
    if (ctl->status & STATUS_OUT)
        activated = ctl->IOW_;
    else
        activated = ctl->MEMW_;
    ctl->activated = activated;
    activated->value = 1;
    edge_lo(activated);
}

static void release_ctls(i8228 ctl)
{
    pEdge               activated = ctl->activated;
    if (activated) {
        ctl->activated = 0;
        ctl->status = STATUS_RESET;
        edge_hi(activated);
    }
}
