	TITLE   'SKELETAL CBIOS FOR FIRST LEVEL OF CP/M ALTERATION'
;
;	NOTE: MSIZE DETERMINES WHERE THIS BIOS IS LOCATED.
MSIZE	EQU	62		;CP/M VERSION MEMORY SIZE IN KILOBYTES
PATCH	EQU	(MSIZE*1024)-(2*256)  ;START OF THE CBIOS PATCH
;
	ORG	PATCH		;ORIGIN OF THIS PROGRAM
;
CBASE	EQU	(MSIZE-16)*1024 ;BIAS FOR SYSTEMS LARGER THAN 16K
CPMB	EQU	CBASE+2900H	;BASE OF CP/M (= BASE OF CCP)
BDOS	EQU	CBASE+3106H	;BASE OF RESIDENT PORTION OF CP/M
CPML	EQU	$-CPMB		;LENGTH OF THE CP/M SYSTEM IN BYTES
NSECTS	EQU	CPML/128	;NUMBER OF SECTORS TO LOAD ON WARM START
;
;	PAGE ZERO STUFF:
;
IOBYTE	EQU	0003H
CDISK	EQU	0004H
;
SCRAT	EQU	40H	;USE THE AREA RESERVED FOR US AT 40H TO HOLD:
TRACK	EQU	SCRAT+0		;LAST SELECTED TRACK
SECTOR	EQU	SCRAT+1		;LAST SELECTED SECTOR
DMAAD	EQU	SCRAT+2		;LAST SELECTED DMA ADDRESS
DISKNO	EQU	SCRAT+4		;LAST SELECTED DISK NUMBER
;
;	JUMP VECTOR FOR INDIVIDUAL SUBROUTINES
;
	JMP	BOOT	;COLD START
WBOOTE:
	JMP	WBOOT	;WARM START

	JMP	CONST	;CONSOLE STATUS
	JMP	CONIN	;CONSOLE CHARACTER IN
	JMP	CONOUT	;CONSOLE CHARACTER OUT
	JMP	LIST	;LIST CHARACTER OUT
	JMP	PUNCH	;PUNCH CHARACTER OUT
	JMP	READER	;READER CHARACTER OUT

	JMP	HOME	;MOVE HEAD TO HOME POSITION
	JMP	SELDSK	;SELECT DISK
	JMP	SETTRK	;SET TRACK NUMBER
	JMP	SETSEC	;SET SECTOR NUMBER
	JMP	SETDMA	;SET DMA ADDRESS
	JMP	READ	;READ DISK
	JMP	WRITE	;WRITE DISK

;; added in cp/m 2.0:
;;	JMP	LISTST	;RETURN LIST STATUS
;;	JMP	SECTRAN	;SECTOR TRANSLATE

        PAGE

;;; INDIVIDUAL SUBROUTINES TO PERFORM EACH FUNCTION


;;; The BOOT entry point gets control from the cold start loader
;;; and is responsible for basic system initialization, includ-
;;; ing sending a sign-on message (which can be omitted in the
;;; first version). If the IOBYTE function is implemented, it
;;; must be set at this point. The various system parameters
;;; which are set by the WBOOT entry point must be initialized,
;;; and control is transferred to the CCP at 2900H+b for further
;;; processing. Note that reg C must be set to zero to select
;;; drive A.

BOOT:
        JMP     GOCPM           ;INITIALIZE AND GO TO CP/M

;;; The WBOOT entry point gets control when a warm start occurs.
;;; A warm start is performed whenever a user program branches to
;;; location 0000H, or when the CPU is reset from the front panel.
;;; The CP/M system must be loaded from the first two tracks of
;;; drive A up to, but not including, the BIOS (or CBIOS, if you
;;; have completed your patch). System parameters must be ini-
;;; tialized as shown below:
;;;
;;;          location 0,1,2    Set to JMP WBOOT for warm starts
;;;                            (0000H: JMP 3E03H+b)
;;;          location 3        Set initial value of IOBYTE, if
;;;                            implemented in your CBIOS.
;;;          location 5,6,7    Set to JMP BDOS, which is the
;;;                            primary entry point to CP/M for
;;;                            transient programs
;;;                            (0005H: JMP 3106H+b)
;;;
;;; Upon completion of the initialization, the WBOOT program
;;; must branch to the CCP at 2900H+b to (re)start the system.
;;; Upon entry to the CCP, register C is set to the drive to
;;; select after system initialization.

WBOOT:  ;SIMPLEST CASE IS TO READ THEDISK UNTIL ALL SECTORS LOADED
	LXI	SP, 80H		;USE SPACE BELOW BUFFER FOR STACK
	MVI	C, 0		;SELECT DISK 0
	CALL	SELDSK
	CALL	HOME		;GO TO TRACK 00
