#pragma once

extern Cstr         i8080_instruction_name(Byte);
extern Word         i8080_instruction_len(Byte);
extern Cstr         i8080_instruction_4asm(Byte);

// === === === === === === === === === === === === === === === ===
// 8080 op codes
// === === === === === === === === === === === === === === === ===
//
// originally generated by bin/gen_opcodes.py

#define OP_NOP          (0x00)
#define OP_LXI__BC      (0x01)
#define OP_STAX_BC      (0x02)
#define OP_INX__BC      (0x03)
#define OP_INR__B       (0x04)
#define OP_DCR__B       (0x05)
#define OP_MVI__B       (0x06)
#define OP_RLC          (0x07)

#define OP__08          (0x08)
#define OP_DAD__BC      (0x09)
#define OP_LDAX_BC      (0x0A)
#define OP_DCX__BC      (0x0B)
#define OP_INR__C       (0x0C)
#define OP_DCR__C       (0x0D)
#define OP_MVI__C       (0x0E)
#define OP_RRC          (0x0F)

#define OP__10          (0x10)
#define OP_LXI__DE      (0x11)
#define OP_STAX_DE      (0x12)
#define OP_INX__DE      (0x13)
#define OP_INR__D       (0x14)
#define OP_DCR__D       (0x15)
#define OP_MVI__D       (0x16)
#define OP_RAL          (0x17)

#define OP__18          (0x18)
#define OP_DAD__DE      (0x19)
#define OP_LDAX_DE      (0x1A)
#define OP_DCX__DE      (0x1B)
#define OP_INR__E       (0x1C)
#define OP_DCR__E       (0x1D)
#define OP_MVI__E       (0x1E)
#define OP_RAR          (0x1F)

#define OP__20          (0x20)
#define OP_LXI__HL      (0x21)
#define OP_SHLD         (0x22)
#define OP_INX__HL      (0x23)
#define OP_INR__H       (0x24)
#define OP_DCR__H       (0x25)
#define OP_MVI__H       (0x26)
#define OP_DAA          (0x27)

#define OP__28          (0x28)
#define OP_DAD__HL      (0x29)
#define OP_LHLD         (0x2A)
#define OP_DCX__HL      (0x2B)
#define OP_INR__L       (0x2C)
#define OP_DCR__L       (0x2D)
#define OP_MVI__L       (0x2E)
#define OP_CMA          (0x2F)

#define OP__30          (0x30)
#define OP_LXI__SP      (0x31)
#define OP_STA          (0x32)
#define OP_INX__SP      (0x33)
#define OP_INR__M       (0x34)
#define OP_DCR__M       (0x35)
#define OP_MVI__M       (0x36)
#define OP_STC          (0x37)

#define OP__38          (0x38)
#define OP_DAD__SP      (0x39)
#define OP_LDA          (0x3A)
#define OP_DCX__SP      (0x3B)
#define OP_INR__A       (0x3C)
#define OP_DCR__A       (0x3D)
#define OP_MVI__A       (0x3E)
#define OP_CMC          (0x3F)

#define OP_MOV__B_B     (0x40)
#define OP_MOV__B_C     (0x41)
#define OP_MOV__B_D     (0x42)
#define OP_MOV__B_E     (0x43)
#define OP_MOV__B_H     (0x44)
#define OP_MOV__B_L     (0x45)
#define OP_MOV__B_M     (0x46)
#define OP_MOV__B_A     (0x47)

#define OP_MOV__C_B     (0x48)
#define OP_MOV__C_C     (0x49)
#define OP_MOV__C_D     (0x4A)
#define OP_MOV__C_E     (0x4B)
#define OP_MOV__C_H     (0x4C)
#define OP_MOV__C_L     (0x4D)
#define OP_MOV__C_M     (0x4E)
#define OP_MOV__C_A     (0x4F)

