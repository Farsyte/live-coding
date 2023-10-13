	TITLE	'test for PUSH and POP instructions'

	.CPU	8080

	;; This test is being used before we deal with SHADOW,
	;; so it can only read the SHADOW ROM and writes will go
	;; to the shadowed ram area.

	ORG	00000h

START:	
	LXI	B,BCDATA
	LDAX	B
	CPI	11H
	JZ	BCPASS
	DB	08H		;SUBTEST FAILED: WRONG DATA RECEIVED
BCPASS: 
	STAX	B

	DB	18H		;next subtest

	LXI	D,DEDATA
	LDAX	D
	CPI	22H
	JZ	DEPASS
	DB	08H		;SUBTEST FAILED: WRONG DATA RECEIVED
DEPASS: 
	STAX	D

	DB	18H		;next subtest

	LDA	ADATA
	CPI	77H
	JZ	APASS
	DB	08H		;SUBEST FAILED: WRONG DATA RECEIVED
APASS:	
	STA	ADATA

	DB	18H		;next subtest

	NOP			;left margin for plot
	LHLD	HLDATA

	DB	18H		;next subtest

	MOV	A,H
	CPI	12H
	JZ	HPASS
	DB	08H		;SUBEST FAILED: WRONG DATA RECEIVED
HPASS:	
	MOV	A,L
	CPI	34H
	JZ	LPASS
	DB	08H		;SUBEST FAILED: WRONG DATA RECEIVED
LPASS:	
	DB	18H		;next subtest

	NOP			;left margin for plot
	SHLD	HLDATA

	DB	10H		;test complete

	NOP
	NOP
	HLT

	ORG	0080H
BCDATA: DB	11H		;data for LDAX/STAX B test
DEDATA: DB	22H		;data for LDAX/STAX D test
ADATA:	DB	77H		;data for LDA/STA test
HLDATA: DW	1234H		;data for LHLD/SHLD test
