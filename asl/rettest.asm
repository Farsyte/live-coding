	TITLE	'test for RET instructions'

	.CPU	8080

	;; This test exercises RET instructions in the simplest
	;; way possible: SP is set to point at a list of potential
	;; return locations, then we do a RET and various Rcc cases
	;; that either do the return, or fall through to a POP that
	;; discards the address we did not return to.
	;;
	;; Since this never does a CALL or pushes a return value onto
	;; the stack, this test still works when SHADOW is active.

	ORG	00000H		;BASE OF MEMORY

	LXI	H,LOG
	MVI	C,1
	LXI	SP, SBOT

	RET			;should "return" to CALL_P
CALL_F: DB	08H		;fail
CALL_P:	NOP

	MOV	M,L
	INX	H

	DB	18H		;next subtest

	MVI	A,1
	SUB	C
	RNZ
	POP	D	       ;discard the "do not go here" address.
CNZ_F3: SUB	C
	RNZ
CNZ_F2:	DB	08H
CNZ_F1:	DB	08H
CNZ_OK:	NOP

	MOV	M,L
	INX	H

	DB	18H		;next subtest

	MVI	A,2
	SUB	C
	RZ
	POP	D	       ;discard the "do not go here" address.
CZ_F3:	SUB	C
	RZ
CZ_F2:	DB	08H
CZ_F1:	DB	08H
CZ_OK:	NOP

	MOV	M,L
	INX	H

	DB	18H		;next subtest

	MVI	A,0
	SUB	C
	RNC
	POP	D	       ;discard the "do not go here" address.
CNC_F3: SUB	C
	RNC
CNC_F2:	DB	08H
CNC_F1:	DB	08H
CNC_OK:	NOP

	MOV	M,L
	INX	H

	DB	18H		;next subtest

	MVI	A,1
	SUB	C
	RC
	POP	D	       ;discard the "do not go here" address.
CC_F3:	SUB	C
	RC
CC_F2:	DB	08H
CC_F1:	DB	08H
CC_OK:	NOP

	MOV	M,L
	INX	H

	DB	18H		;next subtest

	MVI	A,6
	SUB	C
	RPO
	POP	D	       ;discard the "do not go here" address.
CPO_F3: SUB	C
	RPO
CPO_F2:	DB	08H
CPO_F1:	DB	08H
CPO_OK:	NOP

	MOV	M,L
	INX	H

	DB	18H		;next subtest

	MVI	A,8
	SUB	C
	RPE
	POP	D	       ;discard the "do not go here" address.
CPE_F3: SUB	C
	RPE
CPE_F2:	DB	08H
CPE_F1:	DB	08H
CPE_OK:	NOP

	MOV	M,L
	INX	H

	DB	18H		;next subtest

	MVI	A,81H
	SUB	C
	RP
	POP	D	       ;discard the "do not go here" address.
CP_F3:	SUB	C
	RP
CP_F2:	DB	08H
CP_F1:	DB	08H
CP_OK:	NOP

	MOV	M,L
	INX	H

	DB	18H		;next subtest

	MVI	A,01H
	SUB	C
	RM
	POP	D	       ;discard the "do not go here" address.
CM_F3:	SUB	C
	RM
CM_F2:	DB	08H
CM_F1:	DB	08H
CM_OK:	NOP

	MOV	M,L
	INX	H

	DB	10H		;test complete
	NOP
	NOP

	ORG	0080H
	DB	STOP-SBOT
SBOT:
	DW	CALL_P
	DW	CNZ_F1
	DW	CNZ_OK
	DW	CZ_F1
	DW	CZ_OK
	DW	CNC_F1
	DW	CNC_OK
	DW	CC_F1
	DW	CC_OK
	DW	CPO_F1
	DW	CPO_OK
	DW	CPE_F1
	DW	CPE_OK
	DW	CP_F1
	DW	CP_OK
	DW	CM_F1
	DW	CM_OK
STOP:

	ORG	00C0H
LOG:
	DS	32

	END
