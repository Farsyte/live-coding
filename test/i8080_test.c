#include "i8080.h"
#include "clock.h"
#include "decoder.h"
#include "i8080_impl.h"
#include "i8224.h"
#include "i8228.h"
#include "ram8107x8x4.h"
#include "rom8316.h"
#include "sigtrace.h"
#include "target.h"

static void         i8080_test_init();
static void         i8080_trace_init();
static void         i8080_trace_fini();

static CpuTestSys   ts;

static const p8080  cpu = ts->cpu;
static const p8224  gen = ts->gen;
static const p8228  ctl = ts->ctl;
static const pDecoder dec = ts->dec;

static Rom8316     *rom = ts->rom;

static Ram8107x8x4 *ram = ts->ram;

static const pEdge  RESIN_ = ts->RESIN_;
static const pEdge  RDYIN = ts->RDYIN;

static const pAddr  ADDR = cpu->ADDR;
static const pData  DATA = cpu->DATA;
static const pAddr  PC = cpu->PC;
static const pData  A = cpu->A;
static const pData  B = cpu->B;
static const pData  C = cpu->C;
static const pData  D = cpu->D;
static const pData  E = cpu->E;
static const pData  H = cpu->H;
static const pData  L = cpu->L;
static const pData  IR = cpu->IR;

static const pEdge  SYNC = cpu->SYNC;
static const pEdge  DBIN = cpu->DBIN;
static const pEdge  WR_ = cpu->WR_;
static const pEdge  WAIT = cpu->WAIT;
static const pEdge  HOLD = cpu->HOLD;
static const pEdge  HLDA = cpu->HLDA;
static const pEdge  INT = cpu->INT;
static const pEdge  INTE = cpu->INTE;
static const pEdge  RESET_INT = cpu->RESET_INT;

static const pEdge  PHI1 = gen->PHI1;
static const pEdge  PHI2 = gen->PHI2;
static const pEdge  STSTB_ = gen->STSTB_;
static const pEdge  RESET = gen->RESET;
static const pEdge  READY = gen->READY;

static const pEdge  MEMR_ = ctl->MEMR_;
static const pEdge  MEMW_ = ctl->MEMW_;
static const pEdge  IOR_ = ctl->IOR_;
static const pEdge  IOW_ = ctl->IOW_;
static const pEdge  INTA_ = ctl->INTA_;

static pSigSess     ss = ts->ss;

static SigTrace     trace_ADDR;
static SigTrace     trace_DATA;

static SigTrace     trace_SYNC;
static SigTrace     trace_DBIN;
static SigTrace     trace_WR_;
static SigTrace     trace_WAIT;
static SigTrace     trace_HOLD;
static SigTrace     trace_HLDA;
static SigTrace     trace_INT;
static SigTrace     trace_INTE;

static SigTrace     trace_PHI1;
static SigTrace     trace_PHI2;
static SigTrace     trace_STSTB_;
static SigTrace     trace_RESIN_;
static SigTrace     trace_RESET;
static SigTrace     trace_RESET_INT;
static SigTrace     trace_RDYIN;
static SigTrace     trace_READY;

static SigTrace     trace_PC;
static SigTrace     trace_A;
static SigTrace     trace_B;
static SigTrace     trace_C;
static SigTrace     trace_D;
static SigTrace     trace_E;
static SigTrace     trace_H;
static SigTrace     trace_L;

static SigTrace     trace_IR;

static SigTrace     trace_MEMR_;
static SigTrace     trace_MEMW_;
static SigTrace     trace_IOR_;
static SigTrace     trace_IOW_;
static SigTrace     trace_INTA_;

// i8080_post: Power-On Self Test for the i8080 code
//
// This function should be called every time the program starts
// before any code relies on i8080 not being completely broken.

void i8080_post()
{
    i8080_test_init();
    i8080_reset_post(ts);
    i8080_mvi_post(ts);
    i8080_mov_post(ts);
    i8080_eidihlt_post(ts);     // leaves us in HLT state.
}

