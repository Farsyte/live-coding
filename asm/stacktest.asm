        TITLE   'test for PUSH and POP instructions'

        ;; This test is being used before we deal with SHADOW,
        ;; so it can only read the SHADOW ROM and writes will go
        ;; to the shadowed ram area.

        ORG     00000h

START:  
        LXI     SP, DATA
        POP     BC
        POP     DE
        POP     HL
        POP     PSW
        NOP
        DB	18H             ;next subtest

        NOP
        NOP
        PUSH    PSW
        PUSH    HL
        PUSH    DE
        PUSH    BC
        NOP
        DB	10H             ;test complete
        NOP
        NOP
        HLT

        ORG     0080H
DATA:
        DB      11H, 22H
        DB      33H, 44H
        DB      55H, 66H
        DB      28H, 77H
;;; 28H checks ignored bits in the PSW:
;;; - 0x20 is ignored on pop, and pushed as 1
;;; - 0x08 is ignored on pop, and pushed as 1
;;; - 0x02 is ignored on pop, and pushed as 0
