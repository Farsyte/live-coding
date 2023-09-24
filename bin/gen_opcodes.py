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

