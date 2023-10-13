#include "VoidStar8080.h"
#include "clock.h"
#include "i8080_status.h"

// VoidStar8080 boots with four drives connected.
//
//  Dsk   Capacity   Description
//   A     256,256   IBM Single Density 8-inch diskette
//   B     512,512   IBM Double Density 8-inch diskette
//   C   2,109,152   "Two Meg" hard drive
//   D   8,355,840   Largest drive addressable
//
// By default, all drives are initialized with each byte
// set to a simple hash of DSK,TRK,SEC,OFF.

#define BPS		128     /* bytes per sector */

typedef struct sDisk {
    Cstr                name;
    int                 ntrk;
    int                 nsec;
    pByte               data;
}                  *pDisk, Disk[1];

typedef struct sBctx {
    pBdev               bdev;
    Disk                disk[4];
}                  *pBctx, Bctx[1];

static void         VoidStar8080_link(VoidStar8080 sys);
static void         enable_shadow(VoidStar8080 sys);
static unsigned     disk_cap(Disk d);
static Byte        *disk_ptr(pBctx, int dsk, int trk, int sec);
static void         disk_init(Disk d, Cstr name, int ntrk, int nsec);
static void         VoidStar8080_seek(pBctx bctx);

static void         rom_load(VoidStar8080, Cstr basename);
static void         rom_dump(VoidStar8080);

static void         VoidStar8080_reset_to_hlt(VoidStar8080 sys);

static int          verbose = 0;
static int          dryrun = 0;
static int          debug = 0;
static int          bus_trace = 0;

int VoidStar8080_main(int argc, char **argv)
{
    VoidStar8080        sys;

    VoidStar8080_init(sys, "(void*)8080");
    VoidStar8080_linked(sys);

    for (int argi = 1; argi < argc; ++argi) {
        Cstr                argp = argv[argi];

        if (argp[0] == '-') {
            if (argp[1] == '-') {
                STUB("TODO parse word option '%s'", argp);
                continue;
            }
            while (*++argp) {
                switch (*argp) {
                  case 'd':
                      ++debug;
                      break;
                  case 'D':
                      --debug;
                      break;
                  case 'n':
                      ++dryrun;
                      break;
                  case 'N':
                      --dryrun;
                      break;
                  case 'v':
                      ++verbose;
                      break;
                  case 'V':
                      --verbose;
                      break;
                  case 't':
                      bus_trace++;
                      break;
                  case 'T':
                      bus_trace--;
                      break;
                  default:
                      STUB("TODO parse flag option '%c'", *argp);
                }
            }
            continue;
        }

        if (!strncmp("ROM=", argp, 4)) {
            rom_load(sys, argp + 4);
            if (verbose > 1)
                rom_dump(sys);
            continue;
        }

        // TODO allow parameters to specify files to use to
        // initialize the disk images.

        STUB("unrecognized arg '%s'", argp);
    }

    // run the system until we see a HLT instruction
    if (dryrun > 0) {
        STUB("dryrun! not running the system.");
    } else {
        STUB("start run");
        VoidStar8080_reset_to_hlt(sys);
        STUB("end run");
    }

    // TODO allow parameters to specify where to store updated
    // disk images when done

    (void)argc;         // unused
    (void)argv;         // unused

    return 0;
}

