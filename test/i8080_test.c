#include "i8080_test.h"
#include <assert.h>     // Conditionally compiled macro that compares its argument to zero

static void         i8080_test_init();
static void         i8080_trace_init();
static void         i8080_trace_fini();

static CpuTestSys   ts;

static const p8080  cpu = ts->cpu;
static const p8224  gen = ts->gen;
static const p8228  ctl = ts->ctl;
static const pDecoder dec = ts->dec;
static const pTestdev dev = ts->dev;

static Rom8316     *rom = ts->rom;

static Ram8107x8x4 *ram = ts->ram;

static const pEdge  RESIN_ = ts->RESIN_;
static const pEdge  RDYIN = ts->RDYIN;

static const pAddr  ADDR = cpu->ADDR;
static const pData  DATA = cpu->DATA;
static const pAddr  PC = cpu->PC;
static const pAddr  SP = cpu->SP;
static const pData  A = cpu->A;
static const pData  B = cpu->B;
static const pData  C = cpu->C;
static const pData  D = cpu->D;
static const pData  E = cpu->E;
static const pData  H = cpu->H;
static const pData  L = cpu->L;
static const pData  IR = cpu->IR;
static const pData  TMP = cpu->TMP;
static const pData  ACT = cpu->ACT;
static const pData  ALU = cpu->ALU;
static const pData  W = cpu->W;
static const pData  Z = cpu->Z;
static const pData  FLAGS = cpu->FLAGS;

static const pEdge  RETM1_INT = cpu->RETM1_INT;
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

static SigTrace     trace_RETM1_INT;
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
static SigTrace     trace_SP;
static SigTrace     trace_A;
static SigTrace     trace_B;
static SigTrace     trace_C;
static SigTrace     trace_D;
static SigTrace     trace_E;
static SigTrace     trace_H;
static SigTrace     trace_L;

static SigTrace     trace_IR;
static SigTrace     trace_TMP;
static SigTrace     trace_ACT;
static SigTrace     trace_ALU;
static SigTrace     trace_W;
static SigTrace     trace_Z;
static SigTrace     trace_FLAGS;

static SigTrace     trace_MEMR_;
static SigTrace     trace_MEMW_;
static SigTrace     trace_IOR_;
static SigTrace     trace_IOW_;
static SigTrace     trace_INTA_;

static SigTrace     trace_TD;

// i8080_post: Power-On Self Test for the i8080 code
//
// This function should be called every time the program starts
// before any code relies on i8080 not being completely broken.