// i8080_bist: Power-On Self Test for the i8080 code
//
// This function should be called by the program on a run made during
// the build process, to generate a build error if the i8080 code is
// not working correctly.

void i8080_plot_sigs(SigPlot sp)
{
    sigplot_sig(sp, trace_PHI1);
    sigplot_sig(sp, trace_PHI2);
    sigplot_sig(sp, trace_RESIN_);
    sigplot_sig(sp, trace_RESET);
    sigplot_sig(sp, trace_RESET_INT);
    sigplot_sig(sp, trace_RDYIN);
    sigplot_sig(sp, trace_READY);
    sigplot_sig(sp, trace_HOLD);
    sigplot_sig(sp, trace_HLDA);
    sigplot_sig(sp, trace_PC);
    sigplot_sig(sp, trace_A);
    sigplot_sig(sp, trace_B);
    sigplot_sig(sp, trace_C);
    sigplot_sig(sp, trace_D);
    sigplot_sig(sp, trace_E);
    sigplot_sig(sp, trace_H);
    sigplot_sig(sp, trace_L);
    sigplot_sig(sp, trace_ADDR);
    sigplot_sig(sp, trace_SYNC);
    sigplot_sig(sp, trace_STSTB_);
    sigplot_sig(sp, trace_DATA);
    sigplot_sig(sp, trace_INTE);
    sigplot_sig(sp, trace_INT);
    sigplot_sig(sp, trace_DBIN);
    sigplot_sig(sp, trace_WR_);
    sigplot_sig(sp, trace_WAIT);
    sigplot_sig(sp, trace_MEMR_);
    sigplot_sig(sp, trace_MEMW_);
    sigplot_sig(sp, trace_IOR_);
    sigplot_sig(sp, trace_IOW_);
    sigplot_sig(sp, trace_INTA_);
    sigplot_sig(sp, trace_IR);
}

void i8080_bist()
{
    i8080_test_init();
    i8080_trace_init();

    i8080_reset_bist(ts);
    i8080_mvi_bist(ts);
    i8080_mov_bist(ts);
    i8080_eidihlt_bist(ts);     // leaves us in HLT state.
    i8080_trace_fini();
}

// i8080_bench: performance verification for the i8080 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of i8080.

void i8080_bench()
{
    i8080_test_init();
}

static void i8080_test_init()
{
    clock_init(18000000);
    I8080_INIT(cpu);
    I8224_INIT(gen);
    I8228_INIT(ctl);
    DECODER_INIT(dec);

    for (int chip = 0; chip < ROM_CHIPS; ++chip) {
        rom8316_init(rom[chip], format("rom%d", chip + 1));
    }

    for (int board = 0; board < RAM_BOARDS; ++board) {
        ram8107x8x4_init(ram[board], format("ram%d", board + 1));
    }

    EDGE_INIT(RESIN_, 0);
    EDGE_INIT(RDYIN, 0);

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

    for (int chip = 0; chip < ROM_CHIPS; ++chip) {
        rom[chip]->RDYIN = RDYIN;
        rom[chip]->ADDR = ADDR;
        rom[chip]->DATA = DATA;

        // each rom chip is 2 KiB
        // each mem page is 1 KiB

        int                 base = DEC_MEM_PAGES - (ROM_CHIPS - chip) * 2;
        // STUB("for chip %d, base is %d, will write %d and %d", chip, base, base + 1, base + 2);
        dec->mem_rd[base + 1] = rom[chip]->RD_;
        dec->mem_rd[base + 0] = rom[chip]->RD_;
    }

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

    dec->shadow = rom[0]->RD_;

    i8080_linked(cpu);
    i8224_linked(gen);
    i8228_linked(ctl);
    decoder_linked(dec);

    i8080_invar(cpu);
    i8224_invar(gen);
    i8228_invar(ctl);
    decoder_invar(dec);

    for (int chip = 0; chip < ROM_CHIPS; ++chip) {
        rom8316_linked(rom[chip]);
    }

    for (int board = 0; board < RAM_BOARDS; ++board) {
        ram8107x8x4_linked(ram[board]);
    }

    (void)WAIT;         // output: "we are in a wait state."
    (void)HOLD;         // input: "please hold for DMA"

    (void)INT;          // input: ... from interrupt controller
    (void)INTE;         // output: "interrutps enabled"

    (void)INTA_;        // output: "fetching interrupt opcode"
}

