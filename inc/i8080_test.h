#pragma once
#include "addr.h"
#include "clock.h"
#include "data.h"
#include "decoder.h"
#include "edge.h"
#include "i8080.h"
#include "i8080_impl.h"
#include "i8224.h"
#include "i8228.h"
#include "ram8107x8x4.h"
#include "rom8316.h"
#include "sigtrace.h"
#include "support.h"

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
#define ROM_CHIPS	8
    Rom8316             rom[ROM_CHIPS];

    // each Ram8107x8x4 represents 16 KiB of memory.
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

extern void         i8080_asm_bist(CpuTestSys ts, Cstr hexfile, Cstr plotname);

extern void         i8080_one_instruction(i8080 cpu, unsigned plus_TAU);

extern void         i8080_plot_sigs();

extern unsigned     i8080_unimp_ops(i8080);
extern Cstr         i8080_instruction_name(Byte);