void i8080_post()
{
    i8080_test_init();
    i8080_reset_post(ts);

    i8080_reset_for_testing(ts);
    i8080_mvi_post(ts);

    i8080_reset_for_testing(ts);
    i8080_mov_post(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_post(ts);

    i8080_reset_for_testing(ts);
    i8080_incdec_post(ts);

    i8080_reset_for_testing(ts);
    i8080_misc_post(ts);

    i8080_reset_for_testing(ts);
    i8080_lxi_post(ts);

    i8080_reset_for_testing(ts);
    i8080_asm_post(ts, "hex/jmptest.hex");

    i8080_reset_for_testing(ts);
    i8080_asm_post(ts, "hex/iotest.hex");

    i8080_reset_for_testing(ts);
    i8080_dad_post(ts);

    i8080_reset_for_testing(ts);
    i8080_movm_post(ts);

    i8080_reset_for_testing(ts);
    i8080_stack_post(ts);

    i8080_reset_for_testing(ts);
    i8080_ldst_post(ts);

    i8080_reset_for_testing(ts);
    i8080_call_post(ts);

    i8080_reset_for_testing(ts);
    i8080_ret_post(ts);

    i8080_reset_for_testing(ts);
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
    sigplot_sig(sp, trace_SP);
    sigplot_sig(sp, trace_A);
    sigplot_sig(sp, trace_B);
    sigplot_sig(sp, trace_C);
    sigplot_sig(sp, trace_D);
    sigplot_sig(sp, trace_E);
    sigplot_sig(sp, trace_H);
    sigplot_sig(sp, trace_L);
    sigplot_sig(sp, trace_ADDR);
    sigplot_sig(sp, trace_RETM1_INT);
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
    sigplot_sig(sp, trace_TMP);
    sigplot_sig(sp, trace_ACT);
    sigplot_sig(sp, trace_ALU);
    sigplot_sig(sp, trace_W);
    sigplot_sig(sp, trace_Z);
    sigplot_sig(sp, trace_FLAGS);
    sigplot_sig(sp, trace_TD);
}

void i8080_bist()
{
    i8080_test_init();
    i8080_trace_init();

    i8080_reset_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_eidihlt_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_mvi_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_mov_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_alu_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_incdec_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_misc_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_lxi_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_asm_bist(ts, "hex/jmptest.hex", "jmp");

    i8080_reset_for_testing(ts);
    i8080_asm_bist(ts, "hex/iotest.hex", "io");

    i8080_reset_for_testing(ts);
    i8080_dad_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_movm_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_stack_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_ldst_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_call_bist(ts);

    i8080_reset_for_testing(ts);
    i8080_ret_bist(ts);

    i8080_trace_fini();

    unsigned            unset_count = i8080_unimp_ops(ts->cpu);
    fprintf(stderr, "i8080: counted %u unimplemented instruction codes.\n", unset_count);

}

// i8080_bench: performance verification for the i8080 code
//
// This function should be called as needed to measure the performance
// of the time critical parts of i8080.

void i8080_bench()
{
    i8080_test_init();
}

void i8080_reset_for_testing(CpuTestSys ts)
{
    p8224               gen = ts->gen;
    pEdge               RESIN_ = ts->RESIN_;
    pEdge               RDYIN = ts->RDYIN;
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
}

// i8080_unimp_ops(i8080 cpu): count opcodes missing from m1t4 table

unsigned i8080_unimp_ops(i8080 cpu)
{
    unsigned            unset_count = 0;

    FILE               *fp = fopen("log/i8080_unimp.txt", "w");

    for (unsigned inst = 0x00; inst <= 0xFF; inst++) {
        p8080State          m1t4 = cpu->m1t4[inst];
        if (m1t4 != cpu->state_unimp)
            continue;
        fprintf(fp, "%02X %s\n", inst, i8080_instruction_name(inst));
        ++unset_count;
    }
    fclose(fp);
    return unset_count;
}

static void i8080_test_init()
{
    clock_init(18000000);
    I8080_INIT(cpu);
    I8224_INIT(gen);
    I8228_INIT(ctl);
    DECODER_INIT(dec);
    testdev_init(dev, "TD");

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

    dev->DATA = cpu->DATA;
    dec->dev_wr[TDWP] = dev->WR_;
    dec->dev_rd[TDRP] = dev->RD_;

    i8080_linked(cpu);
    i8224_linked(gen);
    i8228_linked(ctl);
    testdev_linked(dev);
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

void testdev_init(Testdev dev, Cstr name)
{
    assert(dev);
    assert(name);
    assert(name[0]);

    dev->name = name;
    edge_init(dev->RD_, format("%s:/RD", name), 1);
    edge_init(dev->WR_, format("%s:/WR", name), 1);
    data_init(dev->REG, format("%s:REG", name));
}

static void testdev_rd(Testdev dev)
{
    data_to(dev->DATA, dev->REG->value);
}

static void testdev_wr(Testdev dev)
{
    data_to(dev->REG, dev->DATA->value);
}

void testdev_linked(Testdev dev)
{
    EDGE_ON_FALL(dev->RD_, testdev_rd, dev);
    EDGE_ON_FALL(dev->WR_, testdev_wr, dev);

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
    sigtrace_init_addr(trace_SP, ss, SP);
    sigtrace_init_data(trace_A, ss, A);
    sigtrace_init_data(trace_B, ss, B);
    sigtrace_init_data(trace_C, ss, C);
    sigtrace_init_data(trace_D, ss, D);
    sigtrace_init_data(trace_E, ss, E);
    sigtrace_init_data(trace_H, ss, H);
    sigtrace_init_data(trace_L, ss, L);
    sigtrace_init_addr(trace_ADDR, ss, ADDR);
    sigtrace_init_edge(trace_RETM1_INT, ss, RETM1_INT);
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
    sigtrace_init_data(trace_TMP, ss, TMP);
    sigtrace_init_data(trace_ACT, ss, ACT);
    sigtrace_init_data(trace_ALU, ss, ALU);
    sigtrace_init_data(trace_W, ss, W);
    sigtrace_init_data(trace_Z, ss, Z);
    sigtrace_init_data(trace_FLAGS, ss, FLAGS);
    sigtrace_init_data(trace_TD, ss, dev->REG);
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
    sigtrace_fini(trace_SP);
    sigtrace_fini(trace_A);
    sigtrace_fini(trace_B);
    sigtrace_fini(trace_C);
    sigtrace_fini(trace_D);
    sigtrace_fini(trace_E);
    sigtrace_fini(trace_H);
    sigtrace_fini(trace_L);
    sigtrace_fini(trace_ADDR);
    sigtrace_fini(trace_RETM1_INT);
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
    sigtrace_fini(trace_TMP);
    sigtrace_fini(trace_ACT);
    sigtrace_fini(trace_ALU);
    sigtrace_fini(trace_W);
    sigtrace_fini(trace_Z);
    sigtrace_fini(trace_FLAGS);
    sigtrace_fini(trace_TD);

    sigsess_fini(ss);
}

void i8080_one_instruction(i8080 cpu, unsigned plus_TAU)
{
    pEdge               RETM1_INT = cpu->RETM1_INT;
    while (1 == RETM1_INT->value)
        clock_run_one();
    while (0 == RETM1_INT->value)
        clock_run_one();
    clock_run_until(TAU + 9 + plus_TAU);
}