static void i8080_trace_init()
{

    sigsess_init(ss, "i8080_bist");

    sigtrace_init_edge(trace_PHI1, ss, PHI1);
    sigtrace_init_edge(trace_PHI2, ss, PHI2);
    sigtrace_init_edge(trace_RESIN_, ss, RESIN_);
    sigtrace_init_edge(trace_RESET, ss, RESET);
    sigtrace_init_edge(trace_RESET_INT, ss, RESET_INT);
    sigtrace_init_edge(trace_RDYIN, ss, RDYIN);
    sigtrace_init_edge(trace_READY, ss, READY);
    sigtrace_init_edge(trace_HOLD, ss, HOLD);
    sigtrace_init_edge(trace_HLDA, ss, HLDA);
    sigtrace_init_addr(trace_PC, ss, PC);
    sigtrace_init_data(trace_A, ss, A);
    sigtrace_init_data(trace_B, ss, B);
    sigtrace_init_data(trace_C, ss, C);
    sigtrace_init_data(trace_D, ss, D);
    sigtrace_init_data(trace_E, ss, E);
    sigtrace_init_data(trace_H, ss, H);
    sigtrace_init_data(trace_L, ss, L);
    sigtrace_init_addr(trace_ADDR, ss, ADDR);
    sigtrace_init_edge(trace_SYNC, ss, SYNC);
    sigtrace_init_edge(trace_STSTB_, ss, STSTB_);
    sigtrace_init_data(trace_DATA, ss, DATA);
    sigtrace_init_edge(trace_INTE, ss, INTE);
    sigtrace_init_edge(trace_INT, ss, INT);
    sigtrace_init_edge(trace_DBIN, ss, DBIN);
    sigtrace_init_edge(trace_WR_, ss, WR_);
    sigtrace_init_edge(trace_WAIT, ss, WAIT);
    sigtrace_init_edge(trace_MEMR_, ss, MEMR_);
    sigtrace_init_edge(trace_MEMW_, ss, MEMW_);
    sigtrace_init_edge(trace_IOR_, ss, IOR_);
    sigtrace_init_edge(trace_IOW_, ss, IOW_);
    sigtrace_init_edge(trace_INTA_, ss, INTA_);
    sigtrace_init_data(trace_IR, ss, IR);
}

static void i8080_trace_fini()
{
    sigtrace_fini(trace_PHI1);
    sigtrace_fini(trace_PHI2);
    sigtrace_fini(trace_RESIN_);
    sigtrace_fini(trace_RESET);
    sigtrace_fini(trace_RESET_INT);
    sigtrace_fini(trace_RDYIN);
    sigtrace_fini(trace_READY);
    sigtrace_fini(trace_HOLD);
    sigtrace_fini(trace_HLDA);
    sigtrace_fini(trace_PC);
    sigtrace_fini(trace_A);
    sigtrace_fini(trace_B);
    sigtrace_fini(trace_C);
    sigtrace_fini(trace_D);
    sigtrace_fini(trace_E);
    sigtrace_fini(trace_H);
    sigtrace_fini(trace_L);
    sigtrace_fini(trace_ADDR);
    sigtrace_fini(trace_SYNC);
    sigtrace_fini(trace_STSTB_);
    sigtrace_fini(trace_DATA);
    sigtrace_fini(trace_INTE);
    sigtrace_fini(trace_INT);
    sigtrace_fini(trace_DBIN);
    sigtrace_fini(trace_WR_);
    sigtrace_fini(trace_WAIT);
    sigtrace_fini(trace_MEMR_);
    sigtrace_fini(trace_MEMW_);
    sigtrace_fini(trace_IOR_);
    sigtrace_fini(trace_IOW_);
    sigtrace_fini(trace_INTA_);
    sigtrace_fini(trace_IR);

    sigsess_fini(ss);
}