;
	MVI	B, NSECTS	;B COUNTS * OF SECTORS TO LOAD
	MVI	C, 0		;C HAS THE CURRENT TRACK NUMBER
	MVI	D, 2		;D HAS THE NEXT SECTOR TO READ
;	NOTE THAT WE BEGIN BY READING TRACK 0, SECTOR 2 SINCE SECTOR 1
;	CONTAINS THE COLD START LOADER, WHICH IS SKIPPED IN A WARM START
	LXI	H, CPMB		;BASE OF CP/M (INITIAL LOAD POINT)
LOAD1:	;LOAD	ONE MORE SECTOR
	PUSH	B		;SAVE SECTOR COUNT, CURRENT TRACK
	PUSH	D		;SAVE NEXT SECTOR TO READ
	PUSH	H		;SAVE DMA ADDRESS
	MOV	C, D		;GET SECTOR ADDRESS TO REGISTER C
	CALL	SETSEC		;SET SECTOR ADDRESS FROM REGISTER C
	POP	B		;RECALL DMA ADDRESS TO B, C
	PUSH	B		;REPLACE ON STACK FOR LATER RECALL
	CALL	SETDMA		;SET DMA ADDRESS FROM B, C
;
;	DRIVE SET TO 0, TRACK SET, SECTOR SET, DMA ADDRESS SET
	CALL	READ
	CPI	00H		;ANY ERRORS?
	JNZ	WBOOT		;RETRY THE ENTIRE BOOT IF AN ERROR OCCURS
;
;	NO ERROR, MOVE TO NEXT SECTOR
	POP	H		;RECALL DMA ADDRESS
	LXI	D, 128		;DMA=DMA+128
	DAD	D		;NEW DMA ADDRESS IS IN H, L
	POP	D		;RECALL SECTOR ADDRESS
	POP	B	        ;RECALL NUMBER OF SECTORS REMAINING, AND CURRENT TRK
	DCR	B		;SECTORS=SECTORS-1
	JZ	GOCPM		;TRANSFER TO CP/M IF ALL HAVE BEEN LOADED
;
;	MORE SECTORS REMAIN TO LOAD, CHECK FOR TRACK CHANGE
	INR	D
	MOV	A,D		;SECTOR=27?, IF SO, CHANGE TRACKS
	CPI	27
	JC	LOAD1		;CARRY GENERATED IF SECTOR<27
;
;	END OF CURRENT TRACK, GO TO NEXT TRACK
	MVI	D, 1		;BEGIN WITH FIRST SECTOR OF NEXT TRACK
	INR	C		;TRACK=TRACK+1
;
;	SAVE REGISTER STATE, AND CHANGE TRACKS
	PUSH	B
	PUSH	D
	PUSH	H
	CALL	SETTRK		;TRACK ADDRESS SET FROM REGISTER C
	POP	H
	POP	D
	POP	B
	JMP	LOAD1		;FOR ANOTHER SECTOR
;
;	END OF	LOAD OPERATION, SET PARAMETERS AND GO TO CP/M
GOCPM:
	MVI	A, 0C3H		;C3 IS A JMP INSTRUCTION
	STA	0		;FOR JMP TO WBOOT
	LXI	H, WBOOTE	;WBOOT ENTRY POINT
	SHLD	1		;SET ADDRESS FIELD FOR JMP AT 0
;
	STA	5		;FOR JMP TO BDOS
	LXI	H, BDOS		;BDOS ENTRY POINT
	SHLD	6		;ADDRESS FIELD OF JUMP AT 5 TO BDOS
;
	LXI	B, 80H		;DEFAULT DMA ADDRESS IS 80H
	CALL	SETDMA
;
	EI			;ENABLE THE INTERRUPT SYSTEM
	LDA	CDISK		;GET CURRENT DISK NUMBER
	MOV	C, A		;SEND TO THE CCP
	JMP	CPMB		;GO TO CP/M FOR FURTHER PROCESSING

;;; Sample the status of the currently assigned console device;
;;; return 0FFH in register A if a character is ready to read
;;; and 00H in register A if no console characters are ready.

CONST:
	HLT                     ;content tbd
        MVI     A,00H
        RET

;;; Read the next console character into register A, and set the
;;; high-order (parity bit). If no console character is ready,
;;; wait until a character is typed before returning.

CONIN:
        MOV     A,C
	HLT                     ;content tbd
        RET

;;; Send the character from register C to the console output de-
;;; veice. The character is in ASCII, with high-order (parity) bit
;;; set to zero. You may want to include a time-out on a line
;;; feed or carriage return, if your console device requires some
;;; time interval at the end of the line (such as a TI Silent 700
;;; terminal). You can, if you wish, filter out control char-
;;; acters which cause your console device to react in a strange
;;; way (a control-z causes the Lear Siegler terminal to clear
;;; the screen, for example).

