#pragma once
#include "decoder.h"
#include "i8080.h"
#include "i8224.h"
#include "i8228.h"
#include "ram8107x8x4.h"
#include "rom8316.h"
#include "sigtrace.h"

// i8080_impl: shared between parts of the 8080 implementation
//
// The simulation of the 8080 is divided out to a number of
// separate files, containing somewhat independent sections
// of the module which communicate through the internal state
// of the i8080 structure.

// Initialization functions for the sub-modules
// to be called after the CPU is marked as linked:

typedef struct sCpuTestSys {
    i8080               cpu;
    i8224               gen;
    i8228               ctl;
    Decoder             dec;

#define ROM_CHIPS	8
    Rom8316             rom[ROM_CHIPS];

#define RAM_BOARDS	2
    Ram8107x8x4         ram[RAM_BOARDS];

    Edge                RESIN_;                 // owned by environment
    Edge                RDYIN;                  // owned by environment

    SigSess             ss;

}                  *pCpuTestSys, CpuTestSys[1];

extern void         i8080_plot_sigs();

extern void         i8080_reset_init(i8080 cpu);
extern void         i8080_fetch_init(i8080 cpu);
extern void         i8080_eidihlt_init(i8080 cpu);
extern void         i8080_mov_init(i8080 cpu);

extern void         i8080_reset_bist(CpuTestSys ts);
extern void         i8080_mov_bist(CpuTestSys ts);
extern void         i8080_eidihlt_bist(CpuTestSys ts);

// Activity during a T-state is managed by a single
// function, which is called three times.
// - Once in response to Phi_1 rising
// - Once in response to Phi_2 rising
// - Once in response to Phi_3 rising
// Additional calls may be added in the future.
// The value passed is currently set to one of these
// three discrete values.
// The initial notion is that the T-state can be
// viewed as nine oscillator periods; if they are
// called 1 through 9, the values here are the periods
// we are entering at that edge.

#define	PHI1_RISE	1
#define	PHI2_RISE	3
#define	PHI2_FALL	7

// 8080 OP codes

#define	OP_NOP		(0x00)
#define	OP_HLT		(0x76)
#define	OP_DI		(0xF3)
#define	OP_EI		(0xFB)

#define OP_MOV_B_B      (0x40)
#define OP_MOV_C_B      (0x41)
#define OP_MOV_D_B      (0x42)
#define OP_MOV_E_B      (0x43)
#define OP_MOV_H_B      (0x44)
#define OP_MOV_L_B      (0x45)
#define OP_MOV_M_B      (0x46)
#define OP_MOV_A_B      (0x47)
#define OP_MOV_B_C      (0x48)
#define OP_MOV_C_C      (0x49)
#define OP_MOV_D_C      (0x4A)
#define OP_MOV_E_C      (0x4B)
#define OP_MOV_H_C      (0x4C)
#define OP_MOV_L_C      (0x4D)
#define OP_MOV_M_C      (0x4E)
#define OP_MOV_A_C      (0x4F)
#define OP_MOV_B_D      (0x50)
#define OP_MOV_C_D      (0x51)
#define OP_MOV_D_D      (0x52)
#define OP_MOV_E_D      (0x53)
#define OP_MOV_H_D      (0x54)
#define OP_MOV_L_D      (0x55)
#define OP_MOV_M_D      (0x56)
#define OP_MOV_A_D      (0x57)
#define OP_MOV_B_E      (0x58)
#define OP_MOV_C_E      (0x59)
#define OP_MOV_D_E      (0x5A)
#define OP_MOV_E_E      (0x5B)
#define OP_MOV_H_E      (0x5C)
#define OP_MOV_L_E      (0x5D)
#define OP_MOV_M_E      (0x5E)
#define OP_MOV_A_E      (0x5F)
#define OP_MOV_B_H      (0x60)
#define OP_MOV_C_H      (0x61)
#define OP_MOV_D_H      (0x62)
#define OP_MOV_E_H      (0x63)
#define OP_MOV_H_H      (0x64)
#define OP_MOV_L_H      (0x65)
#define OP_MOV_M_H      (0x66)
#define OP_MOV_A_H      (0x67)
#define OP_MOV_B_L      (0x68)
#define OP_MOV_C_L      (0x69)
#define OP_MOV_D_L      (0x6A)
#define OP_MOV_E_L      (0x6B)
#define OP_MOV_H_L      (0x6C)
#define OP_MOV_L_L      (0x6D)
#define OP_MOV_M_L      (0x6E)
#define OP_MOV_A_L      (0x6F)
#define OP_MOV_B_M      (0x70)
#define OP_MOV_C_M      (0x71)
#define OP_MOV_D_M      (0x72)
#define OP_MOV_E_M      (0x73)
#define OP_MOV_H_M      (0x74)
#define OP_MOV_L_M      (0x75)
//      OP_MOV_M_M      (0x76) is HLT
#define OP_MOV_A_M      (0x77)
#define OP_MOV_B_A      (0x78)
#define OP_MOV_C_A      (0x79)
#define OP_MOV_D_A      (0x7A)
#define OP_MOV_E_A      (0x7B)
#define OP_MOV_H_A      (0x7C)
#define OP_MOV_L_A      (0x7D)
#define OP_MOV_M_A      (0x7E)
#define OP_MOV_A_A      (0x7F)
