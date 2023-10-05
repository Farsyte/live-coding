#!/usr/bin/env python

ob, cb = '{', '}'

def fix(x):
    return x.upper() if x.isalpha() or x.isdigit() else ' '

def tomac(s):
    s = "".join([fix(x) for x in s])
    s = s.strip();
    s = "".join(['_' if x == ' ' else x for x in s])
    return s
      

ops = ['---- ---' for i in range(256)]
rrr = [ 'B', 'C', 'D', 'E', 'H', 'L', None, 'A' ]
rr = [ 'BC', 'DE', 'HL', 'SP' ]
aluOP = ['add', 'adc', 'sub', 'sbb', 'ana', 'xra', 'ora', 'cmp']
aliOP = ['adi', 'aci', 'sui', 'sbi', 'ani', 'xri', 'ori', 'cpi']
cond = ['nz','z','nc','c','po','pe','p','m']


for ddd in range(8):
    if rrr[ddd] is not None:
        for sss in range(8):
            if rrr[sss] is not None:
                op = 0x40 + ddd*8 + sss;
                ops[op] = f'mov  {rrr[ddd]},{rrr[sss]}'

for ddd in range(8):
    if rrr[ddd] is not None:
        op = 0x46 + ddd*8;
        ops[op] = f'mov  {rrr[ddd]},M'

for sss in range(8):
    if rrr[sss] is not None:
        op = 0x70 + sss;
        ops[op] = f'mov  M,{rrr[sss]}'

for ddd in range(8):
    if rrr[ddd] is not None:
        op = 0x06 + ddd*8;
        ops[op] = f'mvi  {rrr[ddd]},#'

ops[0x36] = f'mvi  M,#'

for rp in range(4):
    op = 0x01 + rp*16
    ops[op] = f'lxi  {rr[rp]},##'

ops[0x3A] = 'lda  ##'
ops[0x32] = 'sta  ##'
ops[0x2A] = 'lhld ##'
ops[0x22] = 'shld ##'

for rp in range(2):
    op = 0x0A + rp*16
    ops[op] = f'ldax {rr[rp]}'

for rp in range(2):
    op = 0x02 + rp*16
    ops[op] = f'stax {rr[rp]}'

ops[0xEB] = 'xchg'



for ooo in range(8):
    for sss in range(8):
        if rrr[sss] is not None:
            op = 0x80 + ooo*8 + sss;
            ops[op] = f'{aluOP[ooo]:4} {rrr[sss]}'
    op = 0x80 + ooo*8 + 6;
    ops[op] = f'{aluOP[ooo]:4} M'
    op = 0xC6 + ooo*8
    ops[op] = f'{aliOP[ooo]:4} #'

for ddd in range(8):
    if rrr[ddd] is not None:
        op = 0x04 + ddd*8
        ops[op] = f'inr  {rrr[ddd]}'

ops[0x34] = f'inr  M'

for ddd in range(8):
    if rrr[ddd] is not None:
        op = 0x05 + ddd*8
        ops[op] = f'dcr  {rrr[ddd]}'
    
ops[0x35] = f'dcr  M'

for rp in range(4):
    op = 0x03 + rp*16
    ops[op] = f'inx  {rr[rp]}'

for rp in range(4):
    op = 0x0B + rp*16
    ops[op] = f'dcx  {rr[rp]}'

for rp in range(4):
    op = 0x09 + rp*16
    ops[op] = f'dad  {rr[rp]}'

ops[0x27] = 'daa'

ops[0x07] = 'rlc'
ops[0x0F] = 'rrc'
ops[0x17] = 'ral'
ops[0x1F] = 'rar'
ops[0x2F] = 'cma'
ops[0x3F] = 'cmc'
ops[0x37] = 'stc'



ops[0xC3] = 'jmp  ##'

for ccc in range(8):
    op = 0xC2 + ccc*8
    ops[op] = f'j{cond[ccc]:3} ##'

ops[0xCD] = 'call ##'

for ccc in range(8):
    op = 0xC4 + ccc*8
    ops[op] = f'c{cond[ccc]:3} ##'

ops[0xC9] = 'ret'

for ccc in range(8):
    op = 0xC0 + ccc*8
    ops[op] = f'r{cond[ccc]:3}'

for n in range(8):
    op = 0xC7 + n*8
    ops[op] = f'rst  {n}'

ops[0xE9] = 'pchl'


for rp in range(3):
    op = 0xC5 + rp*16
    ops[op] = f'push {rr[rp]}'

ops[0xF5] = f'push PSW'

for rp in range(3):
    op = 0xC1 + rp*16
    ops[op] = f'pop  {rr[rp]}'

ops[0xF1] = f'pop  PSW'

ops[0xE3] = 'xthl'
ops[0xF9] = 'sphl'

ops[0xDB] = 'in   #'
ops[0xD3] = 'out  #'

ops[0xFB] = 'ei'
ops[0xF3] = 'di'

ops[0x76] = 'hlt'
ops[0x00] = 'nop'


for op in range(256):
    if 0 == (op % 8):
        if op > 0 and 0 == (op % 64):
            print()
        print()
        print('|', end='')
    prt = ops[op]
    print(f' {prt:10} |', end='')
print()
print()

for op in range(256):
    prt = ops[op]
    mac = tomac(prt)
    if mac == '':
        mac = f'_{op:02X}'
    print(f'#define OP_{mac:12} (0x{op:02X})')
    
print()
print('static Cstr i8080_instruction_names[256] = {', end='');
for op in range(256):
    prt = ops[op]
    if 0 == op % 8:
        print()
        print('   ', end='')
    print(f' "{prt:10}",', end='');
print()
print('}')
print('Cstr i8080_instruction_name(Byte op) {');
print('    return i8080_instruction_names[op];')
print('}')
print()
