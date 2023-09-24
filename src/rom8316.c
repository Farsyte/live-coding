#include "rom8316.h"
#include "hex.h"

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

    memset(rom->cells, 0xFF, sizeof(rom->cells));

    int                 rv = hex_parse(file, rom_load_byte, rom);
    if (rv < 0) {
        if (rv == -1) {
            perror(file);
        } else {
            STUB("error code %d for file '%s'", rv, file);
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
