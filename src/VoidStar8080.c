#include "system/VoidStar8080.h"
#include <assert.h>
#include "chip/i8080_flags.h"
#include "chip/i8080_opcodes.h"
#include "chip/i8080_status.h"
#include "common/clock.h"
#include "simext/mapdrive.h"

// VoidStar8080 boots with four drives connected.
//
//  Dsk   Capacity   Description
//   A     512,512   IBM Double Density Single Sided 8-inch diskette
//   B     256,256   IBM Single Density Single Sided 8-inch diskette
//   C   2,097,152   Fixed drive with 128 tracks of 128 sectors
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
    pDriveData          drive;
}                  *pDisk, Disk[1];

typedef struct sBctx {
    pBdev               bdev;
    Disk                disk[4];
}                  *pBctx, Bctx[1];

static void         VoidStar8080_link(VoidStar8080 sys);
static void         enable_shadow(VoidStar8080 sys);
static unsigned     disk_cap(Disk d);
static Byte        *disk_ptr(pBctx, int dsk, int trk, int sec);
static void         disk_init(Disk d, Cstr name, int drive_number, int ntrk, int nsec);
static void         VoidStar8080_seek(pBctx bctx);

static void         rom_load(VoidStar8080, Cstr basename);
static void         rom_dump(VoidStar8080);

static void         VoidStar8080_RESET_then_run_to_HLT(VoidStar8080 sys);

static int          verbose = 0;
static int          dryrun = 0;
static int          debug = 0;
static int          bus_trace = 0;
static FILE        *trace = 0;
static unsigned     debug_limit = ~0;

static void         VoidStar8080_debug(VoidStar8080 sys);
static int          VoidStar8080_debug_forth_next(VoidStar8080 sys);
static void         VoidStar8080_debug_forth_first(VoidStar8080 sys);
static Word         CFA_to_NFA(Ram8107x8x4 *ram, Word CFA);
static void         de4th_nf(Ram8107x8x4 *ram, Word NFA);
static Word         de4th_asm(Ram8107x8x4 *ram, Word scan);
static Word         de4th(Ram8107x8x4 *ram, unsigned *memcat, Word NFA, int print);

#define DEF_NFLFCFPF()                                  \
    Word                NFA = 0; (void) NFA;            \
    Word                NF0 = 0; (void) NF0;            \
    Word                NFL = 0; (void) NFL;            \
    Word                LFA = 0; (void) LFA;            \
    Word                CFA = 0; (void) CFA;            \
    Word                CODE = 0; (void) CODE;          \
    Word                PFA = 0; (void) PFA;            \
    char                NFCOPY[34]; (void) NFCOPY;      \
    Word                NFI = 0; (void) NFI

#define NFLFCFPF()                                                      \
    NF0 = memrd_byte(ram, NFA); (void) NF0;                             \
    NFL = NF0 & 0x1F; (void) NFL;                                       \
    memset(NFCOPY, 0, sizeof (NFCOPY));                                 \
    for (NFI = 0; NFI <= NFL; ++NFI)                                    \
        NFCOPY[NFI] = 0x7F & memrd_byte(ram, NFA+NFI);                  \
    LFA = NFA + 1 + NFL; (void) LFA;                                    \
    CFA = LFA + 2; (void) CFA;                                          \
    CODE = memrd_word(ram, CFA); (void) CODE;                           \
    PFA = CFA + 2; (void) PFA

#define FPRINTNF()                                                      \
    NFLFCFPF();                                                         \
    fprintf(trace, "%s", NFCOPY+1)

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
        VoidStar8080_RESET_then_run_to_HLT(sys);
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

    disk_init(bctx->disk[0], "A", 0, 77, 52);
    disk_init(bctx->disk[1], "B", 1, 77, 26);
    disk_init(bctx->disk[2], "C", 2, 128, 128);
    disk_init(bctx->disk[3], "D", 3, 256, 255);
    sys->bctx = bctx;

    for (int chip = 0; chip < ROM_CHIPS; ++chip) {
        rom8316_init(sys->rom[chip], format("%s:rom%d", name, chip + 1));
    }

    for (int board = 0; board < RAM_BOARDS; ++board) {
        ram8107x8x4_init(sys->ram[board], format("%s:ram%d", name, board + 1));
    }

    edge_init(sys->RESIN_, format("%s:/RESIN", name), 0);
    edge_init(sys->RDYIN, format("%s:RDYIN", name), 0);

    edge_init(sys->DEBUG_, format("%s:/DEBUG", name), 0);

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
    pBctx               bctx = sys->bctx;

    Rom8316            *rom = sys->rom;

    Ram8107x8x4        *ram = sys->ram;

    pEdge               RESIN_ = sys->RESIN_;
    pEdge               RDYIN = sys->RDYIN;

    pEdge               DEBUG_ = sys->DEBUG_;

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

    bctx->bdev = sys->bdev;

    BDEV_SET_SEEK(bdev, VoidStar8080_seek, bctx);

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

    dec->dev_rd[240] = DEBUG_;
    dec->dev_wr[240] = DEBUG_;

    EDGE_ON_FALL(DEBUG_, VoidStar8080_debug, sys);

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
    sys->dec->shadow = sys->rom[0]->RD_;
}