void VoidStar8080_init(VoidStar8080 sys, Cstr name)
{
    ASSERT(sys, "sys must not be NULL");
    ASSERT(name, "name must not be NULL");
    ASSERT(name[0], "name must not be empty");

    sys->name = name;

    clock_init(18000000);

    i8080_init(sys->cpu, format("%s:cpu", name));
    i8224_init(sys->gen, format("%s:gen", name));
    i8228_init(sys->ctl, format("%s:ctl", name));
    decoder_init(sys->dec, format("%s:dec", name));

    cdev_init(sys->tty, format("%s:tty", name));
    cdev_init(sys->crt, format("%s:crt", name));
    cdev_init(sys->lpt, format("%s:lpt", name));

    bdev_init(sys->bdev, format("%s:bdev", name));

    pBctx               bctx = malloc(sizeof *bctx);
    disk_init(bctx->disk[0], "A", 77, 26);
    disk_init(bctx->disk[1], "B", 77, 52);
    disk_init(bctx->disk[2], "C", 128, 128);
    disk_init(bctx->disk[3], "D", 256, 255);
    sys->bctx = bctx;

    for (int chip = 0; chip < ROM_CHIPS; ++chip) {
        rom8316_init(sys->rom[chip], format("%s:rom%d", name, chip + 1));
    }

    for (int board = 0; board < RAM_BOARDS; ++board) {
        ram8107x8x4_init(sys->ram[board], format("%s:ram%d", name, board + 1));
    }

    edge_init(sys->RESIN_, format("%s:/RESIN", name), 0);
    edge_init(sys->RDYIN, format("%s:RDYIN", name), 0);

    // now set up all the links that are strictly inside VoidStar8080
    VoidStar8080_link(sys);

    // VoidStar8080 is a standalone unit that exports items but does
    // not import any items, so once we have all our owned modules
    // linked up, we should satisfy all our invariants.

    VoidStar8080_invar(sys);
}

void VoidStar8080_invar(VoidStar8080 sys)
{
    ASSERT(sys, "sys must not be NULL");
    ASSERT(sys->name, "name must not be NULL");
    ASSERT(sys->name[0], "name must not be empty");

    i8080_invar(sys->cpu);
    i8224_invar(sys->gen);
    i8228_invar(sys->ctl);
    decoder_invar(sys->dec);

    cdev_invar(sys->tty);
    cdev_invar(sys->crt);
    cdev_invar(sys->lpt);

    bdev_invar(sys->bdev);
    ASSERT(sys->bctx, "sys->bctx must not be NULL");

    for (int chip = 0; chip < ROM_CHIPS; ++chip) {
        rom8316_invar(sys->rom[chip]);
    }

    for (int board = 0; board < RAM_BOARDS; ++board) {
        ram8107x8x4_invar(sys->ram[board]);
    }

    edge_invar(sys->RESIN_);
    edge_invar(sys->RDYIN);
}

// This function is called when the simulation surrounding this sys
// has finished linking everything to (and from) us.
// Check that our invariants are still met.

void VoidStar8080_linked(VoidStar8080 sys)
{
    VoidStar8080_invar(sys);
}

// Establish internal links between components of VoidStar8080

static void saw_action(VoidStar8080 sys, Cstr wat)
{
    p8080               cpu = sys->cpu;

    pEdge               RDYIN = sys->RDYIN;

    pAddr               ADDR = cpu->ADDR;
    pData               DATA = cpu->DATA;

    pEdge               SYNC = cpu->SYNC;
    pEdge               DBIN = cpu->DBIN;
    pEdge               WR_ = cpu->WR_;
    pEdge               WAIT = cpu->WAIT;

    if (SYNC->value)
        fprintf(stderr, "%s %04XH SYNC %02X\n", wat, ADDR->value, DATA->value);
    else if (DBIN->value)
        fprintf(stderr, "%s %04XH DBIN %02X %c\n", wat, ADDR->value, DATA->value,
                RDYIN ? 'R' : 'W');
    else if (WR_->value == 0)
        fprintf(stderr, "%s %04XH /WR  %02X\n", wat, ADDR->value, DATA->value);
    else
        fprintf(stderr, "%s %04XH %02X %c%c%c%c\n",
                wat, ADDR->value, DATA->value,
                SYNC->value ? 'S' : '-',
                DBIN->value ? 'R' : '-', WR_->value ? '-' : 'W', WAIT->value ? 'X' : '-');

}

