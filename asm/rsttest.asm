	TITLE	'test for RST instructions'

        ;; RST does a call to 00nnn000
        ;; at that addresss, we call CHECK in the rom
        ;; the rom CHECK subroutine pulls its return address
        ;; which is in the RST service area, and logs the low byte;
        ;; it then returns to just after the RST instruction.

LOG     EQU     00080H
STOP    EQU     00080H

        ORG     0F800H
        JMP     ROM
ROM:
        MVI     A,0CDH
        LXI     HL,CHECK

        STA     0000H
        SHLD    0001H
        STA     0008H
        SHLD    0009H
        STA     0010H
        SHLD    0011H
        STA     0018H
        SHLD    0019H
        STA     0020H
        SHLD    0021H
        STA     0028H
        SHLD    0029H
        STA     0030H
        SHLD    0031H
        STA     0038H
        SHLD    0039H

        LXI     SP,STOP
        LXI     HL,LOG

        DB      18H             ;next subtest
        RST     0
        DB      18H             ;next subtest
        RST     1
        DB      18H             ;next subtest
        RST     2
        DB      18H             ;next subtest
        RST     3
        DB      18H             ;next subtest
        RST     4
        DB      18H             ;next subtest
        RST     5
        DB      18H             ;next subtest
        RST     6
        DB      18H             ;next subtest
        RST     7
        DB      10H             ;test complete

CHECK:
        POP     DE
        MOV     M,E
        INX     HL
        RET