static unsigned disk_cap(Disk d)
{
    return d->ntrk * d->nsec * 128;
}

static Byte        *disk_ptr(pBctx bctx, int dsk, int trk, int sec)
{
    pDisk               disk;
    pBdev               bdev;
    pDriveData          drive;

    ASSERT(bctx, "somehow bctx is null");
    if ((dsk < 0) || (dsk >= 4))
        return NULL;    /* bad disk number */
    disk = bctx->disk[dsk];
    if ((sec < 1) || (sec > disk->nsec))
        return NULL;    /* bad sector number */
    if ((trk < 0) || (trk >= disk->ntrk))
        return NULL;    /* bad track number */
    ASSERT(disk->data, "somehow disk %s data is null", disk->name);

    bdev = bctx->bdev;
    drive = disk->drive;

    if (bdev->write_protect != drive->write_protect) {
        fprintf(stderr, "Drive %c write_protect is now %s\n",
                'A' + dsk, drive->write_protect ? "ON" : "off");
        bdev->write_protect = drive->write_protect;
    }

    return disk->data + (trk * disk->nsec + sec - 1) * BPS;
}

static void disk_init(Disk d, Cstr name, int drive_number, int ntrk, int nsec)
{
    pDriveData          dd;

    d->name = name;
    d->ntrk = ntrk;
    d->nsec = nsec;

    size_t              size = disk_cap(d);

    dd = map_drive(drive_number);
    ASSERT(NULL != dd, "unable to open media for drive %s", name);
    ASSERT((size_t)dd->data_length >= size,
           "media for drive %s has only %d of the %lu desired byte capacity.",
           name, dd->data_length, size);

    d->data = (Byte *)dd + dd->data_offset;
    d->drive = dd;

    fprintf(stderr, "Disk %s: %3d tracks, %3d sectors: %7lu bytes formatted capacity%s\n",
            name, ntrk, nsec, size, dd->write_protect ? " (WRITE PROTECT)" : "");
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

static void VoidStar8080_RESET_then_run_to_HLT(VoidStar8080 sys)
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

    while (0 == (ctl->status & STATUS_HLTA)) {

        // measure ΔRTC for each 10 seconds of simluated time
        Tau                 rtc0 = rtc_ns();
        clock_run_until(TAU + 180000000);
        Tau                 drtc = rtc_ns() - rtc0;
        double              drtc_ms = drtc / 1000000.0;
        double              ratio = 10000.0 / drtc_ms;
        if (ratio < 4.0 || ratio > 6.0) {
            fprintf(stderr, "<<running at %.1fx real time>>\n", 10000.0 / drtc_ms);
        }

    }

    clock_run_until(TAU + 45);
}

static Byte memrd_byte(Ram8107x8x4 *ram, unsigned addr)
{
    unsigned            baddr = (addr & 0xFFFF) >> RAM8107x8x4_ADDR_BITS;
    ASSERT(baddr < RAM_BOARDS, "addr 0x%04X not in ram array", addr);
    pRam8107x8x4        board = ram[baddr];
    return board->cells[addr & RAM8107x8x4_MASK];
}

static Word memrd_word(Ram8107x8x4 *ram, unsigned addr)
{
    return memrd_byte(ram, addr) | (memrd_byte(ram, addr + 1) << 8);
}