#define OP_MOV__D_B     (0x50)
#define OP_MOV__D_C     (0x51)
#define OP_MOV__D_D     (0x52)
#define OP_MOV__D_E     (0x53)
#define OP_MOV__D_H     (0x54)
#define OP_MOV__D_L     (0x55)
#define OP_MOV__D_M     (0x56)
#define OP_MOV__D_A     (0x57)

#define OP_MOV__E_B     (0x58)
#define OP_MOV__E_C     (0x59)
#define OP_MOV__E_D     (0x5A)
#define OP_MOV__E_E     (0x5B)
#define OP_MOV__E_H     (0x5C)
#define OP_MOV__E_L     (0x5D)
#define OP_MOV__E_M     (0x5E)
#define OP_MOV__E_A     (0x5F)

#define OP_MOV__H_B     (0x60)
#define OP_MOV__H_C     (0x61)
#define OP_MOV__H_D     (0x62)
#define OP_MOV__H_E     (0x63)
#define OP_MOV__H_H     (0x64)
#define OP_MOV__H_L     (0x65)
#define OP_MOV__H_M     (0x66)
#define OP_MOV__H_A     (0x67)

#define OP_MOV__L_B     (0x68)
#define OP_MOV__L_C     (0x69)
#define OP_MOV__L_D     (0x6A)
#define OP_MOV__L_E     (0x6B)
#define OP_MOV__L_H     (0x6C)
#define OP_MOV__L_L     (0x6D)
#define OP_MOV__L_M     (0x6E)
#define OP_MOV__L_A     (0x6F)

#define OP_MOV__M_B     (0x70)
#define OP_MOV__M_C     (0x71)
#define OP_MOV__M_D     (0x72)
#define OP_MOV__M_E     (0x73)
#define OP_MOV__M_H     (0x74)
#define OP_MOV__M_L     (0x75)
#define OP_HLT          (0x76)
#define OP_MOV__M_A     (0x77)

#define OP_MOV__A_B     (0x78)
#define OP_MOV__A_C     (0x79)
#define OP_MOV__A_D     (0x7A)
#define OP_MOV__A_E     (0x7B)
#define OP_MOV__A_H     (0x7C)
#define OP_MOV__A_L     (0x7D)
#define OP_MOV__A_M     (0x7E)
#define OP_MOV__A_A     (0x7F)

#define OP_ADD__B       (0x80)
#define OP_ADD__C       (0x81)
#define OP_ADD__D       (0x82)
#define OP_ADD__E       (0x83)
#define OP_ADD__H       (0x84)
#define OP_ADD__L       (0x85)
#define OP_ADD__M       (0x86)
#define OP_ADD__A       (0x87)

#define OP_ADC__B       (0x88)
#define OP_ADC__C       (0x89)
#define OP_ADC__D       (0x8A)
#define OP_ADC__E       (0x8B)
#define OP_ADC__H       (0x8C)
#define OP_ADC__L       (0x8D)
#define OP_ADC__M       (0x8E)
#define OP_ADC__A       (0x8F)

#define OP_SUB__B       (0x90)
#define OP_SUB__C       (0x91)
#define OP_SUB__D       (0x92)
#define OP_SUB__E       (0x93)
#define OP_SUB__H       (0x94)
#define OP_SUB__L       (0x95)
#define OP_SUB__M       (0x96)
#define OP_SUB__A       (0x97)

#define OP_SBB__B       (0x98)
#define OP_SBB__C       (0x99)
#define OP_SBB__D       (0x9A)
#define OP_SBB__E       (0x9B)
#define OP_SBB__H       (0x9C)
#define OP_SBB__L       (0x9D)
#define OP_SBB__M       (0x9E)
#define OP_SBB__A       (0x9F)

#define OP_ANA__B       (0xA0)
#define OP_ANA__C       (0xA1)
#define OP_ANA__D       (0xA2)
#define OP_ANA__E       (0xA3)
#define OP_ANA__H       (0xA4)
#define OP_ANA__L       (0xA5)
#define OP_ANA__M       (0xA6)
#define OP_ANA__A       (0xA7)

