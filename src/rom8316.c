#include "chip/rom8316.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include <ctype.h>      // Functions to determine the type contained in character data
#include <errno.h>      // Macros reporting error conditions
#include <string.h>     // String handling
#include "simext/hex.h"

static void         rom8316_rd(Rom8316);
static void         rom8316_dz(Rom8316);
static void         rom_load_byte(void *ctx, unsigned addr, unsigned data);

// rom8316: 2 KiB read-only memory for 8080-like microcomputer system
//
// This module expects to connect to the Address and Data bus, and
// attach an /RD signal to the decoder, to provide 2 KiB of read-only
// memory containing preset content to the simulated system.
//
// It is notionally modelled after an 8316 chip with some extra logic
// that allows us to efficiently connect to the simulation.
//
// This module does not (yet) model the effects of failure to refresh
// each row of the memory within 

// rom8316_invar: verify the invariants for a rom8316.
// - valid pointer, valid name, not empty name.
// - valid signal exported

void rom8316_invar(Rom8316 rom)
{
    assert(rom);
    assert(rom->name);
    assert(rom->name[0]);

    assert(rom->ADDR);
    assert(rom->DATA);

    edge_invar(rom->RD_);
    edge_invar(rom->RDYIN);
}

// rom8316_init(s): initialize the given rom8316 to have this
// name and an initial state.

void rom8316_init(Rom8316 rom, Cstr name)
{
    assert(rom);
    assert(name);
    assert(name[0]);

    assert(sizeof(rom->cells) > sizeof(pByte));
    assert(sizeof(rom->cells) == ROM8316_SIZE);
    assert(rom->cells != NULL);

    rom->name = name;
    edge_init(rom->RD_, format("%s:/RD", name), 0);
    edge_hi(rom->RD_);

    rom->prev = ~0;

    rom->rombase = 0;
    rom->hexfile = 0;

    memset(rom->cells, 0xFF, sizeof(rom->cells));
}

// rom8316_load(rom, base, file): load data into rom
// parse the data in the specified file, and load into rom
// the sections that this rom, at base, would include.

void rom8316_load(Rom8316 rom, Word base, Cstr file)
{
    assert(rom);
    assert(file);
    assert(file[0]);

    rom->rombase = base;

    int                 rv = hex_parse(file, rom_load_byte, rom);
    if (rv < 0) {
        if (rv == -1) { // no bist coverage
            FAIL("hex_parse rejects file '%s'\n  error %d: %s", file, errno, strerror(errno));
            // perror(file);       // no bist coverage
        } else {
            STUB("error code %d for file '%s'", rv, file);      // no bist coverage
        }
    }
}

// rom8316_linked(s): finish initialization, called after pEdge
// fields are set.

void rom8316_linked(Rom8316 rom)
{
    assert(rom);

    edge_invar(rom->RDYIN);
    assert(0 == rom->RDYIN->value);

    EDGE_ON_FALL(rom->RD_, rom8316_rd, rom);
    EDGE_ON_RISE(rom->RD_, rom8316_dz, rom);

    rom8316_invar(rom);
}

// rom8316_rd(rom): react to the /RD falling edge.
//
// Provide data, when the address has been valid for long enough.
// For this to work, /RD must receive another falling edge.

static void rom8316_rd(Rom8316 rom)
{
    // this logic provides a single WAIT state.
    // Alternately, we could count calls to rom8316_rd
    // with the same addr, or compare TAU values since
    // the last change to decide when to produce data
    // and raise RDYIN.

    if (rom->prev != rom->ADDR->value) {
        rom->prev = rom->ADDR->value;
        edge_lo(rom->RDYIN);
    } else {
        data_to(rom->DATA, rom->cells[ROM8316_MASK & rom->ADDR->value]);
        edge_hi(rom->RDYIN);
    }
}

// rom8316_dz(rom): react to the /RD rising edge.
//
// release the data bus.

static void rom8316_dz(Rom8316 rom)
{
    data_z(rom->DATA);
}

// rom_load_byte: callback used when loading the rom.

static void rom_load_byte(void *ctx, unsigned addr, unsigned data)
{
    pRom8316            rom = (pRom8316) ctx;
    unsigned            off = addr - rom->rombase;

    if (off < ROM8316_SIZE)
        rom->cells[off] = data;
}

// rom8316_dump(rom): dump rom in "hex dump" format.

void rom8316_dump(Rom8316 rom)
{
    printf("contents of %s:\n", rom->name);
    for (unsigned addr = 0; addr < ROM8316_SIZE; addr += 16) {
        printf("%04X ", addr + rom->rombase);
        for (int i = 0; i < 16; ++i) {
            if (i == 8)
                printf(" ");
            printf(" %02X", rom->cells[addr + i]);
        }
        printf(" | ");
        for (int i = 0; i < 16; ++i) {
            if (isgraph(rom->cells[addr + i]))
                printf("%c", rom->cells[addr + i]);
            else
                printf(".");
        }
        printf(" |\n");
    }
}
