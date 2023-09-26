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
extern void         i8080_2bops_init(i8080 cpu);

extern void         i8080_mvi_init(i8080 cpu);
extern void         i8080_mov_init(i8080 cpu);
extern void         i8080_alu_init(i8080 cpu);
extern void         i8080_eidihlt_init(i8080 cpu);

extern void         i8080_reset_post(CpuTestSys ts);
extern void         i8080_mvi_post(CpuTestSys ts);
extern void         i8080_mov_post(CpuTestSys ts);
extern void         i8080_alu_post(CpuTestSys ts);
extern void         i8080_eidihlt_post(CpuTestSys ts);

extern void         i8080_reset_bist(CpuTestSys ts);
extern void         i8080_mvi_bist(CpuTestSys ts);
extern void         i8080_mov_bist(CpuTestSys ts);
extern void         i8080_alu_bist(CpuTestSys ts);
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

// use bin/gen_opcodes.py to regenerate the opcodes
// that have sss, ddd, or rp embedded in them.

#define OP_NOP          (0x00)
#define OP_MVI_B        (0x06)
#define OP_MVI_C        (0x0E)

#define OP_MVI_D        (0x16)
#define OP_MVI_E        (0x1E)

#define OP_MVI_H        (0x26)
#define OP_MVI_L        (0x2E)

#define OP_MVI_M        (0x36)
#define OP_MVI_A        (0x3E)

#define OP_MOV_B_B      (0x40)
#define OP_MOV_B_C      (0x41)
#define OP_MOV_B_D      (0x42)
#define OP_MOV_B_E      (0x43)
#define OP_MOV_B_H      (0x44)
#define OP_MOV_B_L      (0x45)
#define OP_MOV_B_M      (0x46)
#define OP_MOV_B_A      (0x47)
#define OP_MOV_C_B      (0x48)
#define OP_MOV_C_C      (0x49)
#define OP_MOV_C_D      (0x4A)
#define OP_MOV_C_E      (0x4B)
#define OP_MOV_C_H      (0x4C)
#define OP_MOV_C_L      (0x4D)
#define OP_MOV_C_M      (0x4E)
#define OP_MOV_C_A      (0x4F)

#define OP_MOV_D_B      (0x50)
#define OP_MOV_D_C      (0x51)
#define OP_MOV_D_D      (0x52)
#define OP_MOV_D_E      (0x53)
#define OP_MOV_D_H      (0x54)
#define OP_MOV_D_L      (0x55)
#define OP_MOV_D_M      (0x56)
#define OP_MOV_D_A      (0x57)
#define OP_MOV_E_B      (0x58)
#define OP_MOV_E_C      (0x59)
#define OP_MOV_E_D      (0x5A)
#define OP_MOV_E_E      (0x5B)
#define OP_MOV_E_H      (0x5C)
#define OP_MOV_E_L      (0x5D)
#define OP_MOV_E_M      (0x5E)
#define OP_MOV_E_A      (0x5F)

#define OP_MOV_H_B      (0x60)
#define OP_MOV_H_C      (0x61)
#define OP_MOV_H_D      (0x62)
#define OP_MOV_H_E      (0x63)
#define OP_MOV_H_H      (0x64)
#define OP_MOV_H_L      (0x65)
#define OP_MOV_H_M      (0x66)
#define OP_MOV_H_A      (0x67)
#define OP_MOV_L_B      (0x68)
#define OP_MOV_L_C      (0x69)
#define OP_MOV_L_D      (0x6A)
#define OP_MOV_L_E      (0x6B)
#define OP_MOV_L_H      (0x6C)
#define OP_MOV_L_L      (0x6D)
#define OP_MOV_L_M      (0x6E)
#define OP_MOV_L_A      (0x6F)

#define OP_MOV_M_B      (0x70)
#define OP_MOV_M_C      (0x71)
#define OP_MOV_M_D      (0x72)
#define OP_MOV_M_E      (0x73)
#define OP_MOV_M_H      (0x74)
#define OP_MOV_M_L      (0x75)
#define OP_HLT          (0x76)
#define OP_MOV_M_A      (0x77)
#define OP_MOV_A_B      (0x78)
#define OP_MOV_A_C      (0x79)
#define OP_MOV_A_D      (0x7A)
#define OP_MOV_A_E      (0x7B)
#define OP_MOV_A_H      (0x7C)
#define OP_MOV_A_L      (0x7D)
#define OP_MOV_A_M      (0x7E)
#define OP_MOV_A_A      (0x7F)

#define OP_ADD_B        (0x80)
#define OP_ADD_C        (0x81)
#define OP_ADD_D        (0x82)
#define OP_ADD_E        (0x83)
#define OP_ADD_H        (0x84)
#define OP_ADD_L        (0x85)
#define OP_ADD_M        (0x86)
#define OP_ADD_A        (0x87)
#define OP_ADC_B        (0x88)
#define OP_ADC_C        (0x89)
#define OP_ADC_D        (0x8A)
#define OP_ADC_E        (0x8B)
#define OP_ADC_H        (0x8C)
#define OP_ADC_L        (0x8D)
#define OP_ADC_M        (0x8E)
#define OP_ADC_A        (0x8F)

#define OP_SUB_B        (0x90)
#define OP_SUB_C        (0x91)
#define OP_SUB_D        (0x92)
#define OP_SUB_E        (0x93)
#define OP_SUB_H        (0x94)
#define OP_SUB_L        (0x95)
#define OP_SUB_M        (0x96)
#define OP_SUB_A        (0x97)
#define OP_SBB_B        (0x98)
#define OP_SBB_C        (0x99)
#define OP_SBB_D        (0x9A)
#define OP_SBB_E        (0x9B)
#define OP_SBB_H        (0x9C)
#define OP_SBB_L        (0x9D)
#define OP_SBB_M        (0x9E)
#define OP_SBB_A        (0x9F)

#define OP_ANA_B        (0xA0)
#define OP_ANA_C        (0xA1)
#define OP_ANA_D        (0xA2)
#define OP_ANA_E        (0xA3)
#define OP_ANA_H        (0xA4)
#define OP_ANA_L        (0xA5)
#define OP_ANA_M        (0xA6)
#define OP_ANA_A        (0xA7)
#define OP_XRA_B        (0xA8)
#define OP_XRA_C        (0xA9)
#define OP_XRA_D        (0xAA)
#define OP_XRA_E        (0xAB)
#define OP_XRA_H        (0xAC)
#define OP_XRA_L        (0xAD)
#define OP_XRA_M        (0xAE)
#define OP_XRA_A        (0xAF)

#define OP_ORA_B        (0xB0)
#define OP_ORA_C        (0xB1)
#define OP_ORA_D        (0xB2)
#define OP_ORA_E        (0xB3)
#define OP_ORA_H        (0xB4)
#define OP_ORA_L        (0xB5)
#define OP_ORA_M        (0xB6)
#define OP_ORA_A        (0xB7)
#define OP_CMP_B        (0xB8)
#define OP_CMP_C        (0xB9)
#define OP_CMP_D        (0xBA)
#define OP_CMP_E        (0xBB)
#define OP_CMP_H        (0xBC)
#define OP_CMP_L        (0xBD)
#define OP_CMP_M        (0xBE)
#define OP_CMP_A        (0xBF)
#define OP_DI           (0xF3)
#define OP_EI           (0xFB)
