#!/usr/bin/env python

ob, cb = '{', '}'

rrr = [ 'B', 'C', 'D', 'E', 'H', 'L', 'M', 'A' ]

for ddd in range(8):
    for sss in range(8):
        op = 0x40 + ddd*8 + sss;
        ops = f'MOV_{rrr[ddd]}_{rrr[sss]}'
        print(f'#define {ops:15s} (0x{op:02X})')

for ddd in range(8):
    op = 0x06 + ddd*8;
    ops = f'MVI_{rrr[ddd]}'
    print(f'#define {ops:15s} (0x{op:02X})')

aluOP = ['ADD', 'ADC', 'SUB', 'SBB', 'ANA', 'XRA', 'ORA', 'CMP']
for ooo in range(8):
    for sss in range(8):
        op = 0x80 + ooo*8 + sss;
        ops = f'{aluOP[ooo]}_{rrr[sss]}'
        print(f'#define {ops:15s} (0x{op:02X})')

# 
# #define NAME_M1T4(R)	i8080_state_aluT4 ## R
# #define DEFN_M1T4(R)	static f8080State NAME_M1T4(R)
# 
# DEFN_M1T4(B);
# DEFN_M1T4(C);
# DEFN_M1T4(D);
# DEFN_M1T4(E);
# DEFN_M1T4(H);
# DEFN_M1T4(L);
# DEFN_M1T4(A);
# 
# static p8080State   i8080_state_aluT4[8] = {
#     NAME_M1T4(B),
#     NAME_M1T4(C),
#     NAME_M1T4(D),
#     NAME_M1T4(E),
#     NAME_M1T4(H),
#     NAME_M1T4(L),
#     0,
#     NAME_M1T4(A),
# };
# 
# #define NAME_M1T2(R)	i8080_state_aluT2 ## R
# #define DEFN_M1T2(R)	static f8080State NAME_M1T2(R)
# 
# DEFN_M1T2(ADD);
# DEFN_M1T2(ADC);
# DEFN_M1T2(SUB);
# DEFN_M1T2(SBB);
# DEFN_M1T2(ANA);
# DEFN_M1T2(XRA);
# DEFN_M1T2(ORA);
# DEFN_M1T2(CMP);
# 
# static p8080State   i8080_state_aluT2[8] = {
#     NAME_M1T2(ADD),
#     NAME_M1T2(ADC),
#     NAME_M1T2(SUB),
#     NAME_M1T2(SBB),
#     NAME_M1T2(ANA),
#     NAME_M1T2(XRA),
#     NAME_M1T2(ORA),
#     NAME_M1T2(CMP),
# };
