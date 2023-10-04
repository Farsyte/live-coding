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
        DB      08H             ;fail
PASS1:  NOP
        DB      18H             ;next subtest

	MVI     A,80h
        OUT     TDWP
        MVI     A,42
        IN      TDRP
        DCR     A
        JP      PASS2
        DB      08H             ;fail
PASS2:  NOP

	DB      10H             ;test complete
	NOP
	NOP

	END
