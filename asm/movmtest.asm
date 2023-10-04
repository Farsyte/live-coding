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
        DB      18H

        MVI     B,55H
        MOV     B,M
        MOV     M,B
        INX     HL
        DB      18H

        MVI     C,55H
        MOV     C,M
        MOV     M,C
        INX     HL
        DB      18H

        MVI     D,55H
        MOV     D,M
        MOV     M,D
        INX     HL
        DB      18H

        MVI     E,55H
        MOV     E,M
        MOV     M,E
        INX     HL
        DB      18H

        MVI     M,055H
        INX     HL
        MVI     M,0AAH
        INX     HL
        DB      18H

        INR	M
        INX     HL
        DCR	M
        INX     HL
        DB      18H

        ;; This test program also tests the ALU operations
        ;; that take M as an operand.

        MVI     A,55H
        ADD     M
        MOV     M,A
        INX     HL
        DB      18H

        MVI     A,55H
        STC
        ADC     M
        MOV     M,A
        INX     HL
        DB      18H

        MVI     A,55H
        SUB     M
        MOV     M,A
        INX     HL
        DB      18H

        STC
        MVI     A,55H
        SBB     M
        MOV     M,A
        INX     HL
        DB      18H

        MVI     A,55H
        ANA     M
        MOV     M,A
        INX     HL
        DB      18H

        MVI     A,55H
        XRA     M
        MOV     M,A
        INX     HL
        DB      18H

        MVI     A,55H
        ORA     M
        MOV     M,A
        INX     HL
        DB      18H

        MVI     A,55H
        CMP     M
        MOV     M,A
        INX     HL

        DB	10H             ;test complete
        NOP
        NOP
        NOP
        HLT

