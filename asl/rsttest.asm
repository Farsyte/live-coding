	TITLE	'test for RST instructions'

	.CPU	8080

	;; RST does a call to 00nnn000
	;; at that addresss, we call CHECK in the rom
	;; the rom CHECK subroutine pulls its return address
	;; which is in the RST service area, and logs the low byte;
	;; it then returns to just after the RST instruction.

LOG	EQU	00080H
STOP	EQU	00080H

	ORG	0F800H
	JMP	ROM
ROM:

	MVI	A,0		;leading NOP to allow debug
	STA	0000H
	STA	0008H
	STA	0010H
	STA	0018H
	STA	0020H
	STA	0028H
	STA	0030H
	STA	0038H

	MVI	A,0CDH
	STA	0001H
	STA	0009H
	STA	0011H
	STA	0019H
	STA	0021H
	STA	0029H
	STA	0031H
	STA	0039H

	LXI	H,CHECK
	SHLD	0002H
	SHLD	000AH
	SHLD	0012H
	SHLD	001AH
	SHLD	0022H
	SHLD	002AH
	SHLD	0032H
	SHLD	003AH

	;; write RST 1 to terminate test early.
	;;	   MVI	   A,010H
	;;	   STA	   0008H

	LXI	SP,STOP
	LXI	H,LOG

	DB	18H		;next subtest
	RST	0
	DB	18H		;next subtest
	RST	1
	DB	18H		;next subtest
	RST	2
	DB	18H		;next subtest
	RST	3
	DB	18H		;next subtest
	RST	4
	DB	18H		;next subtest
	RST	5
	DB	18H		;next subtest
	RST	6
	DB	18H		;next subtest
	RST	7
	DB	10H		;test complete

CHECK:
	POP	D
	MOV	M,E
	INX	H
	RET