static void VoidStar8080_debug(VoidStar8080 sys)
{
    if (!debug_limit)
        return;
    --debug_limit;
    if (!debug_limit) {
        if (trace) {
            fclose(trace);
            trace = NULL;
        }
        return;
    }
    if (!trace) {
        trace = fopen("trace", "w");
    }

    p8080               cpu = sys->cpu;

    pAddr               PC = cpu->PC;
    pAddr               SP = cpu->SP;

    pData               B = cpu->B;
    pData               C = cpu->C;
    pData               D = cpu->D;
    pData               E = cpu->E;
    pData               H = cpu->H;
    pData               L = cpu->L;

    pData               A = cpu->A;
    pData               FLAGS = cpu->FLAGS;

    Byte                op = memrd_byte(sys->ram, PC->value);

    // Special case for OUT DPORT in Forth "NEXT" operation,
    // which we trigger any time an OUT DPORT happens just
    // before a PCHL instruction.

    if ((op == 0xE9) && VoidStar8080_debug_forth_next(sys))
        return;

    Cstr                as = i8080_instruction_4asm(op);

    fprintf(trace, "PC=%04X ", PC->value);
    fprintf(trace, "%02X ", op);

    if (as[0] == ' ')
        fprintf(trace, "%s", as);
    else if (as[4] == ' ')
        fprintf(trace, "0x%02X%s", memrd_byte(sys->ram, PC->value + 1), as + 4);
    else
        fprintf(trace, "0x%04X%s", memrd_word(sys->ram, PC->value + 1), as + 6);

    fprintf(trace, "SP=%04X ", SP->value);
    fprintf(trace, "BC=%02X%02X ", B->value, C->value);
    fprintf(trace, "DE=%02X%02X ", D->value, E->value);
    fprintf(trace, "HL=%02X%02X ", H->value, L->value);
    fprintf(trace, "A=%02X ", A->value);

    fprintf(trace, "FLAGS=%02X%s%s%s%s%s\n\r",
            FLAGS->value,
            FLAGS->value & FLAGS_CY ? " CY" : "",
            FLAGS->value & FLAGS_P ? " P" : "",
            FLAGS->value & FLAGS_AC ? " AC" : "",
            FLAGS->value & FLAGS_Z ? " Z" : "", FLAGS->value & FLAGS_S ? " S" : "");
    fflush(trace);
}

static unsigned find_NFA(Ram8107x8x4 *ram, unsigned NFE)
{
    unsigned            NFA = NFE;
    if (!(0x80 & memrd_byte(ram, NFA))) {
        fprintf(trace, "\r\nfind_NFA: NFE=0x%04X has %02X, MSBit is not set.\n",
                NFA, memrd_byte(ram, NFA));
        return ~0;
    }
    for (int i = 0; i < 32; ++i)
        if (0x80 & memrd_byte(ram, --NFA))
            return NFA;
    fprintf(trace,
            "\r\nfind_NFA: NFE=0x%04X has %02X, no MSBits set in prior 32 bytes, back to 0x%04X.\n",
            NFE, memrd_byte(ram, NFE), NFA);
    return ~0;
}

static int          first_forth_debug = 1;

// Dump FORTH return stack:
// -- top of stack stored at 0x0114
// -- stack pointer stored at 0x0128
// Dump the stack, top to bottom, up to 16 entries.
// return the number of characters printed.
static int dump_forth_R_stack(VoidStar8080 sys)
{
    Word                Rmax = memrd_word(sys->ram, 0x0114);
    Word                R_SP = memrd_word(sys->ram, 0x0128);
    Word                Rmin = Rmax - 32;

    // dump R stack but limit the number of entries.
    Cstr                f1 = format("{R %04X:", R_SP);
    if (R_SP < Rmin)
        R_SP = Rmin;
    for (Word at = Rmax - 2; at >= R_SP; at -= 2) {
        Cstr                f2 = format("%s %04X", f1, memrd_word(sys->ram, at));
        free((void *)f1);
        f1 = f2;
    }
    Cstr                f2 = format("%s} ", f1);
    free((void *)f1);
    int                 len = strlen(f2);
    fprintf(trace, "%s", f2);
    free((void *)f2);
    return len;
}

