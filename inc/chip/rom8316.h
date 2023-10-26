#pragma once
#include "common/data.h"
#include "common/edge.h"
#include "simext/support.h"
#include "wa16/addr.h"

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

#define ROM8316_ADDR_BITS	(11)
#define	ROM8316_SIZE	(1u<<ROM8316_ADDR_BITS)
#define	ROM8316_MASK	(ROM8316_SIZE - 1)

typedef struct sRom8316 {
    Cstr                name;

    pAddr               ADDR;
    pData               DATA;
    pEdge               RDYIN;

    Edge                RD_;

    Word                prev;
    Byte                cells[ROM8316_SIZE];

    Word                rombase;
    Cstr                hexfile;
}                  *pRom8316, Rom8316[1];

// rom8316_invar: verify the invariants for a rom8316.
// - valid pointer, valid name, not empty name.
// - valid signals attached, in a non-invalid configuration.

extern void         rom8316_invar(Rom8316);

// rom8316_init(rom): initialize the given rom8316 to have this
// name and an initial state; in order to set the contents of
// the ROM we need to know our base address, and name of a file
// containing data that includes data in our range.

void                rom8316_init(Rom8316 rom, Cstr name);

// ROM8316_INIT(rom): initialize the given rom8316 to have this
// name and an initial state.

#define ROM8316_INIT(gen) 	rom8316_init(gen, #gen)

// rom8316_load(Rom8316, Word base, Cstr file): load data from hex file
extern void         rom8316_load(Rom8316 rom, Word base, Cstr file);

// ROM8316_LOAD(rom, base, name): load the rom from the file
// found in "hex/{name}.hex" for the data starting at base.

#define ROM8316_LOAD(gen, base, name) 	\
    rom8316_load(gen, base, "hex/" name ".hex")

// rom8316_linked(rom): finish initialization, called after pEdge
// fields are set.

extern void         rom8316_linked(Rom8316);

// rom8316_dump(rom): print rom contents to stdout

extern void         rom8316_dump(Rom8316 rom);

// rom8316_post: Power-On Self Test for the rom8316 code
//
// This function should be called every time the program starts
// before any code relies on rom8316 not being completely broken.

extern void         rom8316_post();

// rom8316_bist: Power-On Self Test for the rom8316 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the rom8316 code is
// not working correctly.

extern void         rom8316_bist();

// rom8316_bench: performance verification for the rom8316 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of rom8316.

extern void         rom8316_bench();
