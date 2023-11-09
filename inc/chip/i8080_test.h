#pragma once
#include "chip/decoder.h"
#include "chip/i8080.h"
#include "chip/i8080_impl.h"
#include "chip/i8224.h"
#include "chip/i8228.h"
#include "chip/ram8107x8x4.h"
#include "chip/rom8316.h"
#include "common/clock.h"
#include "common/data.h"
#include "common/edge.h"
#include "common/sigtrace.h"
#include "common/sigtrace_data.h"
#include "common/sigtrace_edge.h"
#include "simext/support.h"
#include "wa16/addr.h"
#include "wa16/sigtrace_addr.h"

// This file provides definitions used during i8080 testing to
// construct the test environment.

#define	TDWP	55
#define	TDRP	205

// Testdev is our first I/O device, which stores the last
// byte written to TDWP and reads it back via port TDRP.

typedef struct sTestdev {
    Cstr                name;

    pData               DATA;

    Edge                RD_;
    Edge                WR_;

    Data                REG;

}                  *pTestdev, Testdev[1];

typedef struct sCpuTestSys {
    i8080               cpu;
    i8224               gen;
    i8228               ctl;
    Decoder             dec;
    Testdev             dev;

    // each Rom8316 represents 2 KiB of memory.
    // rom[0] goes in the top 2 KiB (at F800H)
    // rom[1] goes in the next 2 KiB (at F000H)
    // and so on down.
#define ROM_CHIPS	8
    Rom8316             rom[ROM_CHIPS];

    // each Ram8107x8x4 represents 16 KiB of memory.
    // ram[0] goes in the bottom 16 KiB of memorty (at 0000H)
    // ram[1] goes in the next 16 KiB of memorty (at 4000H)
    // and so on, if we had more than two.
#define RAM_BOARDS	2
    Ram8107x8x4         ram[RAM_BOARDS];

    Edge                RESIN_;                 // owned by environment
    Edge                RDYIN;                  // owned by environment

    SigSess             ss;

}                  *pCpuTestSys, CpuTestSys[1];

extern void         testdev_init(Testdev dev, Cstr name);
extern void         testdev_linked(Testdev dev);

extern void         i8080_reset_for_testing(CpuTestSys ts);

extern void         i8080_reset_post(CpuTestSys ts);
extern void         i8080_eidihlt_post(CpuTestSys ts);
extern void         i8080_mvi_post(CpuTestSys ts);
extern void         i8080_mov_post(CpuTestSys ts);
extern void         i8080_alu_post(CpuTestSys ts);
extern void         i8080_incdec_post(CpuTestSys ts);
extern void         i8080_misc_post(CpuTestSys ts);
extern void         i8080_lxi_post(CpuTestSys ts);
extern void         i8080_movm_post(CpuTestSys ts);
extern void         i8080_dad_post(CpuTestSys ts);
extern void         i8080_stack_post(CpuTestSys ts);
extern void         i8080_ldst_post(CpuTestSys ts);
extern void         i8080_call_post(CpuTestSys ts);
extern void         i8080_ret_post(CpuTestSys ts);
extern void         i8080_rst_post(CpuTestSys ts);

extern void         i8080_asm_post(CpuTestSys ts, Cstr hexfile);

extern void         i8080_reset_bist(CpuTestSys ts);
extern void         i8080_eidihlt_bist(CpuTestSys ts);
extern void         i8080_mvi_bist(CpuTestSys ts);
extern void         i8080_mov_bist(CpuTestSys ts);
extern void         i8080_alu_bist(CpuTestSys ts);
extern void         i8080_incdec_bist(CpuTestSys ts);
extern void         i8080_misc_bist(CpuTestSys ts);
extern void         i8080_lxi_bist(CpuTestSys ts);
extern void         i8080_movm_bist(CpuTestSys ts);
extern void         i8080_dad_bist(CpuTestSys ts);
extern void         i8080_stack_bist(CpuTestSys ts);
extern void         i8080_ldst_bist(CpuTestSys ts);
extern void         i8080_call_bist(CpuTestSys ts);
extern void         i8080_ret_bist(CpuTestSys ts);
extern void         i8080_rst_bist(CpuTestSys ts);

extern void         i8080_asm_bist(CpuTestSys ts, Cstr hexfile, Cstr plotname);

extern void         i8080_one_instruction(i8080 cpu, unsigned plus_TAU);

extern void         i8080_plot_sigs();

extern unsigned     i8080_unimp_ops(i8080);