// Dump FORTH data stack:
// -- top of stack stored at 0x0112
// -- stack pointer in cpu->reg->SP
// Dump the stack, top to bottom, up to 16 entries.
// return the number of characters printed.
static int dump_forth_D_stack(VoidStar8080 sys)
{
    Word                Dmax = memrd_word(sys->ram, 0x0112);
    Word                D_SP = sys->cpu->SP->value;
    Word                Dmin = Dmax - 32;

    // dump D stack but limit the number of entries.
    Cstr                f1 = format("{D %04X:", D_SP);
    if (D_SP < Dmin)
        D_SP = Dmin;
    for (Word at = Dmax - 2; at >= D_SP; at -= 2) {
        Cstr                f2 = format("%s %04X", f1, memrd_word(sys->ram, at));
        free((void *)f1);
        f1 = f2;
    }
    Cstr                f2 = format("%s} ", f1);
    free((void *)f1);
    int                 len = strlen(f2);
    fprintf(trace, "%s", f2);
    free((void *)f2);
    return len;
}

static int VoidStar8080_debug_forth_next(VoidStar8080 sys)
{
    // This is called when we hit the OUT DPORT in NEXT
    // just before it does its PCHL.
    // -- 8080 BC holds the IP
    // -- 8080 DE holds the W register (PFA minus one)
    // -- 8080 HL holds the CODE address
    // -- 8080 SP holds the FORTH data stack pointer
    DEF_NFLFCFPF();

    if (first_forth_debug) {
        VoidStar8080_debug_forth_first(sys);
        first_forth_debug = 0;
    }

    Ram8107x8x4        *ram = sys->ram;

    p8080               cpu = sys->cpu;

    unsigned            B = cpu->B->value;
    unsigned            C = cpu->C->value;
    unsigned            D = cpu->D->value;
    unsigned            E = cpu->E->value;
    unsigned            H = cpu->H->value;
    unsigned            L = cpu->L->value;

    // BC contains the Forth IP register
    const unsigned      IP = (B << 8) | C;

    // HL points at the code that will run
    CODE = (H << 8) | L;

    // DE points at 2nd byte of CFA of word about to run
    const unsigned      DE = ((D << 8) | E);

    // decrement to get CFA of next word
    CFA = DE - 1;

    // back up by two to get LFA
    LFA = CFA - 2;

    // back up by one to get last byte of name field.
    const unsigned      NFE = LFA - 1;

    // Name field is variable length, ends just before LFA.
    // Name field is a control byte, followed by the name,
    // with the MSB set in the last byte of the name.
    // first byte of name field:
    //   bit 7: always set
    //   bit 6: "P" bit (precedence)
    //   bit 5: "S" bit (smuidge
    //   bit 4..0: length of the name

    if (0x00 == (0x80 & memrd_byte(ram, NFE))) {
        // fprintf(trace, " oops, last byte of NF (at 0x%04X) is 0x%02X, MSB not set.\n", NFE,
        //         memrd_byte(ram, NFE));
        return 0;
    }

    NFA = find_NFA(ram, NFE);
    if (NFA >= RAM8107x8x4_SIZE) {
        // fprintf(trace, " unable to locate start of name field ending at 0x%04X\n", NFE);
        return 0;
    }

    NFL = memrd_byte(ram, NFA) & 0x1F;
    if (NFL < 1) {
        // fprintf(trace, " name field at 0x%04X has byte 0x%02X, claiming zero length.\n",
        //         NFA, memrd_byte(ram, NFA));
        return 0;
    }
    if (NFL != NFE - NFA) {
        // fprintf(trace,
        //         " name field from 0x%04X to 0x%04X: head byte 0x%02X wants length %d, have length %d\n",
        //         NFA, NFE, memrd_byte(ram, NFA), NFL, NFE - NFA);
        return 0;
    }

    NFLFCFPF();

    int                 rcp = dump_forth_R_stack(sys);
    fprintf(trace, "  %4X %s", IP - 2, 1 + NFCOPY);

    // add padding so all the stacks line up.
    // rcp is space used by the R stack, up to 90.
    // NFL is space used by the name, up to 32.
    fprintf(trace, "%*s", 122 - rcp - NFL, "");
    dump_forth_D_stack(sys);
    fprintf(trace, "\n");

    fflush(trace);
    return 1;
}

#define MEMSIZE (1u<<16)
unsigned            memcat[MEMSIZE];
#define MEMCAT_UNCAT		0
#define MEMCAT_NF		1
#define MEMCAT_LF		2
#define MEMCAT_CF		3

#define MEMCAT_8080_ENT		5       /* 8080 machine entry point */
#define MEMCAT_8080_OP		6       /* 8080 opcode after memcat_8080_ent scanned it */