#define OP_XRA__B       (0xA8)
#define OP_XRA__C       (0xA9)
#define OP_XRA__D       (0xAA)
#define OP_XRA__E       (0xAB)
#define OP_XRA__H       (0xAC)
#define OP_XRA__L       (0xAD)
#define OP_XRA__M       (0xAE)
#define OP_XRA__A       (0xAF)

#define OP_ORA__B       (0xB0)
#define OP_ORA__C       (0xB1)
#define OP_ORA__D       (0xB2)
#define OP_ORA__E       (0xB3)
#define OP_ORA__H       (0xB4)
#define OP_ORA__L       (0xB5)
#define OP_ORA__M       (0xB6)
#define OP_ORA__A       (0xB7)

#define OP_CMP__B       (0xB8)
#define OP_CMP__C       (0xB9)
#define OP_CMP__D       (0xBA)
#define OP_CMP__E       (0xBB)
#define OP_CMP__H       (0xBC)
#define OP_CMP__L       (0xBD)
#define OP_CMP__M       (0xBE)
#define OP_CMP__A       (0xBF)

#define OP_RNZ          (0xC0)
#define OP_POP__BC      (0xC1)
#define OP_JNZ          (0xC2)
#define OP_JMP          (0xC3)
#define OP_CNZ          (0xC4)
#define OP_PUSH_BC      (0xC5)
#define OP_ADI          (0xC6)
#define OP_RST__0       (0xC7)

#define OP_RZ           (0xC8)
#define OP_RET          (0xC9)
#define OP_JZ           (0xCA)
#define OP__CB          (0xCB)
#define OP_CZ           (0xCC)
#define OP_CALL         (0xCD)
#define OP_ACI          (0xCE)
#define OP_RST__1       (0xCF)

#define OP_RNC          (0xD0)
#define OP_POP__DE      (0xD1)
#define OP_JNC          (0xD2)
#define OP_OUT          (0xD3)
#define OP_CNC          (0xD4)
#define OP_PUSH_DE      (0xD5)
#define OP_SUI          (0xD6)
#define OP_RST__2       (0xD7)

#define OP_RC           (0xD8)
#define OP__D9          (0xD9)
#define OP_JC           (0xDA)
#define OP_IN           (0xDB)
#define OP_CC           (0xDC)
#define OP__DD          (0xDD)
#define OP_SBI          (0xDE)
#define OP_RST__3       (0xDF)

#define OP_RPO          (0xE0)
#define OP_POP__HL      (0xE1)
#define OP_JPO          (0xE2)
#define OP_XTHL         (0xE3)
#define OP_CPO          (0xE4)
#define OP_PUSH_HL      (0xE5)
#define OP_ANI          (0xE6)
#define OP_RST__4       (0xE7)

#define OP_RPE          (0xE8)
#define OP_PCHL         (0xE9)
#define OP_JPE          (0xEA)
#define OP_XCHG         (0xEB)
#define OP_CPE          (0xEC)
#define OP__ED          (0xED)
#define OP_XRI          (0xEE)
#define OP_RST__5       (0xEF)

#define OP_RP           (0xF0)
#define OP_POP__PSW     (0xF1)
#define OP_JP           (0xF2)
#define OP_DI           (0xF3)
#define OP_CP           (0xF4)
#define OP_PUSH_PSW     (0xF5)
#define OP_ORI          (0xF6)
#define OP_RST__6       (0xF7)

#define OP_RM           (0xF8)
#define OP_SPHL         (0xF9)
#define OP_JM           (0xFA)
#define OP_EI           (0xFB)
#define OP_CM           (0xFC)
#define OP__FD          (0xFD)
#define OP_CPI          (0xFE)
#define OP_RST__7       (0xFF)
