	TITLE	'test for IN and OUT instructions'

TDWP	EQU     55
TDRP	EQU	205

	ORG	00000H		;BASE OF MEMORY

	MVI     A,1
        OUT     TDWP
        MVI     A,128
        IN      TDRP
        DCR     A
        JZ      PASS1
        RST     0               ;fail
PASS1:  NOP
        RST     2               ;next subtest

	MVI     A,80h
        OUT     TDWP
        MVI     A,42
        IN      TDRP
        DCR     A
        JP      PASS2
        RST     0               ;fail
PASS2:  NOP

	RST	1               ;test complete
	NOP
	NOP

	END