#define MEMCAT_DOCOL		10      /* colon word: parameter field is sequence of CFAs */
#define MEMCAT_DOVAR		11      /* variable: parameter field contains mutable value */
#define MEMCAT_DOCON		12      /* constant: parameter field contains immutable value */
#define MEMCAT_DODOE		13      /* word defined with "<builds"..."does>" */
#define MEMCAT_DOUSE		14      /* user variable: parameter field contains offset in user variable list */
#define MEMCAT_DOIDO		15      /* re-uses the code at IDO+2 */
#define MEMCAT_BTARG		50      /* target of a BRANCH, 0BRANCH, (LOOP), etc. */
#define MEMCAT_ENTRY		98      /* a CFA somewhere points at this byte. */
#define MEMCAT_CODE		99      /* this is a "code" word (parameter field is assembly code) */

static void memcat_init()
{
    for (unsigned i = 0; i < MEMSIZE; ++i)
        memcat[i] = MEMCAT_UNCAT;
}

Word               *estk = 0;
Word                estk_max = 0;
Word                estk_ptr = 0;

static void entry_push(Word ent)
{
    if ((memcat[ent] == MEMCAT_8080_ENT) || (memcat[ent] == MEMCAT_8080_OP))
        return;
    ASSERT((memcat[ent] == MEMCAT_UNCAT),
           "memcat[0x%04X] is already set to %d", ent, memcat[ent]);
    memcat[ent] = MEMCAT_8080_ENT;

    if (estk_ptr >= estk_max) {
        estk_max = (estk_max < 8) ? 8 : (estk_max * 2);
        estk = reallocarray(estk, estk_max, sizeof *estk);
    }
    estk[estk_ptr++] = ent;
}

static int entry_pend()
{
    return estk_ptr > 0;
}

static Word entry_pop()
{
    ASSERT(estk_ptr > 0, "ENTRY stack underflow");
    return estk[--estk_ptr];
}

static void mark_opcodes_at(Ram8107x8x4 *ram, Word ent)
{
    while (memcat[ent] != MEMCAT_8080_OP) {
        ASSERT((memcat[ent] == MEMCAT_UNCAT) ||
               (memcat[ent] == MEMCAT_8080_ENT),
               "memcat[0x%04X] is already set to %d", ent, memcat[ent]);
        memcat[ent] = MEMCAT_8080_OP;

        const Byte          op = memrd_byte(ram, ent);

        if ((op == OP_PCHL) || (op == OP_RET))
            return;

        ent += i8080_instruction_len(op);

        // some instructions have a 16-bit jump target

        switch (op) {

          case OP_JMP:
              ent = (memrd_byte(ram, ent - 1) << 8) | memrd_byte(ram, ent - 2);
              break;

          case OP_CALL:

          case OP_CNZ:
          case OP_CZ:
          case OP_CNC:
          case OP_CC:
          case OP_CPO:
          case OP_CPE:
          case OP_CP:
          case OP_CM:

          case OP_JNZ:
          case OP_JZ:
          case OP_JNC:
          case OP_JC:
          case OP_JPO:
          case OP_JPE:
          case OP_JP:
          case OP_JM:

              entry_push((memrd_byte(ram, ent - 1) << 8) | memrd_byte(ram, ent - 2));
              break;
        }
    }
}

static void mark_opcodes(Ram8107x8x4 *ram)
{
    while (entry_pend()) {
        mark_opcodes_at(ram, entry_pop());
    }
}

static void de4th_nf(Ram8107x8x4 *ram, Word scan)
{
    DEF_NFLFCFPF();
    NFA = scan;

    FPRINTNF();
    if (0x40 & memrd_byte(ram, NFA))
        fprintf(trace, " [IMMEDIATE]");
    if (0x20 & memrd_byte(ram, NFA))
        fprintf(trace, " [SMUDGE]");
}

static Word de4th_asm(Ram8107x8x4 *ram, Word scan)
{
    Byte                op = memrd_byte(ram, scan);
    Cstr                as = i8080_instruction_4asm(op);
    Word                al = i8080_instruction_len(op);

    fprintf(trace, "/%4X\t  ", scan);

    if (as[0] == ' ')
        fprintf(trace, "%s", as);
    else if (as[4] == ' ')
        fprintf(trace, "0x%02X%s", memrd_byte(ram, scan + 1), as + 4);
    else
        fprintf(trace, "0x%04X%s", memrd_word(ram, scan + 1), as + 6);

    fprintf(trace, "\t|");

    for (int i = 0; i < 3; ++i)
        if (i < al)
            fprintf(trace, " %02X", memrd_byte(ram, scan + i));
        else
            fprintf(trace, "   ");
    fprintf(trace, "\n");
    scan += al;
    return scan - 1;
}

