	TITLE	'test for CALL instructions'

	ORG	00000H		;BASE OF MEMORY

	MVI	C,1
        LXI     SP, 0080H

	CALL	CALL_P
CALL_F: DB	08H		;fail
CALL_P:	NOP

	DB	18H		;next subtest

	MVI	A,1
	SUB	C
	CNZ	CNZ_F1
CNZ_F3: SUB	C
	CNZ	CNZ_OK
CNZ_F2:	DB	08H
CNZ_F1:	DB	08H
CNZ_OK:	NOP

	DB	18H		;next subtest

	MVI	A,2
	SUB	C
	CZ	CZ_F1
CZ_F3:  SUB	C
	CZ	CZ_OK
CZ_F2:	DB	08H
CZ_F1:	DB	08H
CZ_OK:	NOP

	DB	18H		;next subtest

	MVI	A,0
	SUB	C
	CNC	CNC_F1
CNC_F3: SUB	C
	CNC	CNC_OK
CNC_F2:	DB	08H
CNC_F1:	DB	08H
CNC_OK:	NOP

	DB	18H		;next subtest

	MVI	A,1
	SUB	C
	CC	CC_F1
CC_F3:  SUB	C
	CC	CC_OK
CC_F2:	DB	08H
CC_F1:	DB	08H
CC_OK:	NOP

	DB	18H		;next subtest

	MVI	A,6
	SUB	C
	CPO	CPO_F1
CPO_F3: SUB	C
	CPO	CPO_OK
CPO_F2:	DB	08H
CPO_F1:	DB	08H
CPO_OK:	NOP

	DB	18H		;next subtest

	MVI	A,8
	SUB	C
	CPE	CPE_F1
CPE_F3: SUB	C
	CPE	CPE_OK
CPE_F2:	DB	08H
CPE_F1:	DB	08H
CPE_OK:	NOP

	DB	18H		;next subtest

	MVI	A,81H
	SUB	C
	CP	CP_F1
CP_F3:  SUB	C
	CP	CP_OK
CP_F2:	DB	08H
CP_F1:	DB	08H
CP_OK:	NOP

	DB	18H		;next subtest

	MVI	A,01H
	SUB	C
	CM	CM_F1
CM_F3:  SUB	C
	CM	CM_OK
CM_F2:	DB	08H
CM_F1:	DB	08H
CM_OK:	NOP

	DB	10H             ;test complete
	NOP
	NOP

        ORG     0080H
        DB      ENDP-PUSHD
PUSHD: 
        DW      CALL_F
        DW	CNZ_F2
        DW	CZ_F2
        DW	CNC_F2
        DW	CC_F2
        DW	CPO_F2
        DW	CPE_F2
        DW	CP_F2
        DW	CM_F2
ENDP:   

        END
