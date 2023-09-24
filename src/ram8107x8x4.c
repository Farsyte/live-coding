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
// - valid signals exported, in a non-invalid configuration.

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
    edge_init(ram->RD_, format("%s:/RD", name), 1);
    edge_init(ram->WR_, format("%s:/WR", name), 1);

    memset(ram->cells, 0xFF, sizeof(ram->cells));

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
    // assert (!addr_is_z(ram->ADDR));
    data_to(ram->DATA, ram->cells[RAM8107x8x4_MASK & ram->ADDR->value]);
}

static void ram8107x8x4_wr(Ram8107x8x4 ram)
{
    // assert (!addr_is_z(ram->ADDR));
    // assert (!data_is_z(ram->ADDR));
    ram->cells[RAM8107x8x4_MASK & ram->ADDR->value] = ram->DATA->value;
}