static void saw_memr_rise(VoidStar8080 sys)
{
    saw_action(sys, "mem rd ");
}
static void saw_memw_rise(VoidStar8080 sys)
{
    saw_action(sys, "mem WR ");
}
static void saw_ior_rise(VoidStar8080 sys)
{
    saw_action(sys, "i/o rd ");
}
static void saw_iow_rise(VoidStar8080 sys)
{
    saw_action(sys, "i/o WR ");
}

static void VoidStar8080_link(VoidStar8080 sys)
{
    p8080               cpu = sys->cpu;
    p8224               gen = sys->gen;
    p8228               ctl = sys->ctl;
    pDecoder            dec = sys->dec;

    pCdev               tty = sys->tty;
    pCdev               crt = sys->crt;
    pCdev               lpt = sys->lpt;

    pBdev               bdev = sys->bdev;

    Rom8316            *rom = sys->rom;

    Ram8107x8x4        *ram = sys->ram;

    pEdge               RESIN_ = sys->RESIN_;
    pEdge               RDYIN = sys->RDYIN;

    pAddr               ADDR = cpu->ADDR;
    pData               DATA = cpu->DATA;

    pEdge               SYNC = cpu->SYNC;
    pEdge               DBIN = cpu->DBIN;
    pEdge               WR_ = cpu->WR_;
    pEdge               WAIT = cpu->WAIT;
    pEdge               HOLD = cpu->HOLD;
    pEdge               HLDA = cpu->HLDA;
    pEdge               INT = cpu->INT;
    pEdge               INTE = cpu->INTE;

    pEdge               PHI1 = gen->PHI1;
    pEdge               PHI2 = gen->PHI2;
    pEdge               STSTB_ = gen->STSTB_;
    pEdge               RESET = gen->RESET;
    pEdge               READY = gen->READY;

    pEdge               MEMR_ = ctl->MEMR_;
    pEdge               MEMW_ = ctl->MEMW_;
    pEdge               IOR_ = ctl->IOR_;
    pEdge               IOW_ = ctl->IOW_;
    pEdge               INTA_ = ctl->INTA_;

    cpu->PHI1 = PHI1;
    cpu->PHI2 = PHI2;
    cpu->READY = READY;
    cpu->RESET = RESET;

    gen->OSC = CLOCK;
    gen->SYNC = SYNC;
    gen->RESIN_ = RESIN_;
    gen->RDYIN = RDYIN;

    ctl->DATA = DATA;
    ctl->STSTB_ = STSTB_;
    ctl->DBIN = DBIN;
    ctl->WR_ = WR_;
    ctl->HLDA = HLDA;

    dec->ADDR = ADDR;
    dec->MEMR_ = MEMR_;
    dec->MEMW_ = MEMW_;
    dec->IOR_ = IOR_;
    dec->IOW_ = IOW_;

    tty->DATA = DATA;
    dec->dev_rd[0] = tty->rd[0];
    dec->dev_rd[1] = tty->rd[1];
    dec->dev_wr[0] = tty->wr[0];
    dec->dev_wr[1] = tty->wr[1];

    crt->DATA = DATA;
    dec->dev_rd[2] = crt->rd[0];
    dec->dev_rd[3] = crt->rd[1];
    dec->dev_wr[2] = crt->wr[0];
    dec->dev_wr[3] = crt->wr[1];

// have not built the High Speed Paper Tape Punch/Reader yet.
//    ppt->DATA = DATA;
//    dec->dev_rd[4] = ppt->rd[0];
//    dec->dev_rd[5] = ppt->rd[1];
//    dec->dev_wr[4] = ppt->wr[0];
//    dec->dev_wr[5] = ppt->wr[1];

    lpt->DATA = DATA;
    dec->dev_rd[6] = lpt->rd[0];
    dec->dev_rd[8] = lpt->rd[1];
    dec->dev_wr[6] = lpt->wr[0];
    dec->dev_wr[8] = lpt->wr[1];

    bdev->DATA = DATA;
    dec->dev_rd[10] = bdev->RESET_;
    dec->dev_wr[10] = bdev->RESET_;
    dec->dev_rd[11] = bdev->DSK_RD_;
    dec->dev_wr[11] = bdev->DSK_WR_;
    dec->dev_rd[12] = bdev->TRK_RD_;
    dec->dev_wr[12] = bdev->TRK_WR_;
    dec->dev_rd[13] = bdev->SEC_RD_;
    dec->dev_wr[13] = bdev->SEC_WR_;
    dec->dev_rd[14] = bdev->DAT_RD_;
    dec->dev_wr[14] = bdev->DAT_WR_;

    for (int board = 0; board < RAM_BOARDS; ++board) {
        ram[board]->ADDR = ADDR;
        ram[board]->DATA = DATA;

        // each ram board is 16 KiB
        // each mem page is 1 KiB

        int                 base = board * 16;
        for (int bp = 0; bp < 16; ++bp) {
            dec->mem_rd[base + bp] = ram[board]->RD_;
            dec->mem_wr[base + bp] = ram[board]->WR_;
        }
    }

    // Hook up ROM after RAM, as their ranges overlap
    // and we want the READ selects in those ranges
    // to go to the ROM chip.

    for (int chip = 0; chip < ROM_CHIPS; ++chip) {

        rom[chip]->RDYIN = RDYIN;
        rom[chip]->ADDR = ADDR;
        rom[chip]->DATA = DATA;

        // each rom chip is 2 KiB
        // each mem page is 1 KiB

        int                 below = DEC_MEM_PAGES - chip * 2;
        dec->mem_rd[below - 1] = rom[chip]->RD_;
        dec->mem_rd[below - 2] = rom[chip]->RD_;
        dec->mem_wr[below - 1] = NULL;
        dec->mem_wr[below - 2] = NULL;
    }

    BDEV_SET_SEEK(bdev, VoidStar8080_seek, sys);

    i8080_linked(cpu);
    i8224_linked(gen);
    i8228_linked(ctl);
    decoder_linked(dec);
    cdev_linked(tty);
    cdev_linked(crt);
    cdev_linked(lpt);
    bdev_linked(bdev);

    for (int chip = 0; chip < ROM_CHIPS; ++chip) {
        rom8316_linked(rom[chip]);
    }

    for (int board = 0; board < RAM_BOARDS; ++board) {
        ram8107x8x4_linked(ram[board]);
    }

    i8080_invar(cpu);
    i8224_invar(gen);
    i8228_invar(ctl);
    decoder_invar(dec);
    cdev_invar(tty);
    cdev_invar(crt);
    cdev_invar(lpt);
    bdev_invar(bdev);

    for (int chip = 0; chip < ROM_CHIPS; ++chip) {
        rom8316_invar(rom[chip]);
    }

    for (int board = 0; board < RAM_BOARDS; ++board) {
        ram8107x8x4_invar(ram[board]);
    }

    EDGE_ON_RISE(RESET, enable_shadow, sys);
    ASSERT(RESET->value, "RESET must be high during power-up");
    enable_shadow(sys);

    cdev_conn(tty, 2501);
    cdev_conn(crt, 2502);
    cdev_conn(lpt, 2503);

    if (bus_trace > 0) {
        EDGE_FIRST_ON_RISE(MEMR_, saw_memr_rise, sys);
        EDGE_FIRST_ON_RISE(MEMW_, saw_memw_rise, sys);
        EDGE_FIRST_ON_RISE(IOR_, saw_ior_rise, sys);
        EDGE_FIRST_ON_RISE(IOW_, saw_iow_rise, sys);
    }

    // These signals ought to be hooked up to a front panel,
    // along with some way to hold RSTIN_ and READY low.

    (void)WAIT;         // output: "we are in a wait state."
    (void)HOLD;         // input: "please hold for DMA"

    (void)INT;          // input: ... from interrupt controller
    (void)INTE;         // output: "interrutps enabled"

    (void)INTA_;        // output: "fetching interrupt opcode"
}