CONOUT: HLT                     ;content tbd
        RET

;;; Send the character from register C to the currently assigned
;;; listing device. The character is in ASCII with zero parity.

LIST:
	HLT                     ;content tbd
        RET

;;; Send the character from register C to the currently assigned
;;; punch device. The character is in ASCII with zero parity.

PUNCH:
	HLT                     ;content tbd
        RET

;;; Read the next character from the currently assigned reader de-
;;; vice into register A with zero parity (high-order bit must be
;;; zero), an end of file condition is reported by return an
;;; ASCII control-z (1AH).

READER:
	HLT                     ;content tbd
        RET

        PAGE

;;; Return the disk head of the currently selected disk (initially
;;; disk A) to the track 00 position. If your controller allows
;;; access to the track 0 flag from the device, step the head until
;;; the track 0 flag is detected. If your controller does not
;;; support this feature, you can translate the HOME call into a
;;; call on SETTRK with a parameter of 0.

HOME:
        MVI     C,0
        CALL    SETTRK
        RET

;;; Select the disk drive given by register C for further opera-
;;; tions, where register C contains 0 for drive A, 1 for drive B,
;;; 2 for drive C, and 3 for drive D. (The standard CP/M
;;; distribution supports a maximum of four drives). If
;;; your system has less than 4 drives, you may wish to give an
;;; error message at the console, and terminate execution. It is
;;; advisable to postpone the actual disk operation until
;;; an I/O function (seek, read or write) is actually performed,
;;; since disk selects often occur without ultimately performing
;;; any disk I/O, and many controllers will unload the head of the
;;; current disk before selecting the new drive. This would
;;; cause an excessive amount of noise and disk wear.

SELDSK:
        MOV     A,C
        STA     DISKNO
        RET

;;; Register C contains the track number for subsequent disk
;;; accesses on the currently selected drive. You can choose to
;;; seek the selected track at this time, or delay the seek until
;;; the next read or write actually occurs. Register C can take
;;; on values in the range 0-76 corresponding to valid track
;;; numbers.

SETTRK:
        MOV     A,C
        STA     TRACK
        RET

;;; Register C contains the sector number (1 through 26) for sub-
;;; sequent disk accesses on the currently selected drive. You
;;; can choose to send this information to the controller at this
;;; point, or instead delay sector selectino until a read
;;; or write opration occurs.

SETSEC:
        MOV     A,C
        STA     SECTOR
        RET

;;; Registers B and C (high-order 8 bits in B, low-order 8 bits
;;; in C) contain the DMA (Direct Memory Access) address for sub-
;;; sequent read or write operations. For example, if B = 00H
;;; and C = 80H when SETDMA is called, then all subsequent read
;;; operations read their data into 80H through 0FFH, and all
;;; subsequent write operations get their data from 80H through
;;; 0FFH, until the next call to SETDMA occurs. The initial
;;; DMA address is assumed to be 80H. Note that the controller
;;; need not actually support direct memory access. If, for
;;; example, all data is received and sent through I/O ports, the
;;; CBIOS which you construct will use he 128-byte area starting
;;; at the selected DMA address for the memory buffer during the
;;; following read or write operations.

SETDMA:
        MOV     L,C
        MOV     H,B
        SHLD    DMAAD
        RET

;;; Assuming the drive has been selected, the track has been set,
;;; the sector has been set, and the DMA address has been speci-
;;; fied, the READ subroutine attempts to read one sector based
;;; upon these parameters, and returns the following error codes
;;; in register A:
;;;
;;;            0      no errors occurred
;;;            1      non-recoverable error conditino occurred
;;;
;;; Currently, CP/M responds only to a zero or non-zero value as
;;; the return code. That is, if the value in register A is 0
;;; then CP/M assumes that the disk operation completed properly.
;;; If an error occurs, however, the CBIOS should attempt at
;;; least 10 re-trie to see if the error is recoverable. When an
;;; error is reported the BDOS will print the message "BDOS ERR
;;; ON x: BAD SECTOR." The operator then has the option of
;;; typing <cr> to ignore the error, or control-C to abort.

READ:
	HLT                     ;content tbd
        RET

;;; Write the data from the currently selected DMA address to the
;;; currently selected drive, track, and sector. The data should
;;; be marked as "non deleted data" to maintain compatibility
;;; with other CP/M systems. The error codes given in the READ
;;; command are returned in register A, with error recovery at-
;;; tempts as described above.

WRITE:
	HLT                     ;content tbd
        RET

LEFT    EQU     (MSIZE*1024)-$