static void VoidStar8080_debug_forth_first(VoidStar8080 sys)
{
    DEF_NFLFCFPF();

    Ram8107x8x4        *ram = sys->ram;

    memcat_init();

    // The NFA of the last word defined is stored in 010Ch.

    NFA = memrd_word(ram, 0x010C);

    while (NFA != 0) {
        NFLFCFPF();
        // during initial scan, report NFA and nanes of words found.

        assert(memcat[NFA] == MEMCAT_UNCAT);
        assert(memcat[LFA] == MEMCAT_UNCAT);
        assert(memcat[CFA] == MEMCAT_UNCAT);

        memcat[NFA] = MEMCAT_NF;
        memcat[LFA] = MEMCAT_LF;
        memcat[CFA] = MEMCAT_CF;
        if (CODE == PFA)
            memcat[CFA] = MEMCAT_CODE;

        entry_push(CODE);

        NFA = memrd_word(ram, LFA);
    }

    entry_push(0x0104); /* warm start */
    entry_push(0x0100); /* cold start */
    mark_opcodes(ram);

    // the top of the dictionary is stored in a well-known location.

    const Word          INIT_DP = memrd_word(ram, 0x011C);

    // Scan the dictionary again, re-marking bytes that CODE fields point at.

    NFA = memrd_word(ram, 0x010C);
    while (NFA != 0) {
        NFLFCFPF();
        ASSERT(memcat[NFA] == MEMCAT_NF,
               "oops, memcat[0x%04X] is %d, not %d (MEMCAT_NF)", NFA, memcat[NFA], MEMCAT_NF);

        if (CODE != PFA) {
            if ((memcat[CODE] < 128) && (memcat[CODE] != MEMCAT_ENTRY)) {
                memcat[CODE] = MEMCAT_ENTRY;
                Word                saved_NFA = NFA;
                Word                saved_CODE = CODE;
                Word                scan = CODE;
                while ((scan > 0) && (memcat[scan] != MEMCAT_NF))
                    --scan;
                if (scan) {
                    NFA = scan;
                    NFLFCFPF();
                    // fprintf(trace, "/%4X\t| DO(%s)\n", saved_CODE, NFCOPY + 1);
                    if (!strcmp(NFCOPY + 1, ":"))
                        memcat[saved_CODE] = MEMCAT_DOCOL | 128;
                    if (!strcmp(NFCOPY + 1, "VARIABLE"))
                        memcat[saved_CODE] = MEMCAT_DOVAR | 128;
                    if (!strcmp(NFCOPY + 1, "CONSTANT"))
                        memcat[saved_CODE] = MEMCAT_DOCON | 128;
                    if (!strcmp(NFCOPY + 1, "DOES>"))
                        memcat[saved_CODE] = MEMCAT_DODOE | 128;
                    if (!strcmp(NFCOPY + 1, "USER"))
                        memcat[saved_CODE] = MEMCAT_DOUSE | 128;
                    if (!strcmp(NFCOPY + 1, "I"))
                        memcat[saved_CODE] = MEMCAT_DOIDO | 128;
                } else {
                    fprintf(trace, "/%4X\t| entry point associated with no word\n", CODE);
                }
                NFA = saved_NFA;
                NFLFCFPF();
            }

            assert(memcat[CODE] & 0x80);
            memcat[CFA] = memcat[CODE] - 0x80;

        }
        NFA = memrd_word(ram, LFA);
    }

    NFA = 0;

    for (Word scan = 0x0100; scan < INIT_DP; ++scan) {

        switch (memcat[scan]) {

          case MEMCAT_UNCAT:
              fprintf(trace, "\n/%4X\tMEMCAT_UNCAT", scan);
              while (memcat[scan] == MEMCAT_UNCAT)
                  fprintf(trace, " %02X", memrd_byte(ram, scan++));
              fprintf(trace, "\n");
              --scan;
              break;

          case MEMCAT_NF:
              NFA = scan;
              NFLFCFPF();
              scan += NFL;
              break;

          case MEMCAT_LF:
              scan++;
              break;

          case MEMCAT_CF:
              fprintf(trace, "\n/%4X\tWORD ", NFA);
              de4th_nf(ram, NFA);
              fprintf(trace, "\n/%4X\t|\tCODE ==> 0x%04X [memcat %d]\n",
                      scan, memrd_word(ram, scan), memcat[memrd_word(ram, scan)]);
              scan++;
              break;

          case MEMCAT_DOCOL | 128:
          case MEMCAT_DOVAR | 128:
          case MEMCAT_DOCON | 128:
          case MEMCAT_DODOE | 128:
          case MEMCAT_DOUSE | 128:
          case MEMCAT_DOIDO | 128:
          case MEMCAT_8080_ENT:
          case MEMCAT_8080_OP:
              if (memcat[scan] != MEMCAT_8080_OP)
                  fprintf(trace, "\n");
              scan = de4th_asm(ram, scan);
              break;

          case MEMCAT_DOCOL:
              ASSERT(NFA >= 0x0100, "NFA was set to 0x%04X", NFA);
              scan = de4th(ram, memcat, NFA, 0);
              scan = de4th(ram, memcat, NFA, 1);
              NFA = 0x0000;
              break;

          case MEMCAT_DOVAR:
              fprintf(trace, "\n/%4X\tVARIABLE ", NFA);
              FPRINTNF();
              fprintf(trace, "\n");
              fprintf(trace, "/%4X\t|\t", PFA);
              scan = PFA;
              while (memcat[scan] == MEMCAT_UNCAT)
                  fprintf(trace, " %02X", memrd_byte(ram, scan++));
              fprintf(trace, "\n");
              NFA = 0x0000;
              scan--;
              break;

          case MEMCAT_DOCON:
              fprintf(trace, "\n/%4X\t0x%04X CONSTANT ", NFA, memrd_word(ram, PFA));
              FPRINTNF();
              fprintf(trace, "\n");
              scan = PFA + 1;
              NFA = 0x0000;
              ASSERT(scan >= 0x0100, "SCAN was set to 0x%04X", scan);
              break;

          case MEMCAT_DODOE:
              fprintf(trace, "\n/%4X\tDOES> ", NFA);
              FPRINTNF();
              fprintf(trace, "\n");
              NFA = 0x0000;
              scan++;
              break;

          case MEMCAT_DOUSE:
              fprintf(trace, "\n/%4X\t0x%02X USER ", NFA, memrd_byte(ram, PFA));
              FPRINTNF();
              fprintf(trace, "\n");
              NFA = 0x0000;
              scan = PFA;
              if (memcat[scan + 1] == MEMCAT_UNCAT)
                  scan++;       /* some USER words have two byte parameter fields. */
              break;

          case MEMCAT_DOIDO:
              fprintf(trace, "\n/%4X\t[DOIDO] ", NFA);
              FPRINTNF();
              fprintf(trace, "\n");
              NFA = 0x0000;
              scan++;
              break;

          case MEMCAT_CODE:
              fprintf(trace, "\n/%4X\tCODE ", NFA);
              FPRINTNF();
              fprintf(trace, "\n");
              NFA = 0x0000;
              scan++;
              break;

          case MEMCAT_BTARG:
          default:
              fprintf(trace, "\n/%4X\tMEMCAT = %d\n", scan, memcat[scan]);
              NFA = 0x0000;
              break;
        }
        ASSERT(scan >= 0x0100, "SCAN was set to 0x%04X", scan);
    }
}