static void enable_shadow(VoidStar8080 sys)
{
    // STUB("asserting SHADOW");
    sys->dec->shadow = sys->rom[0]->RD_;
}

static unsigned disk_cap(Disk d)
{
    return d->ntrk * d->nsec * 120;
}
static Byte        *disk_ptr(pBctx bctx, int dsk, int trk, int sec)
{
    ASSERT(bctx, "somehow bctx is null");
    if ((dsk < 0) || (dsk >= 4))
        return NULL;    /* bad disk number */
    pDisk               disk = bctx->disk[dsk];
    if ((sec < 1) || (sec > disk->nsec))
        return NULL;    /* bad sector number */
    if ((trk < 0) || (trk >= disk->ntrk))
        return NULL;    /* bad track number */
    ASSERT(disk->data, "somehow disk %s data is null", disk->name);

    return disk->data + (trk * disk->nsec + sec - 1) * BPS;
}
static void disk_init(Disk d, Cstr name, int ntrk, int nsec)
{
    d->name = name;
    d->ntrk = ntrk;
    d->nsec = nsec;

    size_t              size = disk_cap(d);
    d->data = malloc(size);
    ASSERT(d->data, "unable to allocate %lu bytes for drive %s\n", size, name);
    fprintf(stderr, "Disk %s: %3d tracks, %3d sectors: %7lu bytes formatted capacity\n",
            name, ntrk, nsec, size);
    memset(d->data, 0, size);
}

