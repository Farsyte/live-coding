        TITLE   'test for MOV M,Rs and MOV Rd,M instructions'

        ;; This test is being used before we deal with SHADOW,
        ;; so it can only read the SHADOW ROM and writes will go
        ;; to the shadowed ram area.

        ORG     00000h

START:  
        LXI     HL,START

        MVI     A,55H
        MOV     A,M
        MOV     M,A
        INX     HL
        RST     2

        MVI     B,55H
        MOV     B,M
        MOV     M,B
        INX     HL
        RST     2

        MVI     C,55H
        MOV     C,M
        MOV     M,C
        INX     HL
        RST     2

        MVI     D,55H
        MOV     D,M
        MOV     M,D
        INX     HL
        RST     2

        MVI     E,55H
        MOV     E,M
        MOV     M,E
        INX     HL
        RST     2

        MVI     M,055H
        INX     HL
        MVI     M,0AAH
        INX     HL
        RST     2

        INR	M
        INX     HL
        DCR	M
        INX     HL

        RST     1
        NOP
        NOP
        NOP
        HLT

