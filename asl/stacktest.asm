	TITLE	'test for PUSH and POP instructions'

	.CPU	8080

	;; This test is being used before we deal with SHADOW,
	;; so it can only read the SHADOW ROM and writes will go
	;; to the shadowed ram area.

	ORG	00000h

START:	
	LXI	SP, DATA
	POP	B
	POP	D
	POP	H
	POP	PSW
	NOP
	DB	18H		;next subtest

	NOP
	NOP
	PUSH	PSW
	PUSH	H
	PUSH	D
	PUSH	B
	NOP
	DB	18H		;next subtest

	LXI	D,1234H
	LXI	H,5678H
	PUSH	D
	DB	18H		;next subtest
	XTHL
	PUSH	H
	DB	10H		;test complete
	NOP
	NOP
	HLT

	ORG	007CH

	DB	00H, 00H
	DB	34H, 12H

	ORG	0080H
DATA:
	DB	11H, 22H
	DB	33H, 44H
	DB	55H, 66H
	DB	28H, 77H
;;; 28H checks ignored bits in the PSW:
;;; - 0x20 is ignored on pop, and pushed as 1
;;; - 0x08 is ignored on pop, and pushed as 1
;;; - 0x02 is ignored on pop, and pushed as 0