static Word CFA_to_NFA(Ram8107x8x4 *ram, Word CFA)
{
    Word                LFA = CFA - 2;
    Word                NFL = LFA - 1;

//    Byte                last = memrd_byte(ram, NFL);
//    if (0x80 != (0x80 & last))
//        return ~0;

    Word                NFA = NFL - 1;
    while (0x80 != (0x80 & memrd_byte(ram, NFA)))
        NFA--;
    return NFA;
}

static Word de4th(Ram8107x8x4 *ram, unsigned *memcat, Word scan, int print)
{
    ASSERT(scan >= 0x0100, "scan was set to 0x%04X", scan);

    DEF_NFLFCFPF();
    NFA = scan;
    ASSERT(NFA >= 0x0100, "NFA was set to 0x%04X", NFA);
    NFLFCFPF();

    if (print) {
        fprintf(trace, "\n/%4X\t: ", NFA);
        de4th_nf(ram, NFA);
    }

    Word                off;
    Word                dst;
    Word                IP = PFA;
    int                 printip = 1;
    while ((memcat[IP] == MEMCAT_UNCAT) || (memcat[IP] == MEMCAT_BTARG)) {
        if (memcat[IP] == MEMCAT_BTARG)
            printip = 1;
        ASSERT(IP >= 0x0100, "SCAN was set to 0x%04X", IP);
        if (print) {
            if (printip) {
                fprintf(trace, "\n/%4X\t  ", IP);
                printip = 0;
            } else {
                fprintf(trace, " ");
            }
        }
        CFA = memrd_word(ram, IP);
        IP += 2;
        if (memcat[CFA] == MEMCAT_UNCAT) {
            if (print) {
                fprintf(trace, "[0x%04X]", CFA);
            }
        } else {
            NFA = CFA_to_NFA(ram, CFA);
            NFLFCFPF();
            if (print) {
                fprintf(trace, "%s", NFCOPY + 1);
            }

            if (!strcmp(NFCOPY + 1, ";S")) {
                // what if IP is the target of a branch from above ...?
                // see ‹T&SCALC› which branches to TSCALS at 14F3.
                // break;
                if (memcat[IP] == MEMCAT_UNCAT)
                    break;
                printip = 1;
            } else if (!strcmp(NFCOPY + 1, "COMPILE")) {
                if (print) {
                    fprintf(trace, " ");
                }
                CFA = memrd_word(ram, IP);
                if (memcat[CFA] == MEMCAT_UNCAT) {
                    if (print) {
                        fprintf(trace, "0x%04X", CFA);
                    }
                } else {
                    NFA = CFA_to_NFA(ram, CFA);
                    NFLFCFPF();
                    if (print) {
                        fprintf(trace, "%s", NFCOPY + 1);
                    }
                }
                IP += 2;
                ASSERT(IP >= 0x0100, "IP was set to 0x%04X", IP);
            } else if (!strcmp(NFCOPY + 1, "LIT")) {
                if (print) {
                    fprintf(trace, "[0x%04X]", memrd_word(ram, IP));
                }
                IP += 2;
                ASSERT(IP >= 0x0100, "IP was set to 0x%04X", IP);
            } else if (!strcmp(NFCOPY + 1, "0BRANCH")) {
                off = memrd_word(ram, IP);
                dst = IP + off;
                if (print) {
                    fprintf(trace, "[0x%04X]", dst);
                }
                memcat[dst] = MEMCAT_BTARG;
                IP += 2;
                ASSERT(IP >= 0x0100, "IP was set to 0x%04X", IP);
                printip = 1;
            } else if (!strcmp(NFCOPY + 1, "BRANCH")) {
                off = memrd_word(ram, IP);
                dst = IP + off;
                if (print) {
                    fprintf(trace, "[0x%04X]", dst);
                }
                memcat[dst] = MEMCAT_BTARG;
                IP += 2;
                ASSERT(IP >= 0x0100, "IP was set to 0x%04X", IP);
                if (memcat[IP] == MEMCAT_UNCAT)
                    break;
                printip = 1;
            } else if (!strcmp(NFCOPY + 1, "(LOOP)")) {
                off = memrd_word(ram, IP);
                dst = IP + off;
                if (print) {
                    fprintf(trace, "[0x%04X]", dst);
                }
                memcat[dst] = MEMCAT_BTARG;
                IP += 2;
                ASSERT(IP >= 0x0100, "IP was set to 0x%04X", IP);
                printip = 1;
            } else if (!strcmp(NFCOPY + 1, "(.\")")) {
                Byte                len = memrd_byte(ram, IP++);
                if (print) {
                    fprintf(trace, "\n/%4X\t  ‹", IP);
                }
                for (Byte i = 0; i < len; ++i) {
                    Byte                ch = memrd_byte(ram, IP + i);
                    ch &= 0x7F;
                    if (print) {
                        if ((ch < ' ') || (ch > '~')) {
                            fprintf(trace, "…");
                        } else {
                            fprintf(trace, "%c", ch);
                        }
                    }
                }
                if (print) {
                    fprintf(trace, "›");
                }
                IP += len;
                printip = 1;
                ASSERT(IP >= 0x0100, "IP was set to 0x%04X", IP);
            }
            assert(IP != 0);
            ASSERT(IP >= 0x0100, "IP was set to 0x%04X", IP);
        }
        ASSERT(IP >= 0x0100, "IP was set to 0x%04X", IP);
    }
    if (print) {
        fprintf(trace, "\n");
    }
    ASSERT(IP >= 0x0100, "IP was set to 0x%04X", IP);
    return IP - 1;
}
