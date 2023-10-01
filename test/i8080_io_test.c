#include "clock.h"
#include "i8080_impl.h"

// i8080_io_test: TODO write good comments

// === === === === === === === === === === === === === === === ===
//                COMMON SUPPORT FOR POST/BIST/BENCH
// === === === === === === === === === === === === === === === ===

// === === === === === === === === === === === === === === === ===
//                    POWER-ON SELF TEST SUPPORT
// === === === === === === === === === === === === === === === ===

void i8080_io_post(CpuTestSys ts)
{
    (void)ts;
    // TODO refine BIST into POST
}

// === === === === === === === === === === === === === === === ===
//                        BUILT-IN SELF TEST
// === === === === === === === === === === === === === === === ===

static f8080State   i8080_state_io_test_fail;
static f8080State   i8080_state_io_test_pass;
static f8080State   i8080_state_io_test_page;

static int          i8080_io_test_done;
static int          i8080_io_test_page;

static p8080State   saved_rst0;
static p8080State   saved_rst1;
static p8080State   saved_rst2;

typedef struct sTestdev {
    Cstr                name;

    pData               DATA;

    Edge                RD_;
    Edge                WR_;

    Data                REG;

}                  *pTestdev, Testdev[1];

static void testdev_init(Testdev dev, Cstr name)
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

static void testdev_linked(Testdev dev)
{
    EDGE_ON_FALL(dev->RD_, testdev_rd, dev);
    EDGE_ON_FALL(dev->WR_, testdev_wr, dev);
}

#define	TDWP	55
#define	TDRP	205

static SigTrace     trace_TD;

void i8080_io_bist(CpuTestSys ts)
{
    Tau                 t0;

    const p8080         cpu = ts->cpu;

    pDecoder            dec = ts->dec;
    Rom8316            *rom = ts->rom;

    pSigSess            ss = ts->ss;
    SigPlot             sp;

    pAddr               PC = cpu->PC;

    Testdev             dev;
    testdev_init(dev, "TD");

    dev->DATA = cpu->DATA;

    dec->dev_wr[TDWP] = dev->WR_;
    dec->dev_rd[TDRP] = dev->RD_;

    testdev_linked(dev);

    data_z(cpu->A);

    ASSERT_EQ_integer(0x0000, PC->value);

    // override the RST 0 and RST 1 instruction decodes
    // so we can safely use them as fenceposts.

    saved_rst0 = cpu->m1t4[OP_RST__0];
    saved_rst1 = cpu->m1t4[OP_RST__1];
    saved_rst2 = cpu->m1t4[OP_RST__2];

    cpu->m1t4[OP_RST__0] = i8080_state_io_test_fail;
    cpu->m1t4[OP_RST__1] = i8080_state_io_test_pass;
    cpu->m1t4[OP_RST__2] = i8080_state_io_test_page;

    i8080_io_test_done = 0;
    i8080_io_test_page = 0;

    int                 pgno = 0;

    rom8316_load(rom[0], 0x0000, "hex/iotest.hex");

    t0 = TAU;

    Cstr                fn;

    sigtrace_init_data(trace_TD, ss, dev->REG);

    while (i8080_io_test_done == 0) {
        clock_run_one();
        if (i8080_io_test_page) {
            if (TAU > t0) {
                fn = format("i8080_io_%d", ++pgno);

                sigplot_init(sp, ss, fn,
                             "Intel 8080 Single Chip 8-bit Microprocessor",
                             "IO instructions", t0, TAU - t0);
                i8080_plot_sigs(sp);
                sigplot_sig(sp, trace_TD);
                sigplot_fini(sp);

                t0 = TAU;
            }
            i8080_io_test_page = 0;
        }
    }

    if (TAU > t0) {
        fn = format("i8080_io_%d", ++pgno);
        sigplot_init(sp, ss, fn,
                     "Intel 8080 Single Chip 8-bit Microprocessor",
                     "IO instructions", t0, TAU - t0);
        i8080_plot_sigs(sp);
        sigplot_sig(sp, trace_TD);
        sigplot_fini(sp);
        t0 = TAU;
    }

    cpu->m1t4[OP_RST__0] = saved_rst0;
    cpu->m1t4[OP_RST__1] = saved_rst1;
    cpu->m1t4[OP_RST__2] = saved_rst2;

    sigtrace_fini(trace_TD);
}

static void i8080_state_io_test_fail(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_io_test_done = -1;
          STUB("IO test FAIL hit at at PC=%04Xh", cpu->PC->value - 1);
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_io_test_pass(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_io_test_done = 1;
          break;
      case PHI2_FALL:
          break;
    }
}

static void i8080_state_io_test_page(i8080 cpu, int phase)
{
    switch (phase) {
      case PHI1_RISE:
          edge_hi(cpu->RETM1_INT);
          break;
      case PHI2_RISE:
          i8080_io_test_page = 1;
          break;
      case PHI2_FALL:
          break;
    }

}
