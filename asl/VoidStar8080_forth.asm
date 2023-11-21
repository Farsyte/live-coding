	TITLE	'8080 FIG-FORTH 1.1.1 (2023-10-12) for VoidStar8080'
        PAGE    0,0
	.CPU	8080

;;;	based on 8080 FIG-FORTH 1.1 VERSION A0 17SEP79 for CP/M
;
;	FIG-FORTH  RELEASE 1.1	FOR THE 8080 PROCESSOR
;
;	ALL PUBLICATIONS OF THE FORTH INTEREST GROUP
;	ARE PUBLIC DOMAIN.  THEY MAY BE FURTHER
;	DISTRIBUTED BY THE INCLUSION OF THIS CREDIT
;	NOTICE:
;
;	THIS PUBLICATION HAS BEEN MADE AVAILABLE BY THE
;		     FORTH INTEREST GROUP
;		     P. O. BOX 1105
;		     SAN CARLOS, CA 94070
;
;	IMPLEMENTATION BY:
;		JOHN CASSADY
;		 FOR THE FORTH IMPLEMENTATION TEAM (FIT) MARCH 1979
;	MODIFIED for CP/M by:
;		KIM HARRIS
;		FIT LIBRARIAN SEPT 1979
;	MODIFIED for VoidStar8080 by
;		Greg "(void *) Farsyte" Limes
;		OCTOBER 2023
;	ACKNOWLEDGEMENTS:
;		GEORGE FLAMMER
;		ROBT. D. VILLWOCK
;		Microsystems inc. Pasadena Ca.
;
;	 DISTRIBUTED BY	   FORTH POWER
;		P.O. BOX 2455 SAN RAFAEL CA
;		94902	415-471-1762
;		SUPPORT, SYSTEMS PROGRAMMING, 
;		APPLICATIONS PROGRAMMING
;
;  UNLESS OTHERWISE INDICATED, THIS DISTRIBUTION IS SUPPORTED
;  SOLELY BY THE FORTH INTEREST GROUP (LISTINGS) AND BY
;  FORTH POWER (MACHINE READABLE COPIES AND EXTENSIONS).
;
;   COPYRIGHT AND TRADEMARK NOTICES:
;   FORTH (C) 1974,1975,1976,1977,1978,1979 FORTH INC.
;   FIST (C) 1979 FORTH INTERNATIONAL STANDARDS TEAM
;   FIG, FORTH DIMENSIONS, FIT, (C) 1978, 1979 FORTH INTEREST GROUP
;   FORTH POWER (C) 1978, 1979 MARIN SERVICES, INC.
;   FORTH 77, FORTH 78, FORTH 79, STANDARD FORTH, FORTH INTERNATIONAL
;   STANDARD, (C) 1976, 1977, 1978, 1979, FIST
;   MULTI-FORTH (C) 1978, 1979 CREATIVE SOLUTIONS
;   CP/M (C) 1979 DIGITAL RESEARCH INC.
;   MOST ANYTHING WITH AN 11 IN IT (C) DIGITAL EQUIPMENT CORP
;   THERE MAY BE OTHERS ! !
;   MINIFORTH, MICROFORTH, POLYFORTH, FORTH  TM FORTH INC.
;   FIG-FORTH (C) 1978 1979 FORTH INTEREST GROUP
;   ALL RIGHTS RESERVED EXCEPT AS EXPRESSLY INDICATED !
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;		UPDATES, PATCHES, BUG REPORTS, EXTENSIONS
;		FOR THIS SOFTWARE IN  FORTH DIMENSIONS	
;		NEWSLETTER OF FORTH INTEREST GROUP (FIG)
;		6 issues $5.00 includes fig membership
;
;	   DOCUMENTATION FROM FIG or FORTH POWER
;	 
;		FORTH PRIMER (240pp) Richard Stevens
;		KITT PEAK NATIONAL OBSERVATORY	  $20.00
;    
;		FORTH IMPLEMENTATION TEAM LANGUAGE MODEL, EDITOR SOURCE,
;		LANGUAGE GLOSSARY, AND IMPLEMENTATION GUIDE  $10.00
;
;		FORTH FOR MICROCOMPUTERS by JOHN S JAMES
;		reprint from DDJ #25	      $2.00
;
;		FORTH POCKET PROGRAMMERS CARD  FREE W/ S.A.S.E.
;
;		SOURCE CODE FOR TI990, 6502, 6800, PDP11, PACE,
;		8080 (included here)	$10.00/ LISTING
;
;	   DOCUMENTATION FROM FIG
;
;		USING FORTH by ELIZABETH RATHER (200pp)
;		FORTH INC. 1979		      $20.00
;
;	   DOCUMENTATION FROM FORTH POWER
;		
;
;		CP/M MULTI-FORTH USERS MANUAL  $20.00
;		FORTH 79 INTERNATIONAL STANDARD 
;
;		CP/M 8080 FORTH BY FIG 8" DISKETT IBM STD.
;		WITH EDITOR AND ASSEMBLER, COPY AND PRINT,
;		AND USERS GUIDE		       $65.00
;
;		also on 5" CP/M, 5 & 8 Northstar DOS
;
;		CP/M Multi-Forth, Full 79 International
;		Standard with extensions, Strings, Prom burner,
;		Real time clock, VIDEO EDITOR, UTILITIES
;		A PROFESSIONAL LEVEL PRODUCT	$150.00
;		includes manual
;
;		PDP 11 FORTH by JOHN S. JAMES
;		8" RX01 diskett or 9 track 800 bpi DOS tape
;		runs under OS or stand alone
;		WITH USERS GUIDE		$150.00
;
;		FIG TRS 80 FORTH cassette or diskette
;		WRITE FOR PRICES
;
;		APPLE FORTH BY CapN' SOFTWARE	$40.00
;		EASYWRITER (word processor for APPLE
;		by CapN' SOFTWARE)	  $100.00
;
;		APPLE FORTH BY UNIVERSITY OF UTRECHT,
;		includes floating pt and many extensions
;		A PROFESSIONAL LEVEL PRODUCT  $100.00
;
;		FORTH FOR MICROPROSSOR DEVELOPMENT SYSTEMS,
;		FORTH FOR D.G., VAX 11, INTERDATA, Series 1,
;		C.A., HONEYWELL LEVEL 6, and others,   Write for prices
;
;	   DOCUMENTATION FROM CALTECH
;		 CALTECH FORTH MANUAL $6.00
;		CAL TECH BOOKSTORE PASADENA CA
;		by MARTIN S. EWING 100pp postpaid
;
;  CALL FOR PAPERS, ARTICLES, SPEAKERS: FOR FORTH DIMENSIONS
;   AND TRADE PUBLICATIONS SEND TO FIG.	 FOR SPEAKERS, WORKSHOPS,
;   SHOWS AND CONVENTIONS CONTACT FIG.	FIG SOLICITES FORTH SOFTWARE
;   FOR INCLUSION IN THIS EFFORT.
;		FORTH INTERNATIONAL STANDARDS TEAM (FIT)
;	       FORTH 79 INTERNATIONAL STANDARD, REQUIRED AND
;	       RESERVED WORD GLOSSARY, AND STANDARDS ACTIVITY
;	       DISTRIBUTION.  $30.00 TO FIT c/o FIG or to
;
;	       CAROLYN ROSENBERG, FIT SECRETARY
;	       c/o FORTH INC. MANHATTAN BEACH CA.
;
;
;-----------------------------------------------------
;	LABELS USED WHICH DIFFER FROM FIG-FORTH PUBLISHED
;	8080 LISTING 1.0:
;
;	REL 1.1		REL 1.0
;	-------		-------
;	ANDD		AND
;	CSPP		CSP
;	ELSEE		ELSE
;	ENDD		END
;	ENDIFF		ENDIF
;	ERASEE		ERASE
;	IDO		I
;	IFF		IF
;	INN		IN
;	MODD		MOD
;	ORR		OR
;	OUTT		OUT
;	RR		R
;	RPP		RP
;	SUBB		SUB
;	XORR		XOR
;
;	SEE ALSO:
;		RELEASE & VERSION NUMBERS
;		ASCII CHARACTER EQUATES
;		MEMORY ALLOCATION
;		DISK INTERFACE
;		CONSOLE & PRINTER INTERFACE
;
	NEWPAGE

        ;; === === === === === === === === === === === === === === === ===
        ;; VoidStar8080 memory configuration
        ;; current configuration is 48 KiB of ram and 16 KiB of rom.
        ;; === === === === === === === === === === === === === === === ===

RAMKB	EQU	62
ROMKB	EQU	2

RAMTOP	EQU	1024*RAMKB
ROMBASE EQU	1024*(64-ROMKB)

DPORT   EQU     240	        ; "OUT DPORT" triggers debug assist

	NEWPAGE
;
;----------------------------------------------------------
;
;	RELEASE & VERSION NUMBERS
;
FIGREL	EQU	1	; FIG RELEASE #
FIGREV	EQU	1	; FIG REVISION #
USRVER	EQU	1	; USER VERSION #
;
;	ASCII CHARACTERS USED
;
ABL	EQU	20H	; SPACE
ACR	EQU	0DH	; CARRIAGE RETURN
ADOT	EQU	02EH	; PERIOD
BELL	EQU	07H	; (^G)
BSIN	EQU	7FH	; INPUT BACKSPACE CHR = RUBOUT
BSOUT	EQU	08H	; OUTPUT BACKSPACE (^H)
DLE	EQU	10H	; (^P)
LF	EQU	0AH	; LINE FEED
FF	EQU	0CH	; FORM FEED (^L)
;
;	MEMORY ALLOCATION
;
EM	EQU	RAMTOP	; TOP OF MEMORY + 1 = LIMIT
NSCR	EQU	1	; NUMBER OF 1024 BYTE SCREENS
KBBUF	EQU	128	; DATA BYTES PER DISK BUFFER
US	EQU	40H	; USER VARIABLES SPACE
RTS	EQU	0A0H	; RETURN STACK & TERM BUFF SPACE
;
CO	EQU	KBBUF+4	; DISK BUFFER + 2 HEADER + 2 TAIL
NBUF	EQU	NSCR*400H/KBBUF	; NUMBER OF BUFFERS
BUF1	EQU	EM-(CO*NBUF)	; ADDR FIRST DISK BUFFER
INITR0	EQU	BUF1-US		; (R0)
INITS0	EQU	INITR0-RTS	; (S0)
;
	NEWPAGE
;
;-------------------------------------------------------
;
	ORG	100H

ORIG	NOP
	JMP	CLD	; VECTOR TO COLD START
	NOP
	JMP	WRM	; VECTOR TO WARM START

	DB	FIGREL	; FIG RELEASE #
	DB	FIGREV	; FIG REVISION #
	DB	USRVER	; USER VERSION #
	DB	0EH	; IMPLEMENTATION ATTRIBUTES
	DW	TASK-7	; TOPMOST WORD IN FORTH VOCABULARY
	DW	BSIN	; BKSPACE CHARACTER
	DW	INITR0	; INIT (UP)

;<<<<<< FOLLOWING USED BY COLD;
;	MUST BE IN SAME ORDER AS USER VARIABLES
	DW	INITS0	; INIT (S0)
	DW	INITR0	; INIT (R0)
	DW	INITS0	; INIT (TIB)
	DW	20H		; INIT (WIDTH)
	DW	0		; INIT (WARNING)
	DW	INITDP		; INIT (FENCE)
	DW	INITDP		; INIT (DP)
	DW	FORTH+6		; INIT (VOC-LINK)
;<<<<<< END DATA USED BY COLD

	DW	5H,0B320H	; CPU NAME	( HW,LW )

;				  ( 32 BIT, BASE 36 INTEGER )
;
;
;			+---------------+
;	B +ORIGIN	| . . .W:I.E.B.A|	IMPLEMENTATION
;			+---------------+	ATTRIBUTES
;			       ^ ^ ^ ^ ^
;			       | | | | +-- PROCESSOR ADDR =
;			       | | | |	   { 0 BYTE | 1 WORD }
;			       | | | +---- HIGH BYTE AT
;			       | | |	   { 0 LOW ADDR |
;			       | | |	     1 HIGH ADDR }
;			       | | +------ ADDR MUST BE EVEN
;			       | |	   { 0 YES | 1 NO }
;			       | +-------- INTERPRETER IS
;			       |	   { 0 PRE | 1 POST }
;			       |	   INCREMENTING
;			       +---------- { 0 ABOVE SUFFICIENT
;					     | 1 OTHER DIFFER-
;					     ENCES EXIST }
;
	NEWPAGE
;
;------------------------------------------------------
;
;	FORTH REGISTERS
;
;	FORTH	8080	FORTH PRESERVATION RULES
;	-----	----	------------------------------------------------------------------------
;	IP	BC	SHOULD BE PRESERVED ACROSS
;			  FORTH WORDS
;	W	DE	SOMETIMES OUTPUT FROM NEXT
;			MAY BE ALTERED BEFORE JMP'ING TO NEXT
;			INPUT ONLY WHEN 'DPUSH' CALLED
;	SP	SP	SHOULD BE USED ONLY AS DATA STACK
;			  ACROSS FORTH WORDS
;			MAY BE USED WITHIN FORTH WORDS
;			  IF RESTORED BEFORE 'NEXT'
;		HL	NEVER OUTPUT FROM NEXT
;			INPUT ONLY WHEN 'HPUSH' CALLED
;
UP	DW	INITR0	; USER AREA POINTER
RPP	DW	INITR0	; RETURN STACK POINTER
;
;------------------------------------------------------
;
;	COMMENT CONVENTIONS:
;
;	=	MEANS	"IS EQUAL TO"
;	<-	MEANS	ASSIGNMENT
;
;	NAME	=	ADDRESS OF NAME
;	(NAME)	=	CONTENTS AT NAME
;	((NAME))=	INDIRECT CONTENTS
;
;	CFA	=	ADDRESS OF CODE FIELD
;	LFA	=	ADDRESS OF LINK FIELD
;	NFA	=	ADDR OF START OF NAME FIELD
;	PFA	=	ADDR OF START OF PARAMETER FIELD
;
;	S1	=	ADDR OF 1ST WORD OF PARAMETER STACK
;	S2	=	ADDR OF 2ND WORD OF PARAMETER STACK
;	R1	=	ADDR OF 1ST WORD OF RETURN STACK
;	R2	=	ADDR OF 2ND WORD OF RETURN STACK
;	( ABOVE STACK POSITIONS VALID BEFORE & AFTER EXECUTION
;	OF ANY WORD, NOT DURING. )
;
;	LSB	=	LEAST SIGNIFICANT BIT
;	MSB	=	MOST SIGNIFICANT BIT
;	LB	=	LOW BYTE
;	HB	=	HIGH BYTE
;	LW	=	LOW WORD
;	HW	=	HIGH WORD
;	( MAY BE USED AS SUFFIX TO ABOVE NAMES )
;
	NEWPAGE
;
;---------------------------------------------------
;	DEBUG SUPPORT
;
;	TO USE:
;	(1)	SET 'BIP' TO IP VALUE TO HALT, CANNOT BE CFA
;	(2)	SET MONITOR'S BREAKPOINT PC TO 'BREAK'
;			OR PATCH 'HLT' INSTR. THERE
;	(3)	PATCH A 'JMP TNEXT' AT 'NEXT'
;	WHEN (IP) = (BIP) CPU WILL HALT
;
BIP	DW	0	; BREAKPOINT ON IP VALUE
;
TNEXT	LXI	H,BIP
	MOV	A,M	; LB
	CMP	C
	JNZ	TNEXT1
	INX	H
	MOV	A,M	; HB
	CMP	B
	JNZ	TNEXT1
BREAK	NOP		; PLACE BREAKPOINT HERE
	NOP
	NOP
TNEXT1	LDAX	B
	INX	B
	MOV	L,A
	JMP	NEXT+3
;
;--------------------------------------------------
;
;	NEXT, THE FORTH ADDRESS INTERPRETER
;	  ( POST INCREMENTING VERSION )
;
DPUSH	PUSH	D
HPUSH	PUSH	H
NEXT	LDAX	B	;(W) <- ((IP))
	INX	B	;(IP) <- (IP)+2
	MOV	L,A
	LDAX	B
	INX	B
	MOV	H,A	; (HL) <- CFA
NEXT1:	MOV	E,M	;(PC) <- ((W))
	INX	H
	MOV	D,M
	XCHG
        OUT     DPORT
	PCHL		; NOTE: (DE) = CFA+1
;
	NEWPAGE
;
;		FORTH DICTIONARY
;
;
;	DICTIONARY FORMAT:
;
;				BYTE
;	ADDRESS	NAME		CONTENTS
;	------- ----		--------
;					  ( MSB=1
;					  ( P=PRECEDENCE BIT
;					  ( S=SMUDGE BIT
;	NFA	NAME FIELD	1PS<LEN>  < NAME LENGTH
;				0<1CHAR>  MSB=0, NAME'S 1ST CHAR
;				0<2CHAR>
;				  ...
;				1<LCHAR>  MSB=1, NAME'S LAST CHR
;	LFA	LINK FIELD	<LINKLB>  = PREVIOUS WORD'S NFA
;				<LINKHB>
;LABEL:	CFA	CODE FIELD	<CODELB>  = ADDR CPU CODE
;				<CODEHB>
;	PFA	PARAMETER	<1PARAM>  1ST PARAMETER BYTE
;		FIELD		<2PARAM>
;				  ...
;
;
DP0:	DB	83H	; LIT
	DB	'LI'
	DB	'T'+80H
	DW	0	; (LFA)=0 MARKS END OF DICTIONARY
LIT	DW	$+2	;(S1) <- ((IP))
	LDAX	B	; (HL) <- ((IP)) = LITERAL
	INX	B	; (IP) <- (IP) + 2
	MOV	L,A	; LB
	LDAX	B	; HB
	INX	B
	MOV	H,A
	JMP	HPUSH	; (S1) <- (HL)
 ;
	DB	87H	; EXECUTE
	DB	'EXECUT'
	DB	'E'+80H
	DW	LIT-6
EXEC	DW	$+2
	POP	H	; (HL) <- (S1) = CFA
	JMP	NEXT1
;
	DB	86H	; BRANCH
	DB	'BRANC'
	DB	'H'+80H
	DW	EXEC-0AH
BRAN	DW	$+2	;(IP) <- (IP) + ((IP))
BRAN1	MOV	H,B	; (HL) <- (IP)
	MOV	L,C
	MOV	E,M	; (DE) <- ((IP)) = BRANCH OFFSET
	INX	H
	MOV	D,M
	DCX	H
	DAD	D	; (HL) <- (HL) + ((IP))
	MOV	C,L	; (IP) <- (HL)
	MOV	B,H
	JMP	NEXT
;
	DB	87H	; 0BRANCH
	DB	'0BRANC'
	DB	'H'+80H
	DW	BRAN-9
ZBRAN	DW	$+2
	POP	H
	MOV	A,L
	ORA	H
	JZ	BRAN1	; IF (S1)=0 THEN BRANCH
	INX	B	; ELSE SKIP BRANCH OFFSET
	INX	B
	JMP	NEXT
;
	DB	86H	; (LOOP)
	DB	'(LOOP'
	DB	')'+80H
	DW	ZBRAN-0AH
XLOOP	DW	$+2
	LXI	D,1	; (DE) <- INCREMENT
XLOO1	LHLD	RPP	; ((HL)) = INDEX
	MOV	A,M	; INDEX <- INDEX + INCR
	ADD	E
	MOV	M,A
	MOV	E,A
	INX	H
	MOV	A,M
	ADC	D
	MOV	M,A
	INX	H	; ((HL)) = LIMIT
	INR	D
	DCR	D
	MOV	D,A	; (DE) <- NEW INDEX
	JM	XLOO2	; IF INCR > 0
	MOV	A,E
	SUB	M	; THEN (A) <- INDEX - LIMIT
	MOV	A,D
	INX	H
	SBB	M
	JMP	XLOO3
XLOO2	MOV	A,M	; ELSE (A) <- LIMIT - INDEX
	SUB	E
	INX	H
	MOV	A,M
	SBB	D
;			; IF (A) < 0
XLOO3	JM	BRAN1	; THEN LOOP AGAIN
	INX	H	; ELSE DONE
	SHLD	RPP	; DISCARD R1 & R2
	INX	B	; SKIP BRANCH OFFSET
	INX	B
	JMP	NEXT
;
	DB	87H	; (+LOOP)
	DB	'(+LOOP'
	DB	')'+80H
	DW	XLOOP-9
XPLOO	DW	$+2
	POP	D	; (DE) <- INCR
	JMP	XLOO1
;
	DB	84H	; (DO)
	DB	'(DO'
	DB	')'+80H
	DW	XPLOO-0AH
XDO	DW	$+2
	LHLD	RPP	; (RP) <- (RP) - 4
	DCX	H
	DCX	H
	DCX	H
	DCX	H
	SHLD	RPP
	POP	D	; (R1) <- (S1) = INIT INDEX
	MOV	M,E
	INX	H
	MOV	M,D
	POP	D	; (R2) <- (S2) = LIMIT
	INX	H
	MOV	M,E
	INX	H
	MOV	M,D
	JMP	NEXT
;
	DB	81H	; I
	DB	'I'+80H
	DW	XDO-7
IDO	DW	$+2	;(S1) <- (R1) , (R1) UNCHANGED
	LHLD	RPP
	MOV	E,M	; (DE) <- (R1)
	INX	H
	MOV	D,M
	PUSH	D	; (S1) <- (DE)
	JMP	NEXT
;
	DB	85H	; DIGIT
	DB	'DIGI'
	DB	'T'+80H
	DW	IDO-4
DIGIT	DW	$+2
	POP	H	; (L) <- (S1)LB = ASCII CHR TO BE
;			 CONVERTED
	POP	D	; (DE) <- (S2) = BASE VALUE
	MOV	A,E
	SUI	30H	; IF CHR > "0"
	JM	DIGI2
	CPI	0AH	; AND IF CHR > "9"
	JM	DIGI1
	SUI	7
	CPI	0AH	; AND IF CHR >= "A"
	JM	DIGI2
;			; THEN VALID NUMERIC OR ALPHA CHR
DIGI1	CMP	L	; IF < BASE VALUE
	JP	DIGI2
;			; THEN VALID DIGIT CHR
	MOV	E,A	; (S2) <- (DE) = CONVERTED DIGIT
	LXI	H,1	; (S1) <- TRUE
	JMP	DPUSH
;			; ELSE INVALID DIGIT CHR
DIGI2	MOV	L,H	; (HL) <- FALSE
	JMP	HPUSH	; (S1) <- FALSE

        NEWPAGE
        ;;    ##   #######   ###   #     # ######    ##
        ;;   #     #          #    ##    # #     #     #
        ;;  #      #          #    # #   # #     #      #
        ;;  #      #####      #    #  #  # #     #      #
        ;;  #      #          #    #   # # #     #      #
        ;;   #     #          #    #    ## #     #     #
        ;;    ##   #         ###   #     # ######    ##
        ;; 
        ;;     (FIND)                   addr1 addr2 -- pfa b tf (ok)
        ;;                              addr1 addr2 -- ff (bad)
        ;;          
        ;;          Searches the dictionary starting at the name field
        ;;          address addr2, matching to the text at addr1.
        ;;          Returns parameter field address, length byt4e of
        ;;          name field and boolean true for a good match. If
        ;;          no match is found, only a boolean false is left.
        ;;
        ;;         CODE (FIND)
        ;;                   D POP,             ( pop addr2 into HL )
        ;;                                      ( stack now has only addr1 )
        ;;          ( compare word at DE to word at (SP) )
        ;; (L1)              H POP, H PUSH,     ( copy addr1 into HL )
        ;;          ( check input len with NF len and smudge bit )
        ;;                   D LDAX, M XRA, 0x3F ANI, (L5) JNZ,
        ;;
        ;;          ( length byte matches )
        ;; (L2)              H INX, D INX,      ( look at 1st byte of each )
        ;;          ( check next input byte ignoring MS bit )
        ;;                   D LDAX, M XRA, A ADD, (L4) JNZ,
        ;;          ( match so far, keep checking )
        ;;           (L2)     JNC,
        ;;          
        ;;          ( ** success ** )
        ;;          
        ;;          ( string matches. advance to accepted candidate PFA. )
        ;;                 5 H LXI, D DAD, 
        ;;          
        ;;          ( replace addr1 from stack with PFA from HL )
        ;;                     XTHL,
        ;;          
        ;;          ( scan D backwards to the NFA, to pick up first byte )
        ;; (L3)              D DCX, D LDAX, A ORA, (L3) JP,
        ;;
        ;;          ( set up to push NF length byte and TRUE as we NEXT. )
        ;;                 A E MOV, 0x00 D MVI, 1 H LXI, DPUSH JMP,
        ;;
        ;;          ( branch here if data does not match )
        ;;          ( if carry set, mismatch was in last byte of NF )
        ;; (L4)      (L6)     JC,
        ;;
        ;;          ( branch here if length does not match )
        ;;          ( scan DE forward until we see MSBit set )
        ;; (L5)              D INX, D LDAX, A ORA, (L5) JP,
        ;;
        ;; (L6)
        ;;          ( advance DE to failed candidate LFA, put it in HL )
        ;;                   D INX, XCHG,
        ;;          ( follow link to next candidate NFA )
        ;;                 M E MOV, H INX, M D MOV,
        ;;          ( if nonzero, loop back to check the new word )
        ;;                 D A MOV, E ORA, (L1) JNZ,
        ;;
        ;;          ( discard addr1, and do NEXT pushing a 0 )
        ;;                   H POP, 0 H LXI, HPUSH JMP,

;
	DB	86H	; (FIND)  (2-1)FAILURE
	DB	'(FIND'	; (2-3)SUCCESS
	DB	')'+80H
	DW	DIGIT-8
PFIND	DW	$+2
	POP	D	; (DE) <- NFA
PFIN1	POP	H	; (HL) <- STRING ADDR
	PUSH	H	; SAVE STRING ADDR FOR NEXT ITERATION
	LDAX	D
	XRA	M	; CHECK LENGTHS & SMUDGE BIT
	ANI	3FH
	JNZ	PFIN4	; LENGTHS DIFFERENT
;			; LENGTHS MATCH, CHECK EACH CHR
PFIN2	INX	H	; (HL) <- ADDR NEXT CHR IN STRING
	INX	D	; (DE) <- ADDR NEXT CHR IN NF
	LDAX	D
	XRA	M	; IGNORE MSB
	ADD	A
	JNZ	PFIN3	; NO MATCH
	JNC	PFIN2	; MATCH SO FAR, LOOP AGAIN
	LXI	H,5	; STRING MATCHES
	DAD	D	; ((SP)) <- PFA
	XTHL
;			; BACK UP TO LENGTH BYTE OF NF = NFA
PFIN6	DCX	D
	LDAX	D
	ORA	A
	JP	PFIN6	; IF MSB = 1 THEN (DE) = NFA
	MOV	E,A	; (DE) <- LENGTH BYTE
	MVI	D,0
	LXI	H,1	; (HL) <- TRUE
	JMP	DPUSH  ; RETURN, NF FOUND
;	ABOVE NF NOT A MATCH, TRY ANOTHER
PFIN3	JC	PFIN5	; IF NOT END OF NF
PFIN4	INX	D	; THEN FIND END OF NF
	LDAX	D
	ORA	A
	JP	PFIN4
PFIN5	INX	D	; (DE) <- LFA
	XCHG
	MOV	E,M	; (DE) <- (LFA)
	INX	H
	MOV	D,M
	MOV	A,D
	ORA	E	; IF (LFA) <> 0
	JNZ	PFIN1	; THEN TRY PREVIOUS DICT. DEF.
;			; ELSE END OF DICTIONARY
	POP	H	; DISCARD STRING ADDR
	LXI	H,0	; (HL) <- FALSE
	JMP	HPUSH	; RETURN, NO MATCH FOUND
;
	DB	87H	; ENCLOSE
	DB	'ENCLOS'
	DB	'E'+80H
	DW	PFIND-9
ENCL	DW	$+2
	POP	D	; (DE) <- (S1) = DELIMITER CHAR
	POP	H	; (HL) <- (S2) = ADDR TEXT TO SCAN
	PUSH	H	; (S4) <- ADDR
	MOV	A,E
	MOV	D,A	; (D) <- DELIM CHR
	MVI	E,-1	; INITIALIZE CHR OFFSET COUNTER
	DCX	H	; (HL) <- ADDR-1
;			; SKIP OVER LEADING DELIMITER CHRS
ENCL1	INX	H
	INR	E
	CMP	M	; IF TEXT CHR = DELIM CHR
	JZ	ENCL1	; THEN LOOP AGAIN
;			; ELSE NON-DELIM CHR FOUND
	MVI	D,0	; (S3) <- (E) = OFFSET TO 1ST NON-DELIM
	PUSH	D
	MOV	D,A	; (D) <- DELIM CHR
	MOV	A,M	; IF 1ST NON-DELIM = NULL
	ANA	A
	JNZ	ENCL2
	MVI	D,0	; THEN (S2) <- OFFSET TO BYTE
	INR	E	;   FOLLOWING NULL
	PUSH	D
	DCR	E	; (S1) <- OFFSET TO NULL
	PUSH	D
	JMP	NEXT
;			; ELSE TEXT CONTAINS NON-DELIM &
;			  NON-NULL CHR
ENCL2	MOV	A,D	; (A) <- DELIM CHR
	INX	H	; (HL) <- ADDR NEXT CHR
	INR	E	; (E) <- OFFSET TO NEXT CHR
	CMP	M	; IF NEXT CHR <> DELIM CHR
	JZ	ENCL4
	MOV	A,M	; AND IF NEXT CHR <> NULL
	ANA	A
	JNZ	ENCL2	; THEN CONTINUE SCAN
;			; ELSE CHR = NULL
ENCL3	MVI	D,0	; (S2) <- OFFSET TO NULL
	PUSH	D
	PUSH	D	; (S1) <- OFFSET TO NULL
	JMP	NEXT
;			; ELSE CHR = DELIM CHR
ENCL4	MVI	D,0	; (S2) <- OFFSET TO BYTE
;			  FOLLOWING TEXT
	PUSH	D
	INR	E	; (S1) <- OFFSET TO 2 BYTES AFTER
;			    END OF WORD
	PUSH	D
	JMP	NEXT
;
	DB	84H	; EMIT
	DB	'EMI'
	DB	'T'+80H
	DW	ENCL-0AH
EMIT	DW	DOCOL
	DW	PEMIT
	DW	ONE,OUTT
	DW	PSTOR,SEMIS
;
	DB	83H	; KEY
	DB	'KE'
	DB	'Y'+80H
	DW	EMIT-7
KEY	DW	$+2
	JMP	PKEY
;
	DB	89H	; ?TERMINAL
	DB	'?TERMINA'
	DB	'L'+80H
	DW	KEY-6
QTERM	DW	$+2
	LXI	H,0
	JMP	PQTER
;
	DB	82H	; CR
	DB	'C'
	DB	'R'+80H
	DW	QTERM-0CH
CR	DW	$+2
	JMP	PCR
;
	DB	85H	; CMOVE
	DB	'CMOV'
	DB	'E'+80H
	DW	CR-5
CMOVE	DW	$+2
	MOV	L,C	; (HL) <- (IP)
	MOV	H,B
	POP	B	; (BC) <- (S1) = #CHRS
	POP	D	; (DE) <- (S2) = DEST ADDR
	XTHL		; (HL) <- (S3) = SOURCE ADDR
;			; (S1) <- (IP)
	JMP	CMOV2	; RETURN IF #CHRS = 0
CMOV1	MOV	A,M	; ((DE)) <- ((HL))
	INX	H	; INC SOURCE ADDR
	STAX	D
	INX	D	; INC DEST ADDR
	DCX	B	; DEC #CHRS
CMOV2	MOV	A,B
	ORA	C
	JNZ	CMOV1	; REPEAT IF #CHRS <> 0
	POP	B	; RESTORE (IP) FROM (S1)
	JMP	NEXT
;
	DB	82H	; U*	16X16 UNSIGNED MULTIPLY
	DB	'U'	; AVG EXECUUION TIME = 994 CYCLES
	DB	'*'+80H
	DW	CMOVE-8
USTAR	DW	$+2
	POP	D	; (DE) <- MPLIER
	POP	H	; (HL) <- MPCAND
	PUSH	B	; SAVE IP
	MOV	B,H
	MOV	A,L	; (BA) <- MPCAND
	CALL	MPYX	; (AHL)1 <- MPCAND.LB * MPLIER
;			       1ST PARTIAL PRODUCT
	PUSH	H	; SAVE (HL)1
	MOV	H,A
	MOV	A,B
	MOV	B,H	; SAVE (A)1
	CALL	MPYX	; (AHL)2 <- MPCAND.HB * MPLIER
;			       2ND PARTIAL PRODUCT
	POP	D	; (DE) <- (HL)1
	MOV	C,D	; (BC) <- (AH)1
;	FORM SUM OF PARTIALS:
;			   (AHL) 1
;			+ (AHL)	 2
;			--------
;			  (AHLE)
	DAD	B	; (HL) <- (HL)2 + (AH)1
	ACI	0	; (AHLE) <- (BA) * (DE)
	MOV	D,L
	MOV	L,H
	MOV	H,A	; (HLDE) <- MPLIER * MPCAND
	POP	B	; RESTORE IP
	PUSH	D	; (S2) <- PRODUCT.LW
	JMP	HPUSH	; (S1) <- PRODUCT.HW
;
;	MULTIPLY PRIMITIVE
;		(AHL) <- (A) * (DE)
;	#BITS =	 24	  8	16
MPYX	LXI	H,0	; (HL) <- 0 = PARTIAL PRODUCT.LW
	MVI	C,8	; LOOP COUNTER
MPYX1	DAD	H	; LEFT SHIFT (AHL) 24 BITS
	RAL
	JNC	MPYX2	; IF NEXT MPLIER BIT = 1
	DAD	D	; THEN ADD MPCAND
	ACI	0
MPYX2	DCR	C	; IF NOT LAST MPLIER BIT
	JNZ	MPYX1	; THEN LOOP AGAIN
	RET		; ELSE DONE
;
	DB	82H	; U/
	DB	'U'
	DB	'/'+80H
	DW	USTAR-5
USLAS	DW	$+2
	LXI	H,4
	DAD	SP	; ((HL)) <- NUMERATOR.LW
	MOV	E,M	; (DE) <- NUMER.LW
	MOV	M,C	; SAVE IP ON STACK
	INX	H
	MOV	D,M
	MOV	M,B
	POP	B	; (BC) <- DENOMINATOR
	POP	H	; (HL) <- NUMER.HW
	MOV	A,L
	SUB	C	; IF NUMER >= DENOM
	MOV	A,H
	SBB	B
	JC	USLA1
	LXI	H,0FFFFH	; THEN OVERFLOW
	LXI	D,0FFFFH	; SET REM & QUOT TO MAX
	JMP	USLA7
USLA1	MVI	A,16	; LOOP COUNTER
USLA2	DAD	H	; LEFT SHIFT (HLDE) THRU CARRY
	RAL
	XCHG
	DAD	H
	JNC	USLA3
	INX	D
	ANA	A
USLA3	XCHG		; SHIFT DONE
	RAR		; RESTORE 1ST CARRY
	PUSH	PSW	; SAVE COUNTER
	JNC	USLA4	; IF CARRY = 1
	MOV	A,L	; THEN (HL) <- (HL) - (BC)
	SUB	C
	MOV	L,A
	MOV	A,H
	SBB	B
	MOV	H,A
	JMP	USLA5
USLA4	MOV	A,L	; ELSE TRY (HL) <- (HL) - (BC)
	SUB	C
	MOV	L,A
	MOV	A,H
	SBB	B	; (HL) <- PARTIAL REMAINDER
	MOV	H,A
	JNC	USLA5
	DAD	B	; UNDERFLOW, RESTORE
	DCX	D
USLA5	INX	D	; INC QUOT
USLA6	POP	PSW	; RESTORE COUNTER
	DCR	A	; IF COUNTER > 0
	JNZ	USLA2	; THEN LOOP AGAIN
USLA7	POP	B	; ELSE DONE, RESTORE IP
	PUSH	H	; (S2) <- REMAINDER
	PUSH	D	; (S1) <- QUOTIENT
	JMP	NEXT
;
	DB	83H	; AND
	DB	'AN'
	DB	'D'+80H
	DW	USLAS-5
ANDD	DW	$+2	; (S1) <- (S1) AND (S2)
	POP	D
	POP	H
	MOV	A,E
	ANA	L
	MOV	L,A
	MOV	A,D
	ANA	H
	MOV	H,A
	JMP	HPUSH
;
	DB	82H	; OR
	DB	'O'
	DB	'R'+80H
	DW	ANDD-6
ORR	DW	$+2	; (S1) <- (S1) OR (S2)
	POP	D
	POP	H
	MOV	A,E
	ORA	L
	MOV	L,A
	MOV	A,D
	ORA	H
	MOV	H,A
	JMP	HPUSH
;
	DB	83H	; XOR
	DB	'XO'
	DB	'R'+80H
	DW	ORR-5
XORR	DW	$+2	; (S1) <- (S1) XOR (S2)
	POP	D
	POP	H
	MOV	A,E
	XRA	L
	MOV	L,A
	MOV	A,D
	XRA	H
	MOV	H,A
	JMP	HPUSH
;
	DB	83H	; SP@
	DB	'SP'
	DB	'@'+80H
	DW	XORR-6
SPAT	DW	$+2	;(S1) <- (SP)
	LXI	H,0
	DAD	SP	; (HL) <- (SP)
	JMP	HPUSH	; (S1) <- (HL)
;
	DB	83H	; STACK POINTER STORE
	DB	'SP'
	DB	'!'+80H
	DW	SPAT-6
SPSTO	DW	$+2	;(SP) <- (S0) ( USER VARIABLE )
	LHLD	UP	; (HL) <- USER VAR BASE ADDR
	LXI	D,6
	DAD	D	; (HL) <- S0
	MOV	E,M	; (DE) <- (S0)
	INX	H
	MOV	D,M
	XCHG
	SPHL		; (SP) <- (S0)
	JMP	NEXT
;
	DB	83H	; RP@
	DB	'RP'
	DB	'@'+80H
	DW	SPSTO-6
RPAT	DW	$+2	;(S1) <- (RP)
	LHLD	RPP
	JMP	HPUSH
;
	DB	83H	; RETURN STACK POINTER STORE
	DB	'RP'
	DB	'!'+80H
	DW	RPAT-6
RPSTO	DW	$+2	;(RP) <- (R0) ( USER VARIABLE )
	LHLD	UP	; (HL) <- USER VARIABLE BASE ADDR
	LXI	D,8
	DAD	D	; (HL) <- R0
	MOV	E,M	; (DE) <- (R0)
	INX	H
	MOV	D,M
	XCHG
	SHLD	RPP	; (RP) <- (R0)
	JMP	NEXT
;
	DB	82H	; ;S
	DB	3BH ; asm8080 thinks ';' contains a comment start
	DB	'S'+80H
	DW	RPSTO-6
SEMIS	DW	$+2	;(IP) <- (R1)
	LHLD	RPP
	MOV	C,M	; (BC) <- (R1)
	INX	H
	MOV	B,M
	INX	H
	SHLD	RPP	; (RP) <- (RP) + 2
	JMP	NEXT
;
	DB	85H	; LEAVE
	DB	'LEAV'
	DB	'E'+80H
	DW	SEMIS-5
LEAVE	DW	$+2	;LIMIT <- INDEX
	LHLD	RPP
	MOV	E,M	; (DE) <- (R1) = INDEX
	INX	H
	MOV	D,M
	INX	H
	MOV	M,E	; (R2) <- (DE) = LIMIT
	INX	H
	MOV	M,D
	JMP	NEXT
;
	DB	82H	; >R
	DB	'>'
	DB	'R'+80H
	DW	LEAVE-8
TOR	DW	$+2	;(R1) <- (S1)
	POP	D	; (DE) <- (S1)
	LHLD	RPP
	DCX	H	; (RP) <- (RP) - 2
	DCX	H
	SHLD	RPP
	MOV	M,E	; ((HL)) <- (DE)
	INX	H
	MOV	M,D
	JMP	NEXT
;
	DB	82H	; R>
	DB	'R'
	DB	'>'+80H
	DW	TOR-5
FROMR	DW	$+2	;(S1) <- (R1)
	LHLD	RPP
	MOV	E,M	; (DE) <- (R1)
	INX	H
	MOV	D,M
	INX	H
	SHLD	RPP	; (RP) <- (RP) + 2
	PUSH	D	; (S1) <- (DE)
	JMP	NEXT
;
	DB	81H	; R
	DB	'R'+80H
	DW	FROMR-5
RR	DW	IDO+2
;
	DB	82H	; 0=
	DB	'0'
	DB	'='+80H
	DW	RR-4
ZEQU	DW	$+2
	POP	H	; (HL) <- (S1)
	MOV	A,L
	ORA	H	; IF (HL) = 0
	LXI	H,0	; THEN (HL) <- FALSE
	JNZ	ZEQU1
	INX	H	; ELSE (HL) <- TRUE
ZEQU1	JMP	HPUSH	; (S1) <- (HL)
;
	DB	82H	; 0<
	DB	'0'
	DB	'<'+80H
	DW	ZEQU-5
ZLESS	DW	$+2
	POP	H	; (HL) <- (S1)
	DAD	H	; IF (HL) >= 0
	LXI	H,0	; THEN (HL) <- FALSE
	JNC	ZLES1
	INX	H	; ELSE (HL) <- TRUE
ZLES1	JMP	HPUSH	; (S1) <- (HL)
;
	DB	81H	; +
	DB	'+'+80H
	DW	ZLESS-5
PLUS	DW	$+2	;(S1) <- (S1) + (S2)
	POP	D
	POP	H
	DAD	D
	JMP	HPUSH
;
	DB	82H	; D+	(4-2)
	DB	'D'	; XLW XHW  YLW YHW  ---	 SLW SHW
	DB	'+'+80H	; S4  S3   S2  S1	 S2  S1
	DW	PLUS-4
DPLUS	DW	$+2
	LXI	H,6
	DAD	SP	; ((HL)) = XLW
	MOV	E,M	; (DE) = XLW
	MOV	M,C	; SAVE IP ON STACK
	INX	H
	MOV	D,M
	MOV	M,B
	POP	B	; (BC) <- YHW
	POP	H	; (HL) <- YLW
	DAD	D
	XCHG		; (DE) <- YLW + XLW = SUM.LW
	POP	H	; (HL) <- XHW
	MOV	A,L
	ADC	C
	MOV	L,A	; (HL) <- YHW + XHW + CARRY
	MOV	A,H
	ADC	B
	MOV	H,A
	POP	B	; RESTORE IP
	PUSH	D	; (S2) <- SUM.LW
	JMP	HPUSH	; (S1) <- SUM.HW
;
	DB	85H	; MINUS
	DB	'MINU'
	DB	'S'+80H
	DW	DPLUS-5
MINUS	DW	$+2	;(S1) <- -(S1)	( 2'S COMPLEMENT )
	POP	H
	MOV	A,L
	CMA
	MOV	L,A
	MOV	A,H
	CMA
	MOV	H,A
	INX	H
	JMP	HPUSH
;
	DB	86H	; DMINUS
	DB	'DMINU'
	DB	'S'+80H
	DW	MINUS-8
DMINU	DW	$+2
	POP	H	; (HL) <- HW
	POP	D	; (DE) <- LW
	SUB	A
	SUB	E	; (DE) <- 0 - (DE)
	MOV	E,A
	MVI	A,0
	SBB	D
	MOV	D,A
	MVI	A,0
	SBB	L	; (HL) <- 0 - (HL)
	MOV	L,A
	MVI	A,0
	SBB	H
	MOV	H,A
	PUSH	D	; (S2) <- LW
	JMP	HPUSH	; (S1) <- HW
;
	DB	84H	; OVER
	DB	'OVE'
	DB	'R'+80H
	DW	DMINU-9
OVER	DW	$+2
	POP	D
	POP	H
	PUSH	H
	JMP	DPUSH
;
	DB	84H	; DROP
	DB	'DRO'
	DB	'P'+80H
	DW	OVER-7
DROP	DW	$+2
	POP	H
	JMP	NEXT
;
	DB	84H	; SWAP
	DB	'SWA'
	DB	'P'+80H
	DW	DROP-7
SWAP	DW	$+2
	POP	H
	XTHL
	JMP	HPUSH
;
	DB	83H	; DUP
	DB	'DU'
	DB	'P'+80H
	DW	SWAP-7
DUP	DW	$+2
	POP	H
	PUSH	H
	JMP	HPUSH
;
	DB	84H	; 2DUP
	DB	'2DU'
	DB	'P'+80H
	DW	DUP-6
TDUP	DW	$+2
	POP	H
	POP	D
	PUSH	D
	PUSH	H
	JMP	DPUSH
;
	DB	82H	; PLUS STORE
	DB	'+'
	DB	'!'+80H
	DW	TDUP-7
PSTOR	DW	$+2	;((S1)) <- ((S1)) + (S2)
	POP	H	; (HL) <- (S1) = ADDR
	POP	D	; (DE) <- (S2) = INCR
	MOV	A,M	; ((HL)) <- ((HL)) + (DE)
	ADD	E
	MOV	M,A
	INX	H
	MOV	A,M
	ADC	D
	MOV	M,A
	JMP	NEXT
;
	DB	86H	; TOGGLE
	DB	'TOGGL'
	DB	'E'+80H
	DW	PSTOR-5
TOGGL	DW	$+2	;((S2)) <- ((S2)) XOR (S1)LB
	POP	D	; (E) <- BYTE MASK
	POP	H	; (HL) <- ADDR
	MOV	A,M
	XRA	E
	MOV	M,A	; (ADDR) <- (ADDR) XOR (E)
	JMP	NEXT
;
	DB	81H	; @
	DB	'@'+80H
	DW	TOGGL-9
AT	DW	$+2	;(S1) <- ((S1))
	POP	H	; (HL) <- ADDR
	MOV	E,M	; (DE) <- (ADDR)
	INX	H
	MOV	D,M
	PUSH	D	; (S1) <- (DE)
	JMP	NEXT
;
	DB	82H	; C@
	DB	'C'
	DB	'@'+80H
	DW	AT-4
CAT	DW	$+2	;(S1) <- ((S1))LB
	POP	H	; (HL) <- ADDR
	MOV	L,M	; (HL) <- (ADDR)LB
	MVI	H,0
	JMP	HPUSH
;
	DB	82H	; 2@
	DB	'2'
	DB	'@'+80H
	DW	CAT-5
TAT	DW	$+2
	POP	H	; (HL) <- ADDR HW
	LXI	D,2
	DAD	D	; (HL) <- ADDR LW
	MOV	E,M	; (DE) <- LW
	INX	H
	MOV	D,M
	PUSH	D	; (S2) <- LW
	LXI	D,-3	; (HL) <- ADDR HW
	DAD	D
	MOV	E,M	; (DE) <- HW
	INX	H
	MOV	D,M
	PUSH	D	; (S1) <- HW
	JMP	NEXT
;
	DB	81H	; STORE
	DB	'!'+80H
	DW	TAT-5
STORE	DW	$+2	;((S1)) <- (S2)
	POP	H	; (HL) <- (S1) = ADDR
	POP	D	; (DE) <- (S2) = VALUE
	MOV	M,E	; ((HL)) <- (DE)
	INX	H
	MOV	M,D
	JMP	NEXT
;
	DB	82H	; C STORE
	DB	'C'
	DB	'!'+80H
	DW	STORE-4
CSTOR	DW	$+2	;((S1))LB <- (S2)LB
	POP	H	; (HL) <- (S1) = ADDR
	POP	D	; (DE) <- (S2) = BYTE
	MOV	M,E	; ((HL))LB <- (E)
	JMP	NEXT
;
	DB	82H	; 2 STORE
	DB	'2'
	DB	'!'+80H
	DW	CSTOR-5
TSTOR	DW	$+2
	POP	H	; (HL) <- ADDR
	POP	D	; (DE) <- HW
	MOV	M,E	; (ADDR) <- HW
	INX	H
	MOV	M,D
	INX	H	; (HL) <- ADDR LW
	POP	D	; (DE) <- LW
	MOV	M,E	; (ADDR+2) <- LW
	INX	H
	MOV	M,D
	JMP	NEXT

        NEWPAGE
        ;;    #               ##                                             ##
        ;;   # #             #       ####    ####   #        ####   #    #     #
        ;;    #             #       #    #  #    #  #       #    #  ##   #      #
        ;;                  #       #       #    #  #       #    #  # #  #      #
        ;;    #             #       #       #    #  #       #    #  #  # #      #
        ;;   # #             #      #    #  #    #  #       #    #  #   ##     #
        ;;    #               ##     ####    ####   ######   ####   #    #   ##
        ;;
        ;;     :                                            116            "colon"
        ;;          A defining word executed in the form:
        ;;               :  <name>  ...  ;
        ;;          Select  the  CONTEXT  vocabulary to be identical  to  CURRENT.
        ;;          Create  a  dictionary entry for <name>  in  CURRENT,  and  set
        ;;          compile   mode.    Words  thus  defined  are  called   'colon-
        ;;          definitions'.   The  compilation addresses of subsequent words
        ;;          from the input stream which are not immediate words are stored
        ;;          in  the  dictionary  to  be  executed  when  <name>  is  later
        ;;          executed.  IMMEDIATE words are executed as encountered.
        ;;
        ;;          If a word is not found after a search of the CONTEXT and FORTH
        ;;          vocabularies,  conversion and compilation of a literal  number
        ;;          is attempted,  with regard to the current BASE;  that failing,
        ;;          an error condition exists.
        ;;
        ;;      NOTE: requires 0x0128 CONSTANT RP
        ;;      NOTE: requires 0x0145 CONSTANT NEXT
        ;;
        ;;      : :                  ( CREATE NEW COLON-DEFINITION UNTIL ';' *)
        ;;        ?EXEC !CSP
        ;;        CURRENT @ CONTEXT !
        ;;        CREATE
        ;;        ]
        ;;
        ;;      ;CODE
        ;;        RPP LHLD, H DCX, B M MOV, H DCX, C M MOV, RPP SHLD,
        ;;        D INX, E C MOV, D B MOV, 0x0145 JMP,
        ;;
        ;;      IMMEDIATE
;
	DB	0C1H	; :
	DB	':'+80H
	DW	TSTOR-5
COLON	DW	DOCOL
	DW	QEXEC
	DW	SCSP
	DW	CURR
	DW	AT
	DW	CONT
	DW	STORE
	DW	CREAT
	DW	RBRAC
	DW	PSCOD
DOCOL	LHLD	RPP
	DCX	H	; (R1) <- (IP)
	MOV	M,B
	DCX	H	; (RP) <- (RP) - 2
	MOV	M,C
	SHLD	RPP
	INX	D	; (DE) <- CFA+2 = (W)
	MOV	C,E	; (IP) <- (DE) = (W)
	MOV	B,D
	JMP	NEXT

        NEWPAGE
        ;;   ###              ##                                     ##
        ;;   ###             #       ####   ######  #    #     #       #
        ;;                  #       #       #       ##  ##     #        #
        ;;   ###            #        ####   #####   # ## #     #        #
        ;;   ###            #            #  #       #    #     #        #
        ;;    #              #      #    #  #       #    #     #       #
        ;;   #                ##     ####   ######  #    #     #     ##
        ;;
        ;;
        ;;
        ;;
        ;;     ;                                            I,C,196   "semi-colon"
        ;;          Terminate  a  colon  definition  and  stop  compilation.    If
        ;;          compiling  from mass storage and the input stream is exhausted
        ;;          before encountering ; an error condition exists.
        ;;
        ;;      IMPL NOTES:
        ;;          This implementation includes compiler security: it
        ;;          verifies that the data stack pointer matches where
        ;;          it was when the definition was started. This
        ;;          detects unmatched conditional and looping constructs.
        ;;
        ;;      : ;                             ( TERMINATE COLON-DEFINITION *)
        ;;        ?CSP
        ;;        COMPILE ;S
        ;;        SMUDGE [ ;
        ;;      IMMEDIATE
;
	DB	0C1H	; ;
	DB	0BBH ; asm8080 thinks ';'+80H contains a comment start
	DW	COLON-4
SEMI	DW	DOCOL
	DW	QCSP
	DW	COMP
	DW	SEMIS
	DW	SMUDG
	DW	LBRAC
	DW	SEMIS
;
	DB	84H	; NOOP
	DB	'NOO'
	DB	'P'+80H
	DW	SEMI-4
NOOP	DW	DOCOL
	DW	SEMIS

        NEWPAGE
        ;;   ####    ####   #    #   ####    #####    ##    #    #   #####
        ;;  #    #  #    #  ##   #  #          #     #  #   ##   #     #
        ;;  #       #    #  # #  #   ####      #    #    #  # #  #     #
        ;;  #       #    #  #  # #       #     #    ######  #  # #     #
        ;;  #    #  #    #  #   ##  #    #     #    #    #  #   ##     #
        ;;   ####    ####   #    #   ####      #    #    #  #    #     #
        ;;
        ;;     CONSTANT       n --                          185
        ;;          A defining word used in the form:
        ;;               n CONSTANT <name>
        ;;          to  create  a dictionary entry for <name>,  leaving n  in  its
        ;;          parameter  field.   When <name> is later executed,  n will  be
        ;;          left on the stack.
        ;;
        ;;      IMPL NOTES:
        ;;          At runtime, the constructed word finds its PFA and
        ;;          pushes its content onto the data stack.
        ;;
        ;;      : CONSTANT              ( word which later creates constants *)
        ;;        CREATE SMUDGE ,
        ;;
        ;;      ;CODE
        ;;        D INX, XCHG,
        ;;        M E MOV, H INX, M D MOV,
        ;;        D PUSH, NEXT JMP,
;
	DB	88H	; CONSTANT
	DB	'CONSTAN'
	DB	'T'+80H
	DW	NOOP-7
CON	DW	DOCOL
	DW	CREAT
	DW	SMUDG
	DW	COMMA
	DW	PSCOD
DOCON	INX	D	; (DE) <- PFA
	XCHG
	MOV	E,M	; (DE) <- (PFA)
	INX	H
	MOV	D,M
	PUSH	D	; (S1) <- (PFA)
	JMP	NEXT

        ;;  #    #    ##    #####      #      ##    #####   #       ######
        ;;  #    #   #  #   #    #     #     #  #   #    #  #       #
        ;;  #    #  #    #  #    #     #    #    #  #####   #       #####
        ;;  #    #  ######  #####      #    ######  #    #  #       #
        ;;   #  #   #    #  #   #      #    #    #  #    #  #       #
        ;;    ##    #    #  #    #     #    #    #  #####   ######  ######
        ;;
        ;;     VARIABLE                                     227
        ;;          A defining word executed in the form:
        ;;               VARIABLE  <name>
        ;;          to  create a dictionary entry for <name> and allot  two  bytes
        ;;          for  storage  in the parameter field.   The  application  must
        ;;          initialize  the stored value.   When <name> is later executed,
        ;;          it will place the storage address on the stack.
        ;;
        ;;      IMPL NOTES:
        ;;          At runtime, the constructed word finds its PFA and
        ;;          pushes it onto the data stack.
        ;;
        ;;      : VARIABLE
        ;;          CONSTANT
        ;;
        ;;      ;CODE
        ;;        D INX, D PUSH, NEXT JMP,
;
	DB	88H	; VARIABLE
	DB	'VARIABL'
	DB	'E'+80H
	DW	CON-0BH
VAR	DW	DOCOL
	DW	CON
	DW	PSCOD
DOVAR	INX	D	; (DE) <- PFA
	PUSH	D	; (S1) <- PFA
	JMP	NEXT

        NEWPAGE
        ;;  #    #   ####   ######  #####
        ;;  #    #  #       #       #    #
        ;;  #    #   ####   #####   #    #
        ;;  #    #       #  #       #####
        ;;  #    #  #    #  #       #   #
        ;;   ####    ####   ######  #    #
        ;;
        ;;     USER           n --
        ;;          A defining word used in the form:
        ;;               n  USER  <name>
        ;;          which  creates a user variable <name>.   n is the cell  offset
        ;;          within  the  user area where the value for <name>  is  stored.
        ;;          Execution  of  <name> leaves its absolute  user  area  storage
        ;;          address.
        ;;
        ;;      IMPL NOTES:
        ;;          AT RUNTIME, THE CONSTRUCTED WORD ADDS "N" FROM ITS
        ;;          PARAMETER FIELD TO THE USER POINTER (AT 0X126) AND
        ;;          PUSHES THE SUM ONTO THE DATA STACK.
        ;;
        ;;      NOTE: requires 0x0126 CONSTANT UP
        ;;      NOTE: requires 0x0144 CONSTANT HPUSH
        ;;
        ;;      : USER                                ( CREATE USER VARIABLE *)
        ;;        CONSTANT
        ;;
        ;;      ;CODE
        ;;           D INX, XCHG, M E MOV, 0x00 D MVI,
        ;;           UP LHLD, D DAD, HPUSH JMP,
;
        ;; current collection of user variables:
        ;; 0x06 USER S0
        ;; 0x08 USER R0
        ;; 0x0A USER TIB
        ;; 0x0C USER WIDTH
        ;; 0x0E USER WARNING
        ;; 0x10 USER FENCE
        ;; 0x12 USER DP
        ;; 0x14 USER VOC-LINK
        ;; 0x16 USER BLK
        ;; 0x18 USER IN
        ;; 0x1A USER OUT
        ;; 0x1C USER SCR
        ;; 0x1E USER OFFSET
        ;; 0x20 USER CONTEXT
        ;; 0x22 USER CURRENT
        ;; 0x24 USER STATE
        ;; 0x26 USER BASE
        ;; 0x28 USER DPL
        ;; 0x2A USER FLD
        ;; 0x2C USER CSP
        ;; 0x2E USER R#
        ;; 0x30 USER HLD

	DB	84H	; USER
	DB	'USE'
	DB	'R'+80H
	DW	VAR-0BH
USER	DW	DOCOL
	DW	CON
	DW	PSCOD
DOUSE	INX	D	; (DE) <- PFA
	XCHG
	MOV	E,M	; (DE) <- USER VARIABLE OFFSET
	MVI	D,0
	LHLD	UP	; (HL) <- USER VARIABLE BASE ADDR
	DAD	D	; (HL) <- (HL) + (DE)
	JMP	HPUSH	; (S1) <- BASE + OFFSET
;
	DB	81H	; 0
	DB	'0'+80H
	DW	USER-7
ZERO	DW	DOCON
	DW	0
;
	DB	81H	; 1
	DB	'1'+80H
	DW	ZERO-4
ONE	DW	DOCON
	DW	1
;
	DB	81H	; 2
	DB	'2'+80H
	DW	ONE-4
TWO	DW	DOCON
	DW	2
;
	DB	81H	; 3
	DB	'3'+80H
	DW	TWO-4
THREE	DW	DOCON
	DW	3
;
	DB	82H	; BL
	DB	'B'
	DB	'L'+80H
	DW	THREE-4
BL	DW	DOCON
	DW	20H
;
	DB	83H	; C/L ( CHARACTERS/LINE )
	DB	'C/'
	DB	'L'+80H
	DW	BL-5
CSLL	DW	DOCON
	DW	64
;
	DB	85H	; FIRST
	DB	'FIRS'
	DB	'T'+80H
	DW	CSLL-6
FIRST	DW	DOCON
	DW	BUF1
;
	DB	85H	; LIMIT
	DB	'LIMI'
	DB	'T'+80H
	DW	FIRST-8
LIMIT	DW	DOCON
	DW	EM
;
	DB	85H	; B/BUF ( BYTES/BUFFER )
	DB	'B/BU'
	DB	'F'+80H
	DW	LIMIT-8
BBUF	DW	DOCON
	DW	KBBUF
;
	DB	85H	; B/SCR ( BUFFERS/SCREEN )
	DB	'B/SC'
	DB	'R'+80H
	DW	BBUF-8
BSCR	DW	DOCON
	DW	400H/KBBUF
;
	DB	87H	; +ORIGIN
	DB	'+ORIGI'
	DB	'N'+80H
	DW	BSCR-8
PORIG	DW	DOCOL
	DW	LIT
	DW	ORIG
	DW	PLUS
	DW	SEMIS
;
;	USER VARIABLES
;
	DB	82H	; S0
	DB	'S'
	DB	'0'+80H
	DW	PORIG-0AH
SZERO	DW	DOUSE
	DW	6
;
	DB	82H	; R0
	DB	'R'
	DB	'0'+80H
	DW	SZERO-5
RZERO	DW	DOUSE
	DW	8
;
	DB	83H	; TIB
	DB	'TI'
	DB	'B'+80H
	DW	RZERO-5
TIB	DW	DOUSE
	DB	0AH
;
	DB	85H	; WIDTH
	DB	'WIDT'
	DB	'H'+80H
	DW	TIB-6
WIDTH	DW	DOUSE
	DB	0CH
;
	DB	87H	; WARNING
	DB	'WARNIN'
	DB	'G'+80H
	DW	WIDTH-8
WARN	DW	DOUSE
	DB	0EH
;
	DB	85H	; FENCE
	DB	'FENC'
	DB	'E'+80H
	DW	WARN-0AH
FENCE	DW	DOUSE
	DB	10H
;
	DB	82H	; DP
	DB	'D'
	DB	'P'+80H
	DW	FENCE-8
DP	DW	DOUSE
	DB	12H
;
	DB	88H	; VOC-LINK
	DB	'VOC-LIN'
	DB	'K'+80H
	DW	DP-5
VOCL	DW	DOUSE
	DW	14H
;
	DB	83H	; BLK
	DB	'BL'
	DB	'K'+80H
	DW	VOCL-0BH
BLK	DW	DOUSE
	DB	16H
;
	DB	82H	; IN
	DB	'I'
	DB	'N'+80H
	DW	BLK-6
INN	DW	DOUSE
	DB	18H
;
	DB	83H	; OUT
	DB	'OU'
	DB	'T'+80H
	DW	INN-5
OUTT	DW	DOUSE
	DB	1AH
;
	DB	83H	; SCR
	DB	'SC'
	DB	'R'+80H
	DW	OUTT-6
SCR	DW	DOUSE
	DB	1CH
;
	DB	86H	; OFFSET
	DB	'OFFSE'
	DB	'T'+80H
	DW	SCR-6
OFSET	DW	DOUSE
	DB	1EH
;
	DB	87H	; CONTEXT
	DB	'CONTEX'
	DB	'T'+80H
	DW	OFSET-9
CONT	DW	DOUSE
	DB	20H
;
	DB	87H	; CURRENT
	DB	'CURREN'
	DB	'T'+80H
	DW	CONT-0AH
CURR	DW	DOUSE
	DB	22H
;
	DB	85H	; STATE
	DB	'STAT'
	DB	'E'+80H
	DW	CURR-0AH
STATE	DW	DOUSE
	DB	24H
;
	DB	84H	; BASE
	DB	'BAS'
	DB	'E'+80H
	DW	STATE-8
BASE	DW	DOUSE
	DB	26H
;
	DB	83H	; DPL
	DB	'DP'
	DB	'L'+80H
	DW	BASE-7
DPL	DW	DOUSE
	DB	28H
;
	DB	83H	; FLD
	DB	'FL'
	DB	'D'+80H
	DW	DPL-6
FLD	DW	DOUSE
	DB	2AH
;
	DB	83H	; CSP
	DB	'CS'
	DB	'P'+80H
	DW	FLD-6
CSPP	DW	DOUSE
	DB	2CH
;
	DB	82H	; R#
	DB	'R'
	DB	'#'+80H
	DW	CSPP-6
RNUM	DW	DOUSE
	DB	2EH
;
	DB	83H	; HLD
	DB	'HL'
	DB	'D'+80H
	DW	RNUM-5
HLD	DW	DOUSE
	DW	30H
;
;	END OF USER VARIABLES
;
	DB	82H	; 1+
	DB	'1'
	DB	'+'+80H
	DW	HLD-6
ONEP	DW	DOCOL
	DW	ONE
	DW	PLUS
	DW	SEMIS
;
	DB	82H	; 2+
	DB	'2'
	DB	'+'+80H
	DW	ONEP-5
TWOP	DW	DOCOL
	DW	TWO
	DW	PLUS
	DW	SEMIS
;
	DB	84H	; HERE
	DB	'HER'
	DB	'E'+80H
	DW	TWOP-5
HERE	DW	DOCOL
	DW	DP
	DW	AT
	DW	SEMIS
;
	DB	85H	; ALLOT
	DB	'ALLO'
	DB	'T'+80H
	DW	HERE-7
ALLOT	DW	DOCOL
	DW	DP
	DW	PSTOR
	DW	SEMIS
;
	DB	81H	; ,
	DB	','+80H
	DW	ALLOT-8
COMMA	DW	DOCOL
	DW	HERE
	DW	STORE
	DW	TWO
	DW	ALLOT
	DW	SEMIS
;
	DB	82H	; C,
	DB	'C'
	DB	','+80H
	DW	COMMA-4
CCOMM	DW	DOCOL
	DW	HERE
	DW	CSTOR
	DW	ONE
	DW	ALLOT
	DW	SEMIS
;
;	SUBROUTINE USED BY - AND <
;			; (HL) <- (HL) - (DE)
SSUB	MOV	A,L	; LB
	SUB	E
	MOV	L,A
	MOV	A,H	; HB
	SBB	D
	MOV	H,A
	RET
;
	DB	81H	; -
	DB	'-'+80H
	DW	CCOMM-5
SUBB	DW	$+2
	POP	D	; (DE) <- (S1) = Y
	POP	H	; (HL) <- (S2) = X
	CALL	SSUB
	JMP	HPUSH	; (S1) <- X - Y
;
	DB	81H	; =
	DB	'='+80H
	DW	SUBB-4
EQUAL	DW	DOCOL
	DW	SUBB
	DW	ZEQU
	DW	SEMIS
;
	DB	81H	; <
	DB	'<'+80H		; X  <	Y
	DW	EQUAL-4		; S2	S1
LESS	DW	$+2
	POP	D	; (DE) <- (S1) = Y
	POP	H	; (HL) <- (S2) = X
	MOV	A,D	; IF X & Y HAVE SAME SIGNS
	XRA	H
	JM	LES1
	CALL	SSUB	; (HL) <- X - Y
LES1	INR	H	; IF (HL) >= 0
	DCR	H
	JM	LES2
	LXI	H,0	; THEN X >= Y
	JMP	HPUSH	; (S1) <- FALSE
LES2	LXI	H,1	; ELSE X < Y
	JMP	HPUSH	; (S1) <- TRUE
;
	DB	82H	; U< ( UNSIGNED < )
	DB	'U'
	DB	'<'+80H
	DW	LESS-4
ULESS	DW	DOCOL,TDUP
	DW	XORR,ZLESS
        DW      ZBRAN           ; IF
        DW      ULES1-$
	DW	DROP,ZLESS
	DW	ZEQU
        DW      BRAN
        DW      ULES2-$
ULES1	DW	SUBB,ZLESS	; ELSE
ULES2	DW	SEMIS		; ENDIF
;
	DB	81H	; >
	DB	'>'+80H
	DW	ULESS-5
GREAT	DW	DOCOL
	DW	SWAP
	DW	LESS
	DW	SEMIS
;
	DB	83H	; ROT
	DB	'RO'
	DB	'T'+80H
	DW	GREAT-4
ROT	DW	$+2
	POP	D
	POP	H
	XTHL
	JMP	DPUSH
;
	DB	85H	; SPACE
	DB	'SPAC'
	DB	'E'+80H
	DW	ROT-6
SPACE	DW	DOCOL
	DW	BL
	DW	EMIT
	DW	SEMIS
;
	DB	84H	; -DUP
	DB	'-DU'
	DB	'P'+80H
	DW	SPACE-8
DDUP	DW	DOCOL
	DW	DUP
	DW	ZBRAN	; IF
	DW	DDUP1-$
	DW	DUP	; ENDIF
DDUP1	DW	SEMIS
;
	DB	88H	; TRAVERSE
	DB	'TRAVERS'
	DB	'E'+80H
	DW	DDUP-7
TRAV	DW	DOCOL
	DW	SWAP
TRAV1	DW	OVER	; BEGIN
	DW	PLUS
	DW	LIT
	DW	7FH
	DW	OVER
	DW	CAT
	DW	LESS
	DW	ZBRAN	; UNTIL
	DW	TRAV1-$
	DW	SWAP
	DW	DROP
	DW	SEMIS
;
	DB	86H	; LATEST
	DB	'LATES'
	DB	'T'+80H
	DW	TRAV-0BH
LATES	DW	DOCOL
	DW	CURR
	DW	AT
	DW	AT
	DW	SEMIS
;
	DB	83H	; LFA
	DB	'LF'
	DB	'A'+80H
	DW	LATES-9
LFA	DW	DOCOL
	DW	LIT
	DW	4
	DW	SUBB
	DW	SEMIS
;
	DB	83H	; CFA
	DB	'CF'
	DB	'A'+80H
	DW	LFA-6
CFA	DW	DOCOL
	DW	TWO
	DW	SUBB
	DW	SEMIS
;
	DB	83H	; NFA
	DB	'NF'
	DB	'A'+80H
	DW	CFA-6
NFA	DW	DOCOL
	DW	LIT
	DW	5
	DW	SUBB
	DW	LIT
	DW	-1
	DW	TRAV
	DW	SEMIS
;
	DB	83H	; PFA
	DB	'PF'
	DB	'A'+80H
	DW	NFA-6
PFA	DW	DOCOL
	DW	ONE
	DW	TRAV
	DW	LIT
	DW	5
	DW	PLUS
	DW	SEMIS
;
	DB	84H	; STORE CSP
	DB	'!CS'
	DB	'P'+80H
	DW	PFA-6
SCSP	DW	DOCOL
	DW	SPAT
	DW	CSPP
	DW	STORE
	DW	SEMIS
;
	DB	86H	; ?ERROR
	DB	'?ERRO'
	DB	'R'+80H
	DW	SCSP-7
QERR	DW	DOCOL
	DW	SWAP
	DW	ZBRAN	; IF
	DW	QERR1-$
	DW	ERROR
	DW	BRAN	; ELSE
	DW	QERR2-$
QERR1	DW	DROP	; ENDIF
QERR2	DW	SEMIS
;
	DB	85H	; ?COMP
	DB	'?COM'
	DB	'P'+80H
	DW	QERR-9
QCOMP	DW	DOCOL
	DW	STATE
	DW	AT
	DW	ZEQU
	DW	LIT
	DW	11H
	DW	QERR
	DW	SEMIS
;
	DB	85H	; ?EXEC
	DB	'?EXE'
	DB	'C'+80H
	DW	QCOMP-8
QEXEC	DW	DOCOL
	DW	STATE
	DW	AT
	DW	LIT
	DW	12H
	DW	QERR
	DW	SEMIS
;
	DB	86H	; ?PAIRS
	DB	'?PAIR'
	DB	'S'+80H
	DW	QEXEC-8
QPAIR	DW	DOCOL
	DW	SUBB
	DW	LIT
	DW	13H
	DW	QERR
	DW	SEMIS
;
	DB	84H	; ?CSP
	DB	'?CS'
	DB	'P'+80H
	DW	QPAIR-9
QCSP	DW	DOCOL
	DW	SPAT
	DW	CSPP
	DW	AT
	DW	SUBB
	DW	LIT
	DW	14H
	DW	QERR
	DW	SEMIS
;
	DB	88H	; ?LOADING
	DB	'?LOADIN'
	DB	'G'+80H
	DW	QCSP-7
QLOAD	DW	DOCOL
	DW	BLK
	DW	AT
	DW	ZEQU
	DW	LIT
	DW	16H
	DW	QERR
	DW	SEMIS
;
	DB	87H	; COMPILE
	DB	'COMPIL'
	DB	'E'+80H
	DW	QLOAD-0BH
COMP	DW	DOCOL
	DW	QCOMP
	DW	FROMR
	DW	DUP
	DW	TWOP
	DW	TOR
	DW	AT
	DW	COMMA
	DW	SEMIS
;
	DB	0C1H	; [
	DB	'['+80H
	DW	COMP-0AH
LBRAC	DW	DOCOL
	DW	ZERO
	DW	STATE
	DW	STORE
	DW	SEMIS
;
	DB	81H	; ]
	DB	']'+80H
	DW	LBRAC-4
RBRAC	DW	DOCOL
	DW	LIT,0C0H
	DW	STATE,STORE
	DW	SEMIS
;
	DB	86H	; SMUDGE
	DB	'SMUDG'
	DB	'E'+80H
	DW	RBRAC-4
SMUDG	DW	DOCOL
	DW	LATES
	DW	LIT
	DW	20H
	DW	TOGGL
	DW	SEMIS
;
	DB	83H	; HEX
	DB	'HE'
	DB	'X'+80H
	DW	SMUDG-9
HEX	DW	DOCOL
	DW	LIT
	DW	10H
	DW	BASE
	DW	STORE
	DW	SEMIS
;
	DB	87H	; DECIMAL
	DB	'DECIMA'
	DB	'L'+80H
	DW	HEX-6
DEC	DW	DOCOL
	DW	LIT
	DW	0AH
	DW	BASE
	DW	STORE
	DW	SEMIS
;
	DB	87H	; (;CODE)
	DB	'('
	DB	3BH	;asm8080 thinks ';' in a string starts a comment.
	DB	'CODE'
	DB	')'+80H
	DW	DEC-0AH
PSCOD	DW	DOCOL
	DW	FROMR
	DW	LATES
	DW	PFA
	DW	CFA
	DW	STORE
	DW	SEMIS
;
	DB	0C5H	; ;CODE
	DB	3BH	;asm8080 thinks ';' in a string starts a comment.
	DB	'COD'
	DB	'E'+80H
	DW	PSCOD-0AH
SEMIC	DW	DOCOL
	DW	QCSP
	DW	COMP
	DW	PSCOD
	DW	LBRAC
SEMI1	DW	NOOP	; ( ASSEMBLER )
	DW	SEMIS
;
	DB	87H	; <BUILDS
	DB	'<BUILD'
	DB	'S'+80H
	DW	SEMIC-8
BUILD	DW	DOCOL
	DW	ZERO
	DW	CON
	DW	SEMIS
;
	DB	85H	; DOES>
	DB	'DOES'
	DB	'>'+80H
	DW	BUILD-0AH
DOES	DW	DOCOL
	DW	FROMR
	DW	LATES
	DW	PFA
	DW	STORE
	DW	PSCOD
DODOE	LHLD	RPP	; (HL) <- (RP)
	DCX	H
	MOV	M,B	; (R1) <- (IP) = PFA = (SUBSTITUTE CFA)
	DCX	H
	MOV	M,C
	SHLD	RPP	; (RP) <- (RP) - 2
	INX	D	; (DE) <- PFA = (SUBSTITUTE CFA)
	XCHG
	MOV	C,M	; (IP) <- (SUBSTITUTE CFA)
	INX	H
	MOV	B,M
	INX	H
	JMP	HPUSH	; (S1) <- PFA+2 = SUBSTITUTE PFA
;
	DB	85H	; COUNT
	DB	'COUN'
	DB	'T'+80H
	DW	DOES-8
COUNT	DW	DOCOL
	DW	DUP
	DW	ONEP
	DW	SWAP
	DW	CAT
	DW	SEMIS
;
	DB	84H	; TYPE
	DB	'TYP'
	DB	'E'+80H
	DW	COUNT-8
TYPE	DW	DOCOL
	DW	DDUP
	DW	ZBRAN	; IF
	DW	TYPE1-$
	DW	OVER
	DW	PLUS
	DW	SWAP
	DW	XDO	; DO
TYPE2	DW	IDO
	DW	CAT
	DW	EMIT
	DW	XLOOP	; LOOP
	DW	TYPE2-$
	DW	BRAN	; ELSE
	DW	TYPE3-$
TYPE1	DW	DROP	; ENDIF
TYPE3	DW	SEMIS
;
	DB	89H	; -TRAILING
	DB	'-TRAILIN'
	DB	'G'+80H
	DW	TYPE-7
DTRAI	DW	DOCOL
	DW	DUP
	DW	ZERO
	DW	XDO	; DO
DTRA1	DW	OVER
	DW	OVER
	DW	PLUS
	DW	ONE
	DW	SUBB
	DW	CAT
	DW	BL
	DW	SUBB
	DW	ZBRAN	; IF
	DW	DTRA2-$
	DW	LEAVE
	DW	BRAN	; ELSE
	DW	DTRA3-$
DTRA2	DW	ONE
	DW	SUBB	; ENDIF
DTRA3	DW	XLOOP	; LOOP
	DW	DTRA1-$
	DW	SEMIS
;
	DB	84H	; (.")
	DB	'(.'
	DB	22H	;asm8080 thinks '"' in a string starts a comment.
	DB	')'+80H
	DW	DTRAI-0CH
PDOTQ	DW	DOCOL
	DW	RR
	DW	COUNT
	DW	DUP
	DW	ONEP
	DW	FROMR
	DW	PLUS
	DW	TOR
	DW	TYPE
	DW	SEMIS
;
	DB	0C2H	; ."
	DB	'.'
        DB      0A2H    ;asm8080 thinks '"' in a string starts a comment.
	DW	PDOTQ-7
DOTQ	DW	DOCOL
	DW	LIT
	DW	22H
	DW	STATE
	DW	AT
	DW	ZBRAN	; IF
	DW	DOTQ1-$
	DW	COMP
	DW	PDOTQ
	DW	WORD
	DW	HERE
	DW	CAT
	DW	ONEP
	DW	ALLOT
	DW	BRAN	; ELSE
	DW	DOTQ2-$
DOTQ1	DW	WORD
	DW	HERE
	DW	COUNT
	DW	TYPE	; ENDIF
DOTQ2	DW	SEMIS

        NEWPAGE
        ;;  ######  #    #  #####   ######   ####    #####
        ;;  #        #  #   #    #  #       #    #     #
        ;;  #####     ##    #    #  #####   #          #
        ;;  #         ##    #####   #       #          #
        ;;  #        #  #   #       #       #    #     #
        ;;  ######  #    #  #       ######   ####      #
        ;;
        ;;     EXPECT         addr n --                     189 ;
        ;;
        ;;          Transfer  characters  from  the terminal  beginning  at  addr,
        ;;          upward,  until a "return" or the count of n has been received.
        ;;          Take  no action for n less than or equal to zero.   One or two
        ;;          nulls are added at the end of text.
        ;;
        ;;      : EXPECT
        ;;        OVER + OVER DO
        ;;          KEY
        ;;          DUP 0x000E +ORIGIN @ = IF
        ;;            DROP
        ;;            DUP I =
        ;;            DUP R> 2 - + >R
        ;;            IF 7 ELSE 8 ENDIF
        ;;          ELSE
        ;;            DUP 0x000D = IF
        ;;              LEAVE DROP BL 0
        ;;            ELSE
        ;;              DUP
        ;;            ENDIF
        ;;            I C!
        ;;            0 I 1+ !
        ;;          ENDIF
        ;;          EMIT
        ;;        LOOP
        ;;        DROP ;
;
	DB	86H	; EXPECT
	DB	'EXPEC'
	DB	'T'+80H
	DW	DOTQ-5
EXPEC	DW	DOCOL
	DW	OVER
	DW	PLUS
	DW	OVER
	DW	XDO	; DO
EXPE1	DW	KEY
	DW	DUP
	DW	LIT
	DW	0EH
	DW	PORIG
	DW	AT
	DW	EQUAL
	DW	ZBRAN	; IF
	DW	EXPE2-$
	DW	DROP
	DW	DUP
	DW	IDO
	DW	EQUAL
	DW	DUP
	DW	FROMR
	DW	TWO
	DW	SUBB
	DW	PLUS
	DW	TOR
	DW	ZBRAN	; IF
	DW	EXPE6-$
	DW	LIT
	DW	BELL
	DW	BRAN	; ELSE
	DW	EXPE7-$
EXPE6	DW	LIT
	DW	BSOUT	; ENDIF
EXPE7	DW	BRAN	; ELSE
	DW	EXPE3-$
EXPE2	DW	DUP
	DW	LIT
	DW	0DH
	DW	EQUAL
	DW	ZBRAN	; IF
	DW	EXPE4-$
	DW	LEAVE
	DW	DROP
	DW	BL
	DW	ZERO
	DW	BRAN	; ELSE
	DW	EXPE5-$
EXPE4	DW	DUP	; ENDIF
EXPE5	DW	IDO
	DW	CSTOR
	DW	ZERO
	DW	IDO
	DW	ONEP
	DW	STORE	; ENDIF
EXPE3	DW	EMIT
	DW	XLOOP	; LOOP
	DW	EXPE1-$
	DW	DROP
	DW	SEMIS

        NEWPAGE
        ;;   ####   #    #  ######  #####    #   #
        ;;  #    #  #    #  #       #    #    # #
        ;;  #    #  #    #  #####   #    #     #
        ;;  #  # #  #    #  #       #####      #
        ;;  #   #   #    #  #       #   #      #
        ;;   ### #   ####   ######  #    #     #
        ;;
        ;;     QUERY                                        235
        ;;          Accept input of up to 80 characters (or until a 'return') from
        ;;          the operator's terminal, into the terminal input buffer.  WORD
        ;;          may  be  used  to accept text from this buffer  as  the  input
        ;;          stream, by setting >IN and BLK to zero.
        ;;
        ;; : QUERY
        ;;   TIB @ 0x0050 EXPECT
        ;;   0 IN ! ;
;
	DB	85H	; QUERY
	DB	'QUER'
	DB	'Y'+80H
	DW	EXPEC-9
QUERY	DW	DOCOL
	DW	TIB
	DW	AT
	DW	LIT
	DW	50H
	DW	EXPEC
	DW	ZERO
	DW	INN
	DW	STORE
	DW	SEMIS

        NEWPAGE
        ;; #         ###              ##                                     ##
        ;;  #       #   #            #      #    #  #    #  #       #          #
        ;;   #     # #   #          #       ##   #  #    #  #       #           #
        ;;    #    #  #  #          #       # #  #  #    #  #       #           #
        ;;     #   #   # #          #       #  # #  #    #  #       #           #
        ;;      #   #   #            #      #   ##  #    #  #       #          #
        ;;       #   ###              ##    #    #   ####   ######  ######   ##
        ;;
        ;;
        ;;     \0 (null)
        ;;          NOTE: this word is literally the word whose name is
        ;;          the single NUL byte, and is executed at the end
        ;;          of the input.
        ;;
        ;; 8081 HERE            ( prepare to replace NF with 0x81 0x80 )
        ;; : X
        ;;   BLK @ IF                           ( DISC? )
        ;;     1 BLK +!
        ;;     0 IN !
        ;;     BLK @ B/SCR 1 - AND 0=           ( SCR END? )
        ;;     IF ?EXEC R> DROP ENDIF
        ;;   ELSE                               ( TERMINAL )
        ;;     R> DROP
        ;;   ENDIF
        ;;   ; ! IMMEDIATE        ( finish defining "X", overwite the name, then make it immediate. )
;
	DB	0C1H	; 0 (NULL)
	DB	80H
	DW	QUERY-8
NULL	DW	DOCOL
	DW	BLK
	DW	AT
	DW	ZBRAN	; IF
	DW	NULL1-$
	DW	ONE
	DW	BLK
	DW	PSTOR
	DW	ZERO
	DW	INN
	DW	STORE
	DW	BLK
	DW	AT
	DW	BSCR
	DW	ONE
	DW	SUBB
	DW	ANDD
	DW	ZEQU
	DW	ZBRAN	; IF
	DW	NULL2-$
	DW	QEXEC
	DW	FROMR
	DW	DROP	; ENDIF
NULL2	DW	BRAN	; ELSE
	DW	NULL3-$
NULL1	DW	FROMR
	DW	DROP	; ENDIF
NULL3	DW	SEMIS
;
	DB	84H	; FILL
	DB	'FIL'
	DB	'L'+80H
	DW	NULL-4
FILL	DW	$+2
	MOV	L,C
	MOV	H,B
	POP	D
	POP	B
	XTHL
	XCHG
FILL1	MOV	A,B	; BEGIN
	ORA	C
	JZ	FILL2	; WHILE
	MOV	A,L
	STAX	D
	INX	D
	DCX	B
	JMP	FILL1	; REPEAT
FILL2	POP	B
	JMP	NEXT
;
	DB	85H	; ERASE
	DB	'ERAS'
	DB	'E'+80H
	DW	FILL-7
ERASEE	DW	DOCOL
	DW	ZERO
	DW	FILL
	DW	SEMIS
;
	DB	86H	; BLANKS
	DB	'BLANK'
	DB	'S'+80H
	DW	ERASEE-8
BLANK	DW	DOCOL
	DW	BL
	DW	FILL
	DW	SEMIS
;
	DB	84H	; HOLD
	DB	'HOL'
	DB	'D'+80H
	DW	BLANK-9
HOLD	DW	DOCOL
	DW	LIT
	DW	-1
	DW	HLD
	DW	PSTOR
	DW	HLD
	DW	AT
	DW	CSTOR
	DW	SEMIS
;
	DB	83H	; PAD
	DB	'PA'
	DB	'D'+80H
	DW	HOLD-7
PAD	DW	DOCOL
	DW	HERE
	DW	LIT
	DW	44H
	DW	PLUS
	DW	SEMIS

        NEWPAGE
        ;;  #    #   ####   #####   #####
        ;;  #    #  #    #  #    #  #    #
        ;;  #    #  #    #  #    #  #    #
        ;;  # ## #  #    #  #####   #    #
        ;;  ##  ##  #    #  #   #   #    #
        ;;  #    #   ####   #    #  #####
        ;;
        ;;     WORD           char -- addr                  181
        ;;          Receive  characters  from the input stream until the  non-zero
        ;;          delimiting  character  is encountered or the input  stream  is
        ;;          exhausted,  ignoring leading delimiters.   The characters  are
        ;;          stored  as  a  packed string with the character count  in  the
        ;;          first  character position.   The actual delimiter  encountered
        ;;          (char  or  null)  is stored at the end of  the  text  but  not
        ;;          included  in the count.   If the input stream was exhausted as
        ;;          WORD is called,  then a zero length will result.   The address
        ;;          of the beginning of this packed string is left on the stack.
        ;;
        ;;     ................................................( tos ... bos )
        ;;     : WORD                                            ( delimiter )
        ;;       BLK @ IF BLK @ BLOCK ELSE TIB @ ENDIF      ( base delimiter )
        ;;       IN @ + SWAP                                ( delimiter addr )
        ;;       ENCLOSE                        ( count end-ix start-ix addr )
        ;;       HERE 0x22 BLANKS       ( prepare 34 blanks at DP )
        ;;       IN +!          ( take this word )    ( end-ix start-ix addr )
        ;;       OVER - >R      ( save char count )          ( start-ix addr )
        ;;       R HERE C!      ( store length first )       ( start-ix addr )
        ;;       + HERE 1+ R>                        ( count dp+1 start-addr )
        ;;       CMOVE ;
;
	DB	84H	; WORD
	DB	'WOR'
	DB	'D'+80H
	DW	PAD-6
WORD	DW	DOCOL
	DW	BLK
	DW	AT
	DW	ZBRAN	; IF
	DW	WORD1-$
	DW	BLK
	DW	AT
	DW	BLOCK
	DW	BRAN	; ELSE
	DW	WORD2-$
WORD1	DW	TIB
	DW	AT	; ENDIF
WORD2	DW	INN
	DW	AT
	DW	PLUS
	DW	SWAP
	DW	ENCL
	DW	HERE
	DW	LIT
	DW	22H
	DW	BLANK
	DW	INN
	DW	PSTOR
	DW	OVER
	DW	SUBB
	DW	TOR
	DW	RR
	DW	HERE
	DW	CSTOR
	DW	PLUS
	DW	HERE
	DW	ONEP
	DW	FROMR
	DW	CMOVE
	DW	SEMIS
;
	DB	88H	; (NUMBER)
	DB	'(NUMBER'
	DB	')'+80H
	DW	WORD-7
PNUMB	DW	DOCOL
PNUM1	DW	ONEP	; BEGIN
	DW	DUP
	DW	TOR
	DW	CAT
	DW	BASE
	DW	AT
	DW	DIGIT
	DW	ZBRAN	; WHILE
	DW	PNUM2-$
	DW	SWAP
	DW	BASE
	DW	AT
	DW	USTAR
	DW	DROP
	DW	ROT
	DW	BASE
	DW	AT
	DW	USTAR
	DW	DPLUS
	DW	DPL
	DW	AT
	DW	ONEP
	DW	ZBRAN	; IF
	DW	PNUM3-$
	DW	ONE
	DW	DPL
	DW	PSTOR	; ENDIF
PNUM3	DW	FROMR
	DW	BRAN	; REPEAT
	DW	PNUM1-$
PNUM2	DW	FROMR
	DW	SEMIS
;
	DB	86H	; NUMBER
	DB	'NUMBE'
	DB	'R'+80H
	DW	PNUMB-0BH
NUMB	DW	DOCOL
	DW	ZERO
	DW	ZERO
	DW	ROT
	DW	DUP
	DW	ONEP
	DW	CAT
	DW	LIT
	DW	2DH
	DW	EQUAL
	DW	DUP
	DW	TOR
	DW	PLUS
	DW	LIT
	DW	-1
NUMB1	DW	DPL	; BEGIN
	DW	STORE
	DW	PNUMB
	DW	DUP
	DW	CAT
	DW	BL
	DW	SUBB
	DW	ZBRAN	; WHILE
	DW	NUMB2-$
	DW	DUP
	DW	CAT
	DW	LIT
	DW	2EH
	DW	SUBB
	DW	ZERO
	DW	QERR
	DW	ZERO
	DW	BRAN	; REPEAT
	DW	NUMB1-$
NUMB2	DW	DROP
	DW	FROMR
	DW	ZBRAN	; IF
	DW	NUMB3-$
	DW	DMINU	; ENDIF
NUMB3	DW	SEMIS

        NEWPAGE
        ;;          ######     #    #    #  #####
        ;;          #          #    ##   #  #    #
        ;;  #####   #####      #    # #  #  #    #
        ;;          #          #    #  # #  #    #
        ;;          #          #    #   ##  #    #
        ;;          #          #    #    #  #####
        ;;
        ;;     FIND
        ;;         Accepts the next text word (delimited
        ;;         by blanks) in the input screen to
        ;;         HERE, and searches the CONTEXT and
        ;;         then CURRENT vocabularies for a
        ;;         matching entry. If found, the
        ;;         dictionary entry's parameter field
        ;;         address, its length byte, and a
        ;;         boolean true is left. Otherwise,
        ;;         only a boolean false is left.
        ;;
        ;; ................................................( tos ... eos )
        ;; : -FIND                                                 ( --- )
        ;;   BL WORD                                               ( --- )
        ;;   HERE                                                  ( eow )
        ;;   CONTEXT @ @                                       ( sow eow )
        ;;   (FIND)
        ;;   DUP 0= IF
        ;;     DROP
        ;;     HERE
        ;;     LATEST
        ;;     (FIND)
        ;;   ENDIF
        ;;   ;
        ;;
;
	DB	85H	; -FIND	(0-3) SUCCESS
	DB	'-FIN'	; (0-1) FAILURE
	DB	'D'+80H
	DW	NUMB-9
DFIND	DW	DOCOL
	DW	BL
	DW	WORD
	DW	HERE
	DW	CONT
	DW	AT
	DW	AT
	DW	PFIND
	DW	DUP
	DW	ZEQU
	DW	ZBRAN	; IF
	DW	DFIN1-$
	DW	DROP
	DW	HERE
	DW	LATES
	DW	PFIND	; ENDIF
DFIN1	DW	SEMIS
;
	DB	87H	; (ABORT)
	DB	'(ABORT'
	DB	')'+80H
	DW	DFIND-8
PABOR	DW	DOCOL
	DW	ABORT
	DW	SEMIS
;
	DB	85H	; ERROR
	DB	'ERRO'
	DB	'R'+80H
	DW	PABOR-0AH
ERROR	DW	DOCOL
	DW	WARN
	DW	AT
	DW	ZLESS
	DW	ZBRAN	; IF
	DW	ERRO1-$
	DW	PABOR	; ENDIF
ERRO1	DW	HERE
	DW	COUNT
	DW	TYPE
	DW	PDOTQ
	DB	2
	DB	'? '
	DW	MESS
	DW	SPSTO
;	CHANGE FROM FIG MODEL
;	DW	INN,AT,BLK,AT
	DW	BLK,AT
	DW	DDUP
        DW      ZBRAN           ; IF
        DW      ERRO2-$
	DW	INN,AT
	DW	SWAP		; ENDIF
ERRO2	DW	QUIT
;
	DB	83H	; ID.
	DB	'ID'
	DB	'.'+80H
	DW	ERROR-8
IDDOT	DW	DOCOL
	DW	PAD
	DW	LIT
	DW	20H
	DW	LIT
	DW	5FH
	DW	FILL
	DW	DUP
	DW	PFA
	DW	LFA
	DW	OVER
	DW	SUBB
	DW	PAD
	DW	SWAP
	DW	CMOVE
	DW	PAD
	DW	COUNT
	DW	LIT
	DW	1FH
	DW	ANDD
	DW	TYPE
	DW	SPACE
	DW	SEMIS

        NEWPAGE

        ;;   ####   #####   ######    ##     #####  ######
        ;;  #    #  #    #  #        #  #      #    #
        ;;  #       #    #  #####   #    #     #    #####
        ;;  #       #####   #       ######     #    #
        ;;  #    #  #   #   #       #    #     #    #
        ;;   ####   #    #  ######  #    #     #    ######
        ;; 
        ;;     CREATE                                       239 
        ;;          A defining word used in the form: 
        ;;               CREATE  <name> 
        ;;          to  create a dictionary entry for <name>,  without  allocating 
        ;;          any  parameter  field memory.   When  <name>  is  subsequently 
        ;;          executed,  the address of the first byte of <name>'s parameter 
        ;;          field is left on the stack. 
        ;; 
        ;; / E78	: CREATE
        ;; / E83	  -FIND IF DROP NFA ID. 0x0004 MESSAGE SPACE ENDIF
        ;;                HERE DUP C@ WIDTH @ MIN 1+ ALLOT 
        ;;                DUP 0xA0 TOGGLE 
        ;;                HERE 1 - 0x80 TOGGLE 
        ;;                LATEST , CURRENT @ ! 
        ;;                HERE 2+ , ;
;
	DB	86H	; CREATE
	DB	'CREAT'
	DB	'E'+80H
	DW	IDDOT-6
CREAT	DW	DOCOL
	DW	DFIND
	DW	ZBRAN	; IF
	DW	CREA1-$
	DW	DROP
	DW	NFA
	DW	IDDOT
	DW	LIT
	DW	4
	DW	MESS
	DW	SPACE	; ENDIF
CREA1	DW	HERE
	DW	DUP
	DW	CAT
	DW	WIDTH
	DW	AT
	DW	MIN
	DW	ONEP
	DW	ALLOT
	DW	DUP
	DW	LIT
	DW	0A0H
	DW	TOGGL
	DW	HERE
	DW	ONE
	DW	SUBB
	DW	LIT
	DW	80H
	DW	TOGGL
	DW	LATES
	DW	COMMA
	DW	CURR
	DW	AT
	DW	STORE
	DW	HERE
	DW	TWOP
	DW	COMMA
	DW	SEMIS
;
	DB	0C9H	; [COMPILE]
	DB	'[COMPILE'
	DB	']'+80H
	DW	CREAT-9
BCOMP	DW	DOCOL
	DW	DFIND
	DW	ZEQU
	DW	ZERO
	DW	QERR
	DW	DROP
	DW	CFA
	DW	COMMA
	DW	SEMIS
;
	DB	0C7H	; LITERAL
	DB	'LITERA'
	DB	'L'+80H
	DW	BCOMP-0CH
LITER	DW	DOCOL
	DW	STATE
	DW	AT
	DW	ZBRAN	; IF
	DW	LITE1-$
	DW	COMP
	DW	LIT
	DW	COMMA	; ENDIF
LITE1	DW	SEMIS
;
	DB	0C8H	; DLITERAL
	DB	'DLITERA'
	DB	'L'+80H
	DW	LITER-0AH
DLITE	DW	DOCOL
	DW	STATE
	DW	AT
	DW	ZBRAN	; IF
	DW	DLIT1-$
	DW	SWAP
	DW	LITER
	DW	LITER	; ENDIF
DLIT1	DW	SEMIS
;
	DB	86H	; ?STACK
	DB	'?STAC'
	DB	'K'+80H
	DW	DLITE-0BH
QSTAC	DW	DOCOL
	DW	SPAT
	DW	SZERO
	DW	AT
	DW	SWAP
	DW	ULESS
	DW	ONE
	DW	QERR
	DW	SPAT
	DW	HERE
	DW	LIT
	DW	80H
	DW	PLUS
	DW	ULESS
	DW	LIT
	DW	7
	DW	QERR
	DW	SEMIS

        NEWPAGE
        ;;     #    #    #   #####  ######  #####   #####   #####   ######   #####
        ;;     #    ##   #     #    #       #    #  #    #  #    #  #          #
        ;;     #    # #  #     #    #####   #    #  #    #  #    #  #####      #
        ;;     #    #  # #     #    #       #####   #####   #####   #          #
        ;;     #    #   ##     #    #       #   #   #       #   #   #          #
        ;;     #    #    #     #    ######  #    #  #       #    #  ######     #
        ;;
        ;;   : INTERPRET
        ;;     BEGIN
        ;;       -FIND IF
        ;;         STATE @ < IF
        ;;           CFA , ELSE CFA EXECUTE ENDIF
        ;;         ?STACK
        ;;       ELSE
        ;;         HERE NUMBER DPL @ 1+ IF
        ;;           DLITERAL ELSE DROP LITERAL ENDIF
        ;;         ?STACK
        ;;       ENDIF
        ;;     AGAIN
        ;;   ] ( -- THERE IS NO ;S AT THE END OF INTERPRET -- )
;
	DB	89H	; INTERPRET
	DB	'INTERPRE'
	DB	'T'+80H
	DW	QSTAC-9
INTER	DW	DOCOL
INTE1	DW	DFIND	; BEGIN
	DW	ZBRAN	; IF
	DW	INTE2-$
	DW	STATE
	DW	AT
	DW	LESS
	DW	ZBRAN	; IF
	DW	INTE3-$
	DW	CFA
	DW	COMMA
	DW	BRAN	; ELSE
	DW	INTE4-$
INTE3	DW	CFA
	DW	EXEC	; ENDIF
INTE4	DW	QSTAC
	DW	BRAN	; ELSE
	DW	INTE5-$
INTE2	DW	HERE
	DW	NUMB
	DW	DPL
	DW	AT
	DW	ONEP
	DW	ZBRAN	; IF
	DW	INTE6-$
	DW	DLITE
	DW	BRAN	; ELSE
	DW	INTE7-$
INTE6	DW	DROP
	DW	LITER	; ENDIF
INTE7	DW	QSTAC	; ENDIF
INTE5	DW	BRAN	; AGAIN
	DW	INTE1-$
;
	DB	89H	; IMMEDIATE
	DB	'IMMEDIAT'
	DB	'E'+80H
	DW	INTER-0CH
IMMED	DW	DOCOL
	DW	LATES
	DW	LIT
	DW	40H
	DW	TOGGL
	DW	SEMIS
;
	DB	8AH	; VOCABULARY
	DB	'VOCABULAR'
	DB	'Y'+80H
	DW	IMMED-0CH
VOCAB	DW	DOCOL
	DW	BUILD
	DW	LIT
	DW	0A081H
	DW	COMMA
	DW	CURR
	DW	AT
	DW	CFA
	DW	COMMA
	DW	HERE
	DW	VOCL
	DW	AT
	DW	COMMA
	DW	VOCL
	DW	STORE
	DW	DOES
DOVOC	DW	TWOP
	DW	CONT
	DW	STORE
	DW	SEMIS
;
	DB	0C5H	; FORTH
	DB	'FORT'
	DB	'H'+80H
	DW	VOCAB-0DH
FORTH	DW	DODOE
	DW	DOVOC
	DW	0A081H
	DW	TASK-7	; COLD START VALUE ONLY
;			  CHANGED EACH TIME A DEF IS APPENDED
;			  TO THE FORTH VOCABULARY
	DW	0	; END OF VOCABULARY LIST
;
	DB	8BH	; DEFINITIONS
	DB	'DEFINITION'
	DB	'S'+80H
	DW	FORTH-8
DEFIN	DW	DOCOL
	DW	CONT
	DW	AT
	DW	CURR
	DW	STORE
	DW	SEMIS
;
	DB	0C1H	; (
	DB	'('+80H
	DW	DEFIN-0EH
PAREN	DW	DOCOL
	DW	LIT
	DW	29H
	DW	WORD
	DW	SEMIS
;
	DB	84H	; QUIT
	DB	'QUI'
	DB	'T'+80H
	DW	PAREN-4
QUIT	DW	DOCOL
	DW	ZERO
	DW	BLK
	DW	STORE
	DW	LBRAC
QUIT1	DW	RPSTO	; BEGIN
	DW	CR
	DW	QUERY
	DW	INTER
	DW	STATE
	DW	AT
	DW	ZEQU
	DW	ZBRAN	; IF
	DW	QUIT2-$
	DW	PDOTQ
	DB	2
	DB	'OK'	; ENDIF
QUIT2	DW	BRAN	; AGAIN
	DW	QUIT1-$

        NEWPAGE
        ;;    ##    #####    ####   #####    #####
        ;;   #  #   #    #  #    #  #    #     #
        ;;  #    #  #####   #    #  #    #     #
        ;;  ######  #    #  #    #  #####      #
        ;;  #    #  #    #  #    #  #   #      #
        ;;  #    #  #####    ####   #    #     #
        ;;
        ;; ABORT
        ;;        Get back to a known useful state:
        ;;        - clear the data stack
        ;;        - set BASE back to decimal
        ;;        - print the initial greeting (8080 fig-FORTH 1.1)
        ;;        - select FORTH vocabulary for search and extension
        ;;
        ;;      : ABORT
        ;;        SP! DECIMAL ?STACK
        ;;        CR .CPU ." fig-FORTH 1.1"
        ;;        FORTH DEFINITIONS
        ;;        QUIT
        ;;
        ;;      [ ( -- stop compiling, no ";S" after QUIT -- )
;
	DB	85H	; ABORT
	DB	'ABOR'
	DB	'T'+80H
	DW	QUIT-7
ABORT	DW	DOCOL
	DW	SPSTO
	DW	DEC
	DW	QSTAC
	DW	CR
	DW	DOTCPU
	DW	PDOTQ
	DB	0DH
	DB	'fig-FORTH '
	DB	FIGREL+30H,ADOT,FIGREV+30H
	DW	FORTH
	DW	DEFIN
	DW	QUIT
;
WRM	LXI	B,WRM1
	JMP	NEXT
WRM1	DW	WARM
;
	DB	84H	; WARM
	DB	'WAR'
	DB	'M'+80H
	DW	ABORT-8
WARM	DW	DOCOL
	DW	MTBUF
	DW	ABORT
;
CLD	LXI	B,CLD1          ;INITIAL INTERPRET POINTER
	LHLD	ORIG+12H        ;INITIAL DATA STACK POINTER
	SPHL
	JMP	NEXT
CLD1	DW	COLD

        NEWPAGE
        ;;   ####    ####   #       #####
        ;;  #    #  #    #  #       #    #
        ;;  #       #    #  #       #    #
        ;;  #       #    #  #       #    #
        ;;  #    #  #    #  #       #    #
        ;;   ####    ####   ######  #####
        ;;
        ;; uses various constants not defined elsewhere:
        ;;   0xF3E0 CONSTANT BUF1
        ;;   0x1625 CONSTANT EPRINT
        ;;   0x0112 CONSTANT USER-INIT
        ;;   0x0126 CONSTANT USER-PTR
        ;;   FORTH 6 + CONSTANT FORTH-DICT
        ;;   ' TASK NFA CONSTANT TOP-WORD
        ;;
        ;; NOTE: VoidStar8080 defaults to DOUBLE DENSITY disks.
        ;;
        ;; : COLD
        ;;   EMPTY-BUFFERS
        ;;   1 DENSITY !
        ;;   BUF1 USE !
        ;;   BUF1 PREV !
        ;;   DR0
        ;;   0 EPRINT !
        ;;   USER-INIT USER-PTR @ 6 + 0x10 CMOVE
        ;;   TOP-WORD @ FORTH-DICT !
        ;;   ABORT
        ;;
        ;; [ ( -- stop compiling, no ";S" after ABORT -- )
;
	DB	84H	; COLD
	DB	'COL'
	DB	'D'+80H
	DW	WARM-7
COLD	DW	DOCOL
	DW	MTBUF
        DW      ONE,DENSTY
	DW	STORE
	DW	LIT,BUF1
	DW	USE,STORE
	DW	LIT,BUF1
	DW	PREV,STORE
	DW	DRZER
	DW	LIT,0
	DW	LIT,EPRINT
	DW	STORE
;
	DW	LIT
	DW	ORIG+12H
	DW	LIT
	DW	UP
	DW	AT
	DW	LIT
	DW	6
	DW	PLUS
	DW	LIT
	DW	10H
	DW	CMOVE
	DW	LIT
	DW	ORIG+0CH
	DW	AT
	DW	LIT
	DW	FORTH+6
	DW	STORE
	DW	ABORT
;
	DB	84H	; S->D
	DB	'S->'
	DB	'D'+80H
	DW	COLD-7
STOD	DW	$+2
	POP	D
	LXI	H,0
	MOV	A,D
	ANI	80H
	JZ	STOD1
	DCX	H
STOD1	JMP	DPUSH
;
	DB	82H	; +-
	DB	'+'
	DB	'-'+80H
	DW	STOD-7
PM	DW	DOCOL
	DW	ZLESS
	DW	ZBRAN	; IF
	DW	PM1-$
	DW	MINUS	; ENDIF
PM1	DW	SEMIS
;
	DB	83H	; D+-
	DB	'D+'
	DB	'-'+80H
	DW	PM-5
DPM	DW	DOCOL
	DW	ZLESS
	DW	ZBRAN	; IF
	DW	DPM1-$
	DW	DMINU	; ENDIF
DPM1	DW	SEMIS
;
	DB	83H	; ABS
	DB	'AB'
	DB	'S'+80H
	DW	DPM-6
ABS	DW	DOCOL
	DW	DUP
	DW	PM
	DW	SEMIS
;
	DB	84H	; DABS
	DB	'DAB'
	DB	'S'+80H
	DW	ABS-6
DABS	DW	DOCOL
	DW	DUP
	DW	DPM
	DW	SEMIS
;
	DB	83H	; MIN
	DB	'MI'
	DB	'N'+80H
	DW	DABS-7
MIN	DW	DOCOL,TDUP
	DW	GREAT
	DW	ZBRAN	; IF
	DW	MIN1-$
	DW	SWAP	; ENDIF
MIN1	DW	DROP
	DW	SEMIS
;
	DB	83H	; MAX
	DB	'MA'
	DB	'X'+80H
	DW	MIN-6
MAX	DW	DOCOL,TDUP
	DW	LESS
	DW	ZBRAN	; IF
	DW	MAX1-$
	DW	SWAP	; ENDIF
MAX1	DW	DROP
	DW	SEMIS
;
	DB	82H	; M*
	DB	'M'
	DB	'*'+80H
	DW	MAX-6
MSTAR	DW	DOCOL,TDUP
	DW	XORR
	DW	TOR
	DW	ABS
	DW	SWAP
	DW	ABS
	DW	USTAR
	DW	FROMR
	DW	DPM
	DW	SEMIS
;
	DB	82H	; M/
	DB	'M'
	DB	'/'+80H
	DW	MSTAR-5
MSLAS	DW	DOCOL
	DW	OVER
	DW	TOR
	DW	TOR
	DW	DABS
	DW	RR
	DW	ABS
	DW	USLAS
	DW	FROMR
	DW	RR
	DW	XORR
	DW	PM
	DW	SWAP
	DW	FROMR
	DW	PM
	DW	SWAP
	DW	SEMIS
;
	DB	81H	; *
	DB	'*'+80H
	DW	MSLAS-5
STAR	DW	DOCOL
	DW	MSTAR
	DW	DROP
	DW	SEMIS
;
	DB	84H	; /MOD
	DB	'/MO'
	DB	'D'+80H
	DW	STAR-4
SLMOD	DW	DOCOL
	DW	TOR
	DW	STOD
	DW	FROMR
	DW	MSLAS
	DW	SEMIS
;
	DB	81H	; /
	DB	'/'+80H
	DW	SLMOD-7
SLASH	DW	DOCOL
	DW	SLMOD
	DW	SWAP
	DW	DROP
	DW	SEMIS
;
	DB	83H	; MOD
	DB	'MO'
	DB	'D'+80H
	DW	SLASH-4
MODD	DW	DOCOL
	DW	SLMOD
	DW	DROP
	DW	SEMIS
;
	DB	85H	; */MOD
	DB	'*/MO'
	DB	'D'+80H
	DW	MODD-6
SSMOD	DW	DOCOL
	DW	TOR
	DW	MSTAR
	DW	FROMR
	DW	MSLAS
	DW	SEMIS
;
	DB	82H	; */
	DB	'*'
	DB	'/'+80H
	DW	SSMOD-8
SSLA	DW	DOCOL
	DW	SSMOD
	DW	SWAP
	DW	DROP
	DW	SEMIS
;
	DB	85H	; M/MOD
	DB	'M/MO'
	DB	'D'+80H
	DW	SSLA-5
MSMOD	DW	DOCOL
	DW	TOR
	DW	ZERO
	DW	RR
	DW	USLAS
	DW	FROMR
	DW	SWAP
	DW	TOR
	DW	USLAS
	DW	FROMR
	DW	SEMIS
;
;	BLOCK MOVED DOWN 2 PAGES
;

        NEWPAGE
        ;;    ##   #         ###   #     # #######   ##
        ;;   #     #          #    ##    # #           #
        ;;  #      #          #    # #   # #            #
        ;;  #      #          #    #  #  # #####        #
        ;;  #      #          #    #   # # #            #
        ;;   #     #          #    #    ## #           #
        ;;    ##   #######   ###   #     # #######   ##
        ;;
        ;;     (LINE)                   n1 n2 -- addr count
        ;;          Convert the line number n1 and the screen n2 to
        ;;          the disc buffer address containing the data. A
        ;;          count of 64 indicates the full line text length.
        ;;          
        ;; : (LINE)
        ;;   >R                                 ( save screen on R stack )
        ;;                  ( a screen is a bunch of B/BUF-byte buffers. )
        ;;                  ( compute number of buffers into the screen, )
        ;;                        ( and number of bytes into the buffer. )
        ;;             ( presumes that a line does not straddle buffers. )
        ;;   0x40 B/BUF */MOD                            ( scrbuf bufchr )
        ;;   R>                                      ( scr scrbuf bufchr )
        ;;   B/SCR * +                                   ( absbuf bufchr )
        ;;   BLOCK                                      ( bufaddr bufchr )
        ;;   + 0x40 ;                                         ( len addr )
;
	DB	86H	; (LINE)
	DB	'(LINE'
	DB	')'+80H
	DW	MSMOD-8
PLINE	DW	DOCOL
	DW	TOR
	DW	LIT
	DW	40H
	DW	BBUF
	DW	SSMOD
	DW	FROMR
	DW	BSCR
	DW	STAR
	DW	PLUS
	DW	BLOCK
	DW	PLUS
	DW	LIT
	DW	40H
	DW	SEMIS

        NEWPAGE
        ;;          #          #    #    #  ######
        ;;          #          #    ##   #  #
        ;;          #          #    # #  #  #####
        ;;   ###    #          #    #  # #  #
        ;;   ###    #          #    #   ##  #
        ;;   ###    ######     #    #    #  ######
        ;; 
        ;;     .LINE          n1 n2 --
        ;;          Print on the terminal device, a line of text from
        ;;          the disc by its line and screen number. Trailing
        ;;          blanks are suppressed.
        ;; 
        ;; : .LINE
        ;;   (LINE) -TRAILING TYPE CR ;
;
	DB	85H	; .LINE
	DB	'.LIN'
	DB	'E'+80H
	DW	PLINE-9
DLINE	DW	DOCOL
	DW	PLINE
	DW	DTRAI
	DW	TYPE
	DW	SEMIS
        NEWPAGE

        ;; #     # #######  #####   #####     #     #####  #######
        ;; ##   ## #       #     # #     #   # #   #     # #
        ;; # # # # #       #       #        #   #  #       #
        ;; #  #  # #####    #####   #####  #     # #  #### #####
        ;; #     # #             #       # ####### #     # #
        ;; #     # #       #     # #     # #     # #     # #
        ;; #     # #######  #####   #####  #     #  #####  #######
        ;; 
        ;;     MESSAGE
        ;;         Print on the selected output device the text of line n
        ;;         relative to screen 4 of drive 0. n may be positive or
        ;;         negative. MESSAGE may be used to print incidental text such as
        ;;         report headers. If WARNING is zero, the message will simply be
        ;;         printed as a number (disc un-available).
        ;; 
        ;; This is the original code from the 8080 fig-FORTH sources:
        ;; 
        ;; : MESSAGE
        ;;   WARNING @ IF
        ;;     -DUP IF
        ;;       0x0004 ( BASE SCREEN NUMBER )
        ;;       OFFSET @ B/SCR / ( SCREEN OFFSET )
        ;;       - 
        ;;       .LINE CR
        ;;     ENDIF
        ;;   ELSE
        ;;     ." MSG # "
        ;;     .       
        ;;   ENDIF
        ;;
        ;; This code, unfortunately, presumes that we want to
        ;; access Screen 4 using an OFFSET of 0 blocks.
;
	DB	87H	; MESSAGE
	DB	'MESSAG'
	DB	'E'+80H
	DW	DLINE-8
MESS	DW	DOCOL
	DW	WARN
	DW	AT
	DW	ZBRAN	; IF
	DW	MESS1-$
	DW	DDUP
	DW	ZBRAN	; IF
	DW	MESS2-$
	DW	LIT
	DW	4
	DW	OFSET
	DW	AT
	DW	BSCR
	DW	SLASH
	DW	SUBB
	DW	DLINE
	DW	SPACE	; ENDIF
MESS2	DW	BRAN	; ELSE
	DW	MESS3-$
MESS1	DW	PDOTQ
	DB	6
	DB	'MSG # '
	DW	DOT	; ENDIF
MESS3	DW	SEMIS
	NEWPAGE
;------------------------------------------
;
;	8080 PORT FETCH AND STORE
;	( SELF MODIFYING CODE, NOT REENTRANT )
;
	DB	82H	; P@ "PORT @"
	DB	'P'
	DB	'@'+80H
	DW	MESS-0AH
PTAT:	DW	$+2
	POP	D	;E <- PORT#
	LXI	H,$+5
	MOV	M,E
	IN	0	;( PORT# MODIFIED )
	MOV	L,A	;L <- (PORT#)
	MVI	H,0
	JMP	HPUSH
;
	DB	82H	; "PORT STORE"
	DB	'P'
	DB	'!'+80H
	DW	PTAT-5
PTSTO:	DW	$+2
	POP	D	;E <- PORT#
	LXI	H,$+7
	MOV	M,E
	POP	H	;H <- CDATA
	MOV	A,L
	OUT	0	;( PORT# MODIFIED )
	JMP	NEXT

	NEWPAGE

;;; === === === === === === === === === === === === === === === ===
;;; VoidStar8080 Disk Interface
;;; === === === === === === === === === === === === === === === ===

BDRES	EQU	10		;RESET CONTROLLER (DRV 0, TRK 0, SEC 1)
BDDRV	EQU	11		;SELECT DRIVE
BDTRK	EQU	12		;SELECT TRACK
BDSEC	EQU	13		;SELECT SECTOR
BDDAT	EQU	14		;READ/WRITE NEXT BYTE

;	DOUBLE DENSITY 8" FLOPPY CAPACITIES
        ;; VoidStar8080: FORTH block zero is at the start of the
        ;; second track, so FORTH only sees 76 tracks.
        ;; FOR REASONS, I want the SPDRV to be a multiple of 3 KiB.
SPT2	EQU	52	; SECTORS PER TRACK
TRKS2	EQU	76	; NUMBER OF TRACKS
SPDRV2a	EQU	SPT2*TRKS2      ;raw SECTORS/DRIVE
SPDRV2s	EQU	(SPDRV2a+23)/24	;rounded-up TRIADS/DRIVE
SPDRV2	EQU	SPDRV2s*24      ;rounded up SECTORS/DRIVE

;	SINGLE DENSITY 8" FLOPPY CAPACITIES
        ;; VoidStar8080: FORTH block zero is at the start of the
        ;; second track, so FORTH only sees 76 tracks.
SPT1	EQU	26	; SECTORS/TRACK
TRKS1	EQU	76	; # TRACKS
SPDRV1a	EQU	SPT1*TRKS1      ;raw SECTORS/DRIVE
SPDRV1s	EQU	(SPDRV1a+23)/24	;rounded-up TRIADS/DRIVE
SPDRV1	EQU	SPDRV1s*24       ;rounded up SECTORS/DRIVE

BPS	EQU	128	; BYTES PER SECTOR
MXDRV	EQU	16	; MAX # DRIVES


;
;	FORTH VARIABLES AND CONSTANTS USED IN DISK INTERFACE
;
	DB	85H	; DRIVE ( CURRENT DRIVE # )
	DB	'DRIV'
	DB	'E'+80H
	DW	PTSTO-5
DRIVE	DW	DOVAR,0
;
	DB	83H	; SEC	( SECTOR # )
	DB	'SE'
	DB	'C'+80H
	DW	DRIVE-8
SEC:	DW	DOVAR
	DW	0
;
	DB	85H	; TRACK	( TRACK # )
	DB	'TRAC'
	DB	'K'+80H
	DW	SEC-6
TRACK:	DW	DOVAR,0
;
	DB	83H	; USE	( ADDR OF NEXT BUFFER TO USE )
	DB	'US'
	DB	'E'+80H
	DW	TRACK-8
USE:	DW	DOVAR
	DW	BUF1
;
	DB	84H	; PREV
;			( ADDR OF PREVIOUSLY USED BUFFER )
	DB	'PRE'
	DB	'V'+80H
	DW	USE-6
PREV	DW	DOVAR
	DW	BUF1
;
	DB	87H	; SEC/BLK ( # SECTORS/BLOCK )
	DB	'SEC/BL'
	DB	'K'+80H
	DW	PREV-7
SPBLK	DW	DOCON
	DW	KBBUF/BPS
;
	DB	85H	; #BUFF	 ( NUMBER OF BUFFERS )
	DB	'#BUF'
	DB	'F'+80H
	DW	SPBLK-10
NOBUF	DW	DOCON,NBUF
;
	DB	87H	; DENSITY ( 0 = SINGLE , 1 = DOUBLE )
	DB	'DENSIT'
	DB	'Y'+80H
	DW	NOBUF-8
DENSTY	DW	DOVAR
	DW	0
;
	DB	8AH	; DISK-ERROR  ( DISK ERROR STATUS )
	DB	'DISK-ERRO'
	DB	'R'+80H
	DW	DENSTY-10
DSKERR	DW	DOVAR,0
;
;	DISK INTERFACE HIGH-LEVEL ROUTINES
;
	DB	84H	; +BUF	( ADVANCE BUFFER )
	DB	'+BU'
	DB	'F'+80H
	DW	DSKERR-13
PBUF	DW	DOCOL
	DW	LIT,CO
	DW	PLUS,DUP
	DW	LIMIT,EQUAL
        DW      ZBRAN
        DW      PBUF1-$
	DW	DROP,FIRST
PBUF1:	DW	DUP,PREV
	DW	AT,SUBB
	DW	SEMIS
;
	DB	86H	; UPDATE
	DB	'UPDAT'
	DB	'E'+80H
	DW	PBUF-7
UPDAT	DW	DOCOL,PREV
	DW	AT,AT
	DW	LIT,8000H
	DW	ORR
	DW	PREV,AT
	DW	STORE,SEMIS
;
	DB	8DH	; EMPTY-BUFFERS
	DB	'EMPTY-BUFFER'
	DB	'S'+80H
	DW	UPDAT-9
MTBUF	DW	DOCOL,FIRST
	DW	LIMIT,OVER
	DW	SUBB,ERASEE
	DW	SEMIS
;
	DB	83H	; DR0
	DB	'DR'
	DB	'0'+80H
	DW	MTBUF-16
DRZER	DW	DOCOL,ZERO
	DW	OFSET,STORE
	DW	SEMIS
;
	DB	83H	; DR1
	DB	'DR'
	DB	'1'+80H
	DW	DRZER-6
DRONE	DW	DOCOL
	DW	DENSTY,AT
        DW      ZBRAN
        DW      DRON1-$
	DW	LIT,SPDRV2
        DW      BRAN
        DW      DRON2-$
DRON1	DW	LIT,SPDRV1
DRON2	DW	OFSET,STORE
	DW	SEMIS
;
	DB	86H	; BUFFER
	DB	'BUFFE'
	DB	'R'+80H
	DW	DRONE-6
BUFFE:	DW	DOCOL,USE
	DW	AT,DUP
	DW	TOR
BUFF1	DW	PBUF		; WON'T WORK IF SINGLE BUFFER
        DW      ZBRAN
        DW      BUFF1-$
	DW	USE,STORE
	DW	RR,AT
	DW	ZLESS
        DW      ZBRAN
        DW      BUFF2-$
	DW	RR,TWOP
	DW	RR,AT
	DW	LIT,7FFFH
	DW	ANDD,ZERO
	DW	RSLW
BUFF2	DW	RR,STORE
	DW	RR,PREV
	DW	STORE,FROMR
	DW	TWOP,SEMIS

        NEWPAGE
        ;;  #####   #        ####    ####   #    #
        ;;  #    #  #       #    #  #    #  #   #
        ;;  #####   #       #    #  #       ####
        ;;  #    #  #       #    #  #       #  #
        ;;  #    #  #       #    #  #    #  #   #
        ;;  #####   ######   ####    ####   #    #
        ;; 
        ;;     BLOCK          n -- addr                     191 
        ;;          Leave the address of the first byte in block n.   If the block 
        ;;          is not already in memory,  it is transferred from mass storage 
        ;;          into whichever memory buffer has been least recently accessed. 
        ;;          If  the  block occupying that buffer has  been  UPDATEd  (i.e. 
        ;;          modified), it is rewritten onto mass storage before block n is 
        ;;          read  into the buffer.   n is an unsigned number.   If correct 
        ;;          mass storage read or write is not possible, an error condition 
        ;;          exists.  Only data within the latest block referenced by BLOCK 
        ;;          is valid by byte address, due to sharing of the block buffers. 
        ;; 
        ;; : BLOCK
        ;;   OFFSET @ + >R
        ;;   PREV @ DUP @ R - DUP + IF
        ;;     BEGIN
        ;;       +BUF 0= IF
        ;;         DROP 
        ;;         R BUFFER 
        ;;         DUP R 1 R/W 
        ;;         2 -
        ;;       ENDIF
        ;;       DUP @ R - DUP + 0=
        ;;     UNTIL
        ;;     DUP PREV !
        ;;   ENDIF
        ;;   R> DROP 2+ ;S
;
	DB	85H	; BLOCK
	DB	'BLOC'
	DB	'K'+80H
	DW	BUFFE-9
BLOCK	DW	DOCOL,OFSET
	DW	AT,PLUS
	DW	TOR,PREV
	DW	AT,DUP
	DW	AT,RR
	DW	SUBB
	DW	DUP,PLUS
        DW      ZBRAN
        DW      BLOC1-$
BLOC2	DW	PBUF,ZEQU
        DW      ZBRAN
        DW      BLOC3-$
	DW	DROP,RR
	DW	BUFFE,DUP
	DW	RR,ONE
	DW	RSLW
	DW	TWO,SUBB
BLOC3	DW	DUP,AT
	DW	RR,SUBB
	DW	DUP,PLUS
	DW	ZEQU
        DW      ZBRAN
        DW      BLOC2-$
	DW	DUP,PREV
	DW	STORE
BLOC1	DW	FROMR,DROP
	DW	TWOP,SEMIS
;
;
;	VoidStar8080 INTERFACE ROUTINES
;	modeled on CP/M INTERFACE ROUTINES
;	intended to run without BIOS or ROM support
;
	DB	86H	; SET-IO
;			( ASSIGN SECTOR, TRACK FOR BDOS )
	DB	'SET-I'
	DB	'O'+80H
	DW	BLOCK-8
SETIO:	DW	$+2

	;; === === === === === === === === === === === ===
	LDA	SEC+2
	OUT	BDSEC		;VoidStar8080: set sector number (1..26 or 1..52)
	LDA	TRACK+2
        INR     A              ;FORTH does not access Track Zero.
	OUT	BDTRK		;VoidStar8080: set track number (0..76)
	;; === === === === === === === === === === === ===

	JMP	NEXT
;
	DB	89H	; SET-DRIVE
	DB	'SET-DRIV'
	DB	'E'+80H
	DW	SETIO-9
SETDRV:	DW	$+2

	;; === === === === === === === === === === === ===
	LDA	DRIVE+2	; (A) <- (DRIVE) = DRIVE #
	OUT	BDDRV		;VoidStar8080: set drive number
	;; === === === === === === === === === === === ===

	JMP	NEXT

        NEWPAGE
        ;; #######   ##     #####   #####     #    #        #####
        ;;    #     #  #   #     # #     #   # #   #       #     #
        ;;    #      ##    #       #        #   #  #       #
        ;;    #     ###     #####  #       #     # #       #
        ;;    #    #   # #       # #       ####### #       #
        ;;    #    #    #  #     # #     # #     # #       #     #
        ;;    #     #### #  #####   #####  #     # #######  #####
        ;; 
        ;; T&SCALC         ( CALCULATES DRIVE#, TRACK#, & SECTOR# )
        ;; STACK INPUT: SECTOR-DISPLACEMENT = BLK# * SEC/BLK
        ;; OUTPUT: VARIABLES DRIVE, TRACK, & SEC
        ;;
        ;; The boot image of forth uses 128 bytes for the boot sector
        ;; plus the current image size of FORTH, which adds up to just
        ;; a few bytes short of filling the first track of a DOUBLE
        ;; density disk.
        ;;
        ;; The normal disk access primative for VoidStar8080
        ;; increments the track number, so Track 0 is reserved,
        ;; placing Screen 0 at the start of the second track.
        ;;
        ;; Use ‹DR0› to access screens on the first drive.
        ;; Use ‹DR1› to access screens on the second drive.
        ;;
        ;; Rewrite T&SCALC for more complicated systems.
        ;; 
        ;; : T&SCALC
        ;;   DENSITY @ IF
        ;;     SPDRV2 /MOD 0x10 MIN
        ;;     DUP DRIVE @ = IF
        ;;       DROP
        ;;     ELSE
        ;;       DRIVE ! SET-DRIVE
        ;;     ENDIF
        ;;     0x34 /MOD TRACK ! 1+ SEC !
        ;;   ELSE
        ;;     SPDRV1 /MOD 0x10 MIN 
        ;;     DUP DRIVE @ = IF
        ;;       DROP
        ;;     ELSE
        ;;       DRIVE ! SET-DRIVE
        ;;     ENDIF
        ;;     0x1A /MOD TRACK ! 1+ SEC !
        ;;   ENDIF ;        ;; 
        ;; NOTE: in reality, a ;S is placed at the ELSE
        ;; for density, not a BRANCH to the ENDIF.
;
;	T&SCALC		( CALCULATES DRIVE#, TRACK#, & SECTOR# )
;	STACK INPUT: SECTOR-DISPLACEMENT = BLK# * SEC/BLK
;	OUTPUT: VARIABLES DRIVE, TRACK, & SEC
;
	DB	87H	; T&SCALC
	DB	'T&SCAL'
	DB	'C'+80H
	DW	SETDRV-12
TSCALC:	DW	DOCOL,DENSTY
	DW	AT
        DW      ZBRAN
        DW      TSCALS-$
	DW	LIT,SPDRV2
	DW	SLMOD
	DW	LIT,MXDRV
	DW	MIN
	DW	DUP,DRIVE
	DW	AT,EQUAL
        DW      ZBRAN
        DW      TSCAL1-$
	DW	DROP
        DW      BRAN
        DW      TSCAL2-$
TSCAL1	DW	DRIVE,STORE
	DW	SETDRV
TSCAL2	DW	LIT,SPT2
	DW	SLMOD,TRACK
	DW	STORE,ONEP
	DW	SEC,STORE
	DW	SEMIS
;	SINGLE DENSITY
TSCALS	DW	LIT,SPDRV1
	DW	SLMOD
	DW	LIT,MXDRV
	DW	MIN
	DW	DUP,DRIVE
	DW	AT,EQUAL
        DW      ZBRAN
        DW      TSCAL3-$
	DW	DROP
        DW      BRAN
        DW      TSCAL4-$
TSCAL3	DW	DRIVE,STORE
	DW	SETDRV
TSCAL4	DW	LIT,SPT1
	DW	SLMOD,TRACK
	DW	STORE,ONEP
	DW	SEC,STORE
	DW	SEMIS
;
;	SEC-READ
;	( READ A SECTOR SETUP BY 'SET-DRIVE' & 'SETIO' )
;
	DB	88H	; SEC-READ
	DB	'SEC-REA'
	DB	'D'+80H
	DW	TSCALC-10
SECRD	DW	$+2

	;; === === === === === === === === === === === ===
	;; VoidStar8080 sector READ:
	;;   DBDRV, DBTRK, DBSEC ports have been set.
	LHLD	USE+2	; (HL) <- ADDR BUFFER
	MVI	E,BPS
SECRL	IN	BDDAT
	MOV	M,A
	INX	H
	DCR	E
	JNZ	SECRL
	;; === === === === === === === === === === === ===

	JMP	NEXT
;
;	SEC-WRITE
;	( WRITE A SECTOR SETUP BY 'SET-DRIVE' & 'SETIO' )
;
	DB	89H	; SEC-WRITE
	DB	'SEC-WRIT'
	DB	'E'+80H
	DW	SECRD-11
SECWT	DW	$+2

	;; === === === === === === === === === === === ===
	;; VoidStar8080 sector write:
	;;   DBDRV, DBTRK, DBSEC ports have been set.
	LHLD	USE+2	; (HL) <- ADDR BUFFER
	MVI	E,BPS
SECWL	MOV	A,M
	INX	H
	OUT	BDDAT
	DCR	E
	JNZ	SECWL
	;; === === === === === === === === === === === ===

	JMP	NEXT

        NEWPAGE
        ;; ######        # #     #
        ;; #     #      #  #  #  #
        ;; #     #     #   #  #  #
        ;; ######     #    #  #  #
        ;; #   #     #     #  #  #
        ;; #    #   #      #  #  #
        ;; #     # #        ## ##
        ;; 
        ;;     R/W                 addr blk f ---
        ;;         The fig-FORTH standard disc read-write linkage. addr specifies
        ;;         the source or destination block buffer. blk is the sequential
        ;;         number of the referenced block; and f is a flag for f=0 write
        ;;         and f=1 read. R/W determines the location on mass storage,
        ;;         performs the read-write and performs any error checking.
        ;; 
        ;; : R/W
        ;;   USE @ >R 
        ;;   SWAP SEC/BLK * ROT USE !
        ;;   SEC/BLK 0 DO
        ;;     OVER OVER T&SCALC SET-IO 
        ;;     IF SEC-READ ELSE SEC-WRITE ENDIF
        ;;     1+ 0x80 USE +! 
        ;;   LOOP
        ;;   DROP DROP
        ;;   R> USE ! ;
;
	DB	83H	; R/W	( FORTH DISK PRIMATIVE )
	DB	'R/'
	DB	'W'+80H
	DW	SECWT-12
RSLW	DW	DOCOL
	DW	USE,AT
	DW	TOR
	DW	SWAP,SPBLK
	DW	STAR,ROT
	DW	USE,STORE
	DW	SPBLK,ZERO
	DW	XDO
RSLW1	DW	OVER,OVER
	DW	TSCALC,SETIO
        DW      ZBRAN
        DW      RSLW2-$
	DW	SECRD
        DW      BRAN
        DW      RSLW3-$
RSLW2	DW	SECWT
RSLW3	DW	ONEP
	DW	LIT,80H
	DW	USE,PSTOR
        DW      XLOOP
        DW      RSLW1-$
	DW	DROP,DROP
	DW	FROMR,USE
	DW	STORE,SEMIS
;
;--------------------------------------------------------
;
;	ALTERNATIVE R/W FOR NO DISK INTERFACE
;
;RSLW	DW	DOCOL,DROP,DROP,DROP,SEMIS
;
;--------------------------------------------------------
;
	DB	85H	; FLUSH
	DB	'FLUS'
	DB	'H'+80H
	DW	RSLW-6
FLUSH	DW	DOCOL
	DW	NOBUF,ONEP
	DW	ZERO,XDO
FLUS1	DW	ZERO,BUFFE
	DW	DROP
        DW      XLOOP
        DW      FLUS1-$
	DW	SEMIS
;
	DB	84H	; LOAD
	DB	'LOA'
	DB	'D'+80H
	DW	FLUSH-8
LOAD	DW	DOCOL,BLK
	DW	AT,TOR
	DW	INN,AT
	DW	TOR,ZERO
	DW	INN,STORE
	DW	BSCR,STAR
	DW	BLK,STORE	; BLK <- SCR * B/SCR
	DW	INTER		; INTERPRET FROM OTHER SCREEN
	DW	FROMR,INN
	DW	STORE
	DW	FROMR,BLK
	DW	STORE
	DW	SEMIS
;
	DB	0C3H	; -->
	DB	'--'
	DB	'>'+80H
	DW	LOAD-7
ARROW	DW	DOCOL
	DW	QLOAD
	DW	ZERO
	DW	INN
	DW	STORE
	DW	BSCR
	DW	BLK
	DW	AT
	DW	OVER
	DW	MODD
	DW	SUBB
	DW	BLK
	DW	PSTOR
	DW	SEMIS
;
	NEWPAGE

;;; === === === === === === === === === === === === === === === ===
;;; VoidStar8080 Serial Device Ports
;;; === === === === === === === === === === === === === === === ===

TTYD	EQU	0		;READ/WRITE DATA FROM/TO PRINTING CONSOLE
TTYC	EQU	1		;PRINTING CONSOLE STATUS/CONTROL PORT
CRTD	EQU	2		;READ/WRITE DATA FROM/TO PRINTING CONSOLE
CRTC	EQU	3		;PRINTING CONSOLE STATUS/CONTROL PORT
PPTD	EQU	4		;READ/WRITE DATA FROM/TO HIGH SPEED PAPER TAPE
PPTC	EQU	5		;HIGH SPEED PAPER TAPE STATUS/CONTROL PORT
LPTD	EQU	6		;WRITE DATA TO LINE PRINTER
LPTC	EQU	7		;LINE PRINTER STATUS/CONTROL PORT
;
CDEV_CN EQU     01h             ;CDEV status: connected
CDEV_RX EQU     02h             ;CDEV status: receive data ready
CDEV_TX EQU     04h             ;CDEV status: ready to transmit

;
EPRINT	DW	0	; ENABLE PRINTER VARIABLE
;			; 0 = DISABLED, 1 = ENABLED
;

	;; === === === === === === === === === === === ===
	;; VoidStar8080 CSTAT: A=FF if char ready on console, else A=00
	;; do not modify BC.
CSTAT	IN	TTYC
        ANI     CDEV_RX
	RZ			;RETURN 00 IF NO DATA AVAILABLE
	MVI	A,0FFH		;RETURN FF IF DATA IS AVAILABLE
	RET
	;; === === === === === === === === === === === ===
;
	;; === === === === === === === === === === === ===
	;; VoidStar8080 CIN: A=next char from console
	;; if no char available, wait for one
	;; do not modify BC
CIN
;;	IN	TTYC
;;      ANI     CDEV_RX
;;	JZ	CIN		;WAIT FOR DATA AVAILABLE
	IN	TTYD
	ANI	7FH		;STRIP THE PARITY BIT
	RET			;RETURN WITH CHAR IN (A)
	;; === === === === === === === === === === === ===
;
	;; === === === === === === === === === === === ===
	;; VoidStar8080 COUT: send (C) to the console outputC
	;; if necessary, wait until the channel is available
COUT
;;	IN	TTYC
;;      ANI     CDEV_TX
;;	JZ	COUT
	MOV	A,C
        ANI     7Fh
	OUT	TTYD
	RET
	;; === === === === === === === === === === === ===
;
	;; === === === === === === === === === === === ===
	;; VoidStar8080 POUT: send (C) to the printer output
	;; if necessary, wait until the channel is available
POUT	IN	LPTC
        ANI     CDEV_TX
	JZ	POUT
	MOV	A,C
	OUT	LPTD
	RET
	;; === === === === === === === === === === === ===
;
CPOUT	CALL	COUT	; OUTPUT (C) TO CONSOLE
	XCHG
	LXI	H,EPRINT
	MOV	A,M	; IF (EPRINT) <> 0
	ORA	A
	JZ	CPOU1
	MOV	C,E	; THEN OUTPUT (C) TO PRINTER
	CALL	POUT
CPOU1	RET
;
;	FORTH TO CP/M-LIKE SERIAL IO INTERFACE
;
PQTER	CALL	CSTAT	; IF CHR TYPED
	LXI	H,0
	ORA	A
	JZ	PQTE1
	INR	L	; THEN (S1) <- TRUE
PQTE1	JMP	HPUSH	; ELSE (S1) <- FALSE
;
PKEY	CALL	CIN	; READ CHR FROM CONSOLE
	CPI	DLE	; IF CHR = (^P)
	MOV	E,A
	JNZ	PKEY1
	LXI	H,EPRINT  ; THEN TOGGLE (EPRINT)LSB
	MVI	E,ABL	; CHR <- BLANK
	MOV	A,M
	XRI	1
	MOV	M,A
PKEY1	MOV	L,E
	MVI	H,0
	JMP	HPUSH	; (S1)LB <- CHR

        NEWPAGE
        ;;    ##   ####### #     #   ###   #######   ##
        ;;   #     #       ##   ##    #       #        #
        ;;  #      #       # # # #    #       #         #
        ;;  #      #####   #  #  #    #       #         #
        ;;  #      #       #     #    #       #         #
        ;;   #     #       #     #    #       #        #
        ;;    ##   ####### #     #   ###      #      ##
        ;;
        ;;     (EMIT)         ch --
        ;;          Send the character on the data stack to the console output.
        ;;
        ;;      IMPL NOTES:
        ;;          In stock FIG-FORTH this is an orphan word (lacking
        ;;          name and link fields). This implementation makes
        ;;          this word searchable by adding a name field and a
        ;;          link field, and inserting it into the dictionary
        ;;          before ARROW (above) and after TICK (below).
        ;;
        ;;      CODE (EMIT)
        ;;        H POP, B PUSH, L C MOV, CPOUT CALL, B POP, NEXT JMP,
;
        DB      86h             ; (EMIT)
        DB      '(EMIT'
        DB      ')'+80H
        DW      ARROW-6
PEMIT   DW      $+2
	POP	H	; (L) <- (S1)LB = CHR
	PUSH	B	; SAVE (IP)
	MOV	C,L
	CALL	CPOUT	; OUTPUT CHR TO CONSOLE
;			; & MAYBE PRINTER
	POP	B	; RESTORE (IP)
	JMP	NEXT
;
PCR	PUSH	B	; SAVE (IP)
	MVI	C,ACR	; OUTPUT (CR) TO CONSOLE
	MOV	L,C
	CALL	CPOUT	; & MAYBE TO PRINTER
	MVI	C,LF	; OUTPUT (LF) TO CONSOLE
	MOV	L,C
	CALL	CPOUT	; & MAYBE TO PRINTER
	POP	B	; RESTORE (IP)
	JMP	NEXT
;
;----------------------------------------------------
	NEWPAGE
;
	DB	0C1H	; '	( TICK )
	DB	0A7H
        DW      PEMIT-9
TICK	DW	DOCOL
	DW	DFIND
	DW	ZEQU
	DW	ZERO
	DW	QERR
	DW	DROP
	DW	LITER
	DW	SEMIS
;
	DB	86H	; FORGET
	DB	'FORGE'
	DB	'T'+80H
	DW	TICK-4
FORG	DW	DOCOL
	DW	CURR
	DW	AT
	DW	CONT
	DW	AT
	DW	SUBB
	DW	LIT
	DW	18H
	DW	QERR
	DW	TICK
	DW	DUP
	DW	FENCE
	DW	AT
	DW	LESS
	DW	LIT
	DW	15H
	DW	QERR
	DW	DUP
	DW	NFA
	DW	DP
	DW	STORE
	DW	LFA
	DW	AT
	DW	CONT
	DW	AT
	DW	STORE
	DW	SEMIS
;
	DB	84H	; BACK
	DB	'BAC'
	DB	'K'+80H
	DW	FORG-9
BACK	DW	DOCOL
	DW	HERE
	DW	SUBB
	DW	COMMA
	DW	SEMIS
;
	DB	0C5H	; BEGIN
	DB	'BEGI'
	DB	'N'+80H
	DW	BACK-7
BEGIN	DW	DOCOL
	DW	QCOMP
	DW	HERE
	DW	ONE
	DW	SEMIS
;
	DB	0C5H	; ENDIF
	DB	'ENDI'
	DB	'F'+80H
	DW	BEGIN-8
ENDIFF	DW	DOCOL
	DW	QCOMP
	DW	TWO
	DW	QPAIR
	DW	HERE
	DW	OVER
	DW	SUBB
	DW	SWAP
	DW	STORE
	DW	SEMIS
;
	DB	0C4H	; THEN
	DB	'THE'
	DB	'N'+80H
	DW	ENDIFF-8
THEN	DW	DOCOL
	DW	ENDIFF
	DW	SEMIS
;
	DB	0C2H	; DO
	DB	'D'
	DB	'O'+80H
	DW	THEN-7
DO	DW	DOCOL
	DW	COMP
	DW	XDO
	DW	HERE
	DW	THREE
	DW	SEMIS
;
	DB	0C4H	; LOOP
	DB	'LOO'
	DB	'P'+80H
	DW	DO-5
LOOP	DW	DOCOL
	DW	THREE
	DW	QPAIR
	DW	COMP
	DW	XLOOP
	DW	BACK
	DW	SEMIS
;
	DB	0C5H	; +LOOP
	DB	'+LOO'
	DB	'P'+80H
	DW	LOOP-7
PLOOP	DW	DOCOL
	DW	THREE
	DW	QPAIR
	DW	COMP
	DW	XPLOO
	DW	BACK
	DW	SEMIS
;
	DB	0C5H	; UNTIL
	DB	'UNTI'
	DB	'L'+80H
	DW	PLOOP-8
UNTIL	DW	DOCOL
	DW	ONE
	DW	QPAIR
	DW	COMP
	DW	ZBRAN
	DW	BACK
	DW	SEMIS
;
	DB	0C3H	; END
	DB	'EN'
	DB	'D'+80H
	DW	UNTIL-8
ENDD	DW	DOCOL
	DW	UNTIL
	DW	SEMIS
;
	DB	0C5H	; AGAIN
	DB	'AGAI'
	DB	'N'+80H
	DW	ENDD-6
AGAIN	DW	DOCOL
	DW	ONE
	DW	QPAIR
	DW	COMP
	DW	BRAN
	DW	BACK
	DW	SEMIS
;
	DB	0C6H	; REPEAT
	DB	'REPEA'
	DB	'T'+80H
	DW	AGAIN-8
REPEA	DW	DOCOL
	DW	TOR
	DW	TOR
	DW	AGAIN
	DW	FROMR
	DW	FROMR
	DW	TWO
	DW	SUBB
	DW	ENDIFF
	DW	SEMIS
;
	DB	0C2H	; IF
	DB	'I'
	DB	'F'+80H
	DW	REPEA-9
IFF	DW	DOCOL
	DW	COMP
	DW	ZBRAN
	DW	HERE
	DW	ZERO
	DW	COMMA
	DW	TWO
	DW	SEMIS
;
	DB	0C4H	; ELSE
	DB	'ELS'
	DB	'E'+80H
	DW	IFF-5
ELSEE	DW	DOCOL
	DW	TWO
	DW	QPAIR
	DW	COMP
	DW	BRAN
	DW	HERE
	DW	ZERO
	DW	COMMA
	DW	SWAP
	DW	TWO
	DW	ENDIFF
	DW	TWO
	DW	SEMIS
;
	DB	0C5H	; WHILE
	DB	'WHIL'
	DB	'E'+80H
	DW	ELSEE-7
WHILE	DW	DOCOL
	DW	IFF
	DW	TWOP
	DW	SEMIS
;
	DB	86H	; SPACES
	DB	'SPACE'
	DB	'S'+80H
	DW	WHILE-8
SPACS	DW	DOCOL
	DW	ZERO
	DW	MAX
	DW	DDUP
	DW	ZBRAN	; IF
	DW	SPAX1-$
	DW	ZERO
	DW	XDO	; DO
SPAX2	DW	SPACE
	DW	XLOOP	; LOOP	ENDIF
	DW	SPAX2-$
SPAX1	DW	SEMIS
;
	DB	82H	; <#
	DB	'<'
	DB	'#'+80H
	DW	SPACS-9
BDIGS	DW	DOCOL
	DW	PAD
	DW	HLD
	DW	STORE
	DW	SEMIS
;
	DB	82H	; #>
	DB	'#'
	DB	'>'+80H
	DW	BDIGS-5
EDIGS	DW	DOCOL
	DW	DROP
	DW	DROP
	DW	HLD
	DW	AT
	DW	PAD
	DW	OVER
	DW	SUBB
	DW	SEMIS
;
	DB	84H	; SIGN
	DB	'SIG'
	DB	'N'+80H
	DW	EDIGS-5
SIGN	DW	DOCOL
	DW	ROT
	DW	ZLESS
	DW	ZBRAN	; IF
	DW	SIGN1-$
	DW	LIT
	DW	2DH
	DW	HOLD	; ENDIF
SIGN1	DW	SEMIS
;
	DB	81H	; #
	DB	'#'+80H
	DW	SIGN-7
DIG	DW	DOCOL
	DW	BASE
	DW	AT
	DW	MSMOD
	DW	ROT
	DW	LIT
	DW	9
	DW	OVER
	DW	LESS
	DW	ZBRAN	; IF
	DW	DIG1-$
	DW	LIT
	DW	7
	DW	PLUS	; ENDIF
DIG1	DW	LIT
	DW	30H
	DW	PLUS
	DW	HOLD
	DW	SEMIS
;
	DB	82H	; #S
	DB	'#'
	DB	'S'+80H
	DW	DIG-4
DIGS	DW	DOCOL
DIGS1	DW	DIG	; BEGIN
	DW	OVER
	DW	OVER
	DW	ORR
	DW	ZEQU
	DW	ZBRAN	; UNTIL
	DW	DIGS1-$
	DW	SEMIS
;
	DB	83H	; D.R
	DB	'D.'
	DB	'R'+80H
	DW	DIGS-5
DDOTR	DW	DOCOL
	DW	TOR
	DW	SWAP
	DW	OVER
	DW	DABS
	DW	BDIGS
	DW	DIGS
	DW	SIGN
	DW	EDIGS
	DW	FROMR
	DW	OVER
	DW	SUBB
	DW	SPACS
	DW	TYPE
	DW	SEMIS
;
	DB	82H	; .R
	DB	'.'
	DB	'R'+80H
	DW	DDOTR-6
DOTR	DW	DOCOL
	DW	TOR
	DW	STOD
	DW	FROMR
	DW	DDOTR
	DW	SEMIS
;
	DB	82H	; D.
	DB	'D'
	DB	'.'+80H
	DW	DOTR-5
DDOT	DW	DOCOL
	DW	ZERO
	DW	DDOTR
	DW	SPACE
	DW	SEMIS
;
	DB	81H	; .
	DB	'.'+80H
	DW	DDOT-5
DOT	DW	DOCOL
	DW	STOD
	DW	DDOT
	DW	SEMIS
;
	DB	81H	; ?
	DB	'?'+80H
	DW	DOT-4
QUES	DW	DOCOL
	DW	AT
	DW	DOT
	DW	SEMIS
;
	DB	82H	; U.
	DB	'U'
	DB	'.'+80H
	DW	QUES-4
UDOT	DW	DOCOL
	DW	ZERO
	DW	DDOT
	DW	SEMIS
;
	DB	85H	; VLIST
	DB	'VLIS'
	DB	'T'+80H
	DW	UDOT-5
VLIST	DW	DOCOL
	DW	LIT
	DW	80H
	DW	OUTT
	DW	STORE
	DW	CONT
	DW	AT
	DW	AT
VLIS1	DW	OUTT	; BEGIN
	DW	AT
	DW	CSLL
	DW	GREAT
	DW	ZBRAN	; IF
	DW	VLIS2-$
	DW	CR
	DW	ZERO
	DW	OUTT
	DW	STORE	; ENDIF
VLIS2	DW	DUP
	DW	IDDOT
	DW	SPACE
	DW	SPACE
	DW	PFA
	DW	LFA
	DW	AT
	DW	DUP
	DW	ZEQU
	DW	QTERM
	DW	ORR
	DW	ZBRAN	; UNTIL
	DW	VLIS1-$
	DW	DROP
	DW	SEMIS
;
	;; === === === === === === === === === === === ===
	;; EXIT to VoidStar8080 Monitor
	DB	83H	; BYE
	DB	'BY'
	DB	'E'+80H
	DW	VLIST-8
BYE	DW	$+2
	JMP	ROMBASE
	;; === === === === === === === === === === === ===
        ;;          

        NEWPAGE
        ;;  #          #     ####    #####
        ;;  #          #    #          #
        ;;  #          #     ####      #
        ;;  #          #         #     #
        ;;  #          #    #    #     #
        ;;  ######     #     ####      #
        ;; 
        ;;     LIST           n --                          109 
        ;;          List  the ASCII symbolic contents of screen n on  the  current 
        ;;          output device, setting SCR to contain n.  n is unsigned. 
        ;;          
        ;;     IMPL NOTE:
        ;;          Screen numbers above 32767 are displayed as negative.
        ;;          This is not an issue as we do not anticipate
        ;;          having 32 MiB of mass storage. If we did, we would
        ;;          certainly want a better file system than just a
        ;;          linear sequence of numbered screens.
        ;; 
        ;; : LIST
        ;;   DECIMAL CR DUP SCR !
        ;;   ." SCR # " .
        ;;   0x10 0 DO
        ;;     CR I 3 .R SPACE
        ;;     I SCR @ .LINE
        ;;     ?TERMINAL IF LEAVE THEN
        ;;   LOOP
        ;;   CR ;
;
	DB	84H	; LIST
	DB	'LIS'
	DB	'T'+80H
	DW	BYE-6
LIST	DW	DOCOL,DEC
	DW	CR,DUP
	DW	SCR,STORE
	DW	PDOTQ
	DB	6,'SCR # '
	DW	DOT
	DW	LIT,10H
	DW	ZERO,XDO
LIST1	DW	CR,IDO
	DW	LIT,3
	DW	DOTR,SPACE
	DW	IDO,SCR
	DW	AT,DLINE
	DW	QTERM		; ?TERMINAL
        DW      ZBRAN           ; IF
        DW      LIST2-$
	DW	LEAVE		; LEAVE
LIST2   DW      XLOOP           ; ENDIF
        DW      LIST1-$
	DW	CR,SEMIS
;
	DB	85H	; INDEX
	DB	'INDE'
	DB	'X'+80H
	DW	LIST-7
INDEX	DW	DOCOL
	DW	LIT,FF
	DW	EMIT,CR
	DW	ONEP,SWAP
	DW	XDO
INDE1	DW	CR,IDO
	DW	LIT,3
	DW	DOTR,SPACE
	DW	ZERO,IDO
	DW	DLINE,QTERM
        DW      ZBRAN
        DW      INDE2-$
	DW	LEAVE
INDE2   DW      XLOOP
        DW      INDE1-$
	DW	SEMIS
;
	DB	85H	; TRIAD
	DB	'TRIA'
	DB	'D'+80H
	DW	INDEX-8
TRIAD	DW	DOCOL
	DW	LIT,FF
	DW	EMIT
	DW	LIT,3
	DW	SLASH
	DW	LIT,3
	DW	STAR
	DW	LIT,3
	DW	OVER,PLUS
	DW	SWAP,XDO
TRIA1	DW	CR,IDO
	DW	LIST
	DW	QTERM		; ?TERMINAL
        DW      ZBRAN           ; IF
        DW      TRIA2-$
	DW	LEAVE		; LEAVE
TRIA2   DW      XLOOP           ; ENDIF
        DW      TRIA1-$
	DW	CR
	DW	LIT,15
	DW	MESS,CR
	DW	SEMIS
;
	DB	84H	; .CPU
	DB	'.CP'
	DB	'U'+80H
	DW	TRIAD-8
DOTCPU	DW	DOCOL
	DW	BASE,AT
	DW	LIT,36
	DW	BASE,STORE
	DW	LIT,22H
	DW	PORIG,TAT
	DW	DDOT
	DW	BASE,STORE
	DW	SEMIS
;
	DB	84H	; TASK
	DB	'TAS'
	DB	'K'+80H
	DW	DOTCPU-7
TASK	DW	DOCOL
	DW	SEMIS
;
INITDP:	DS	EM-$	;CONSUME MEMORY TO LIMIT
;
	NEWPAGE
;
;		MEMORY MAP
;	( THE FOLLOWING EQUATES ARE NOT REFERENCED ELSEWHERE )
;
;		LOCATION	CONTENTS
;		--------	--------
MCOLD	EQU	ORIG		;JMP TO COLD START
MWARM	EQU	ORIG+4		;JMP TO WARM START
MA2	EQU	ORIG+8		;COLD START PARAMETERS
MUP	EQU	UP		;USER VARIABLES' BASE 'REG'
MRP	EQU	RPP		;RETURN STACK 'REGISTER'
;
MBIP	EQU	BIP		;DEBUG SUPPORT
MDPUSH	EQU	DPUSH		;ADDRESS INTERPRETER
MHPUSH	EQU	HPUSH
MNEXT	EQU	NEXT
;
MDP0	EQU	DP0		;START FORTH DICTIONARY
MDIO	EQU	DRIVE		  ;BDEV INTERFACE
MCIO	EQU	EPRINT		  ;CDEV INTERFACE
MIDP	EQU	INITDP		;END INITIAL FORTH DICTIONARY
;				  = COLD (DP) VALUE
;				  = COLD (FENCE) VALUE
;				  |  NEW
;				  |  DEFINITIONS
;				  V
;
;				  ^
;				  |  DATA
;				  |  STACK
MIS0	EQU	INITS0		;  = COLD (SP) VALUE = (S0)
;				   = (TIB)
;				  |  TERMINAL INPUT
;				  |  BUFFER
;				  V
;
;				  ^
;				  |  RETURN
;				  |  STACK
MIR0	EQU	INITR0		;START USER VARIABLES
;				  = COLD (RP) VALUE = (R0)
;				  = (UP)
;				;END USER VARIABLES
MFIRST	EQU	BUF1		;START DISK BUFFERS
;				  = FIRST
MEND	EQU	EM-1		;END DISK BUFFERS
MLIMIT	EQU	EM		;LAST MEMORY LOC USED + 1
;				  = LIMIT
;
; The bootable image is 128 bytes of boot sector,
; followed by the forth image.
BSIZE	EQU     128+INITDP-ORIG
BSECT	EQU     (BSIZE+127)/128
BSRU	EQU     BSECT*128
BSPAD	EQU     BSRU-BSIZE
BKIB	EQU     (BSIZE+1023)/1024
BKRU    EQU     BKIB*1024
BKPAD	EQU     BKRU-BSIZE
	END	ORIG