static void VoidStar8080_seek(pBctx bctx)
{
    pBdev               bdev = bctx->bdev;

    ASSERT(bdev, "somehow bdev is NULL");

    Byte                dsk = bdev->DSK->value;
    Byte                trk = bdev->TRK->value;
    Byte                sec = bdev->SEC->value;

    Byte               *cursor = disk_ptr(bctx, dsk, trk, sec);

    if (NULL == cursor) {
        bdev->cursor = NULL;
        bdev->cursor_lim = NULL;
        return;
    }

    Byte               *cursor_lim = cursor + BPS;

    int                 spt = bctx->disk[dsk]->nsec;
    if (sec < spt) {
        data_to(bdev->SEC, sec + 1);
    } else {
        data_to(bdev->SEC, 1);
        data_to(bdev->TRK, trk + 1);
    }

    bdev->cursor = cursor;
    bdev->cursor_lim = cursor_lim;
}

static void rom_load(VoidStar8080 sys, Cstr basename)
{
    rom8316_load(sys->rom[0], 0xF800, format("hex/%s.hex", basename));
}

static void rom_dump(VoidStar8080 sys)
{
    rom8316_dump(sys->rom[0]);
}

static void VoidStar8080_reset_to_hlt(VoidStar8080 sys)
{
    p8224               gen = sys->gen;
    p8228               ctl = sys->ctl;

    pEdge               RESIN_ = sys->RESIN_;
    pEdge               RDYIN = sys->RDYIN;
    pEdge               PHI2 = gen->PHI2;
    pEdge               RESET = gen->RESET;

    clock_run_one();
    edge_lo(RESIN_);
    edge_lo(RDYIN);
    clock_run_until(TAU + 9 * 4);
    while (0 == PHI2->value)
        clock_run_one();
    while (1 == PHI2->value)
        clock_run_one();
    clock_run_one();

    edge_hi(RESIN_);
    edge_hi(RDYIN);
    while (1 == RESET->value)
        clock_run_one();
    while (1 == PHI2->value)
        clock_run_one();
    clock_run_until(TAU + 11);

    while (0 == (ctl->status & STATUS_HLTA))
        clock_run_one();

    clock_run_until(TAU + 45);
}
