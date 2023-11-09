	title	'Boot ROM for the (void *)8080 simulated microcomputer'

	.cpu	8080

;;; === === === === === === === === === === === === === === === ===
;;; VoidStar8080 memory configuration
;;; === === === === === === === === === === === === === === === ===

ramkb	equ	62
ramtop	equ	ramkb*1024
romkb	equ	2
rombase equ	(64-romkb)*1024

;;; === === === === === === === === === === === === === === === ===
;;; RAM usage by ROM
;;; === === === === === === === === === === === === === === === ===

kbbuf	equ	ramtop-256	;dedicate a page to kbbuf
initsp	equ	kbbuf

;;; === === === === === === === === === === === === === === === ===
;;; CDEV Controller I/O Ports
;;; === === === === === === === === === === === === === === === ===

ttyd	equ	0		;read/write data from/to printing console
ttyc	equ	1		;printing console status/control port
crtd	equ	2		;read/write data from/to video console
crtc	equ	3		;video console status/control port
pptd	equ	4		;read/write data from/to paper tape
pptc	equ	5		;paper tape status/control port
lptd	equ	6		;write data to line printer
lptc	equ	7		;line printer status/control port

;;; === === === === === === === === === === === === === === === ===
;;; BDEV Controller I/O Ports
;;; === === === === === === === === === === === === === === === ===

bdres	equ	10		;reset controller (drv 0, trk 0, sec 1)
bddrv	equ	11		;select drive
bdtrk	equ	12		;select track
bdsec	equ	13		;select sector
bddat	equ	14		;read/write next byte

;;; === === === === === === === === === === === === === === === ===
;;; SYSTEM RESET HANDLING
;;; === === === === === === === === === === === === === === === ===

	;; BOOT ROM lives in the top 2 KiB of memory while the system
	;; is running normally, and is visible in all of memory while
	;; the SHADOW mechanism is active.
	;;
	;; The CPU arrives here during system reset due to SHADOW
	;; being active; also arrive here if code jumps to ROMBASE to
	;; initiate a cold start.
	;;
	;; Disable interrupts until we are properly initialized and
	;; jump to the COLD START routine. This will release the
	;; SHADOW facility if it is active.

	org	rombase

	di
	jmp	warm

;;; === === === === === === === === === === === === === === === ===
;;; ROM MONITOR ENTRY POINT
;;; === === === === === === === === === === === === === === === ===

	;; === === === === === === === === === === === ===
	;; Get us back to a known good state:
	;; - reset the BDEV controller (cancel transfer in progress)
	;; - point SP back at the memory for the ROM stack
	;; === === === === === === === === === === === ===

warm:
	nop
	lxi	sp,4000h       ;establish rom stack
	call	ttyilm
	db	10,13,'VoidStar8080 boot rom v0.0.1 build 3',10,13,10,0

	call	memsize

mon:	
	in	bdres		;reset BDEV controller
	ei			;enable the interrupt system

	call	ttyilm
	db	10,13,'(void *) ',0

	call	getline

	push	h
	call	ttyilm
	db	10,13,'getline done, calling dispatch',0
	pop	h

	mov	a,m
	ora	a
	cnz	dispatch

	call	ttyilm
	db	10,13,'dispatch done, looping back to mon',10,13,0

	jmp	mon

memsize:
	;; report memory size.
	lxi	h,0000h		;test address
memcheck:
	mov	a,m		;read original value
	mov	c,a		;save it for comparison
	cma
	mov	m,a		;write different value
	mov	a,c		;grab original value

	cmp	m		;compare original vs memory

	jz	romfound	;if unchanged, this is not ram.

	mov	m,a		;restore memory value

	inr	h
	jnz	memcheck

	call	ttyilm
	db	10,13,'RAM as far as the eye can see.',10,13,0
	ret

romfound:

	push	h
	push	d
	push	b

	mov	a,h

	;; divide A by ten,
	;; leaving quotient in b
	;; and remainder in c

	;; ALSO HAD A BUG IN PRINTING:
	;; RRC does not take a shift count.

	rrc
	rrc

	;; compute a/10 and a%10
	;; b := a/10
	;; a := a%10
	;; using c == 9
	lxi	b,10

loop:
	sub	c
	inr	b
	jnc	loop
out:
	add	c
	mov	c,a
	dcr	b

	;; divided A by ten,
	;; left quotient in b,
	;; remainder in c,
	;; and Z flag set if quotient is zero.

	jz	skiptens

	push	b
	mvi	a,'0'
	add	b
	mov	c,a
	call	ttyput
	pop	b

skiptens:
	mvi	a,'0'
	add	c
	mov	c,a
	call	ttyput
	
	pop	b
	pop	d
	pop	h

	call	ttyilm
	db	' KiB memory detected',10,13,0
	ret
	

	
	;; === === === === === === === === === === === ===
	;; getline: read a string from the keyboard.
	;; - CR and LF indicate output is complete
	;; - BS and DEL erase to the left;
	;;   if at the left margin they BEEP.
	;; - NAK (Control-U) cancels the line
	;; backspacing to the left of the first column.
	;; returns the address of the start of the line,
	;; and forces a zero at the end of the line.
	;; === === === === === === === === === === === ===

BEL	equ	07h
BS	equ	08h
TAB	equ	09h
LF	equ	0Ah
CR	equ	0Dh
NAK	equ	15h
DEL	equ	7Fh

getline:
	lxi	h,kbbuf

getline_clear:	
	mvi	m,0
	inr	l
	jnz	getline_clear
	inr	l

	call	getline_loop
	lxi	h,kbbuf+1
	mov	a,m
	ret

getline_loop:
	call	ttyget

	;; CR or LF ends the input and accepts the line.
	cpi	LF
	rz
	cpi	CR
	rz

	;; NAK (Control-U) cancels the line,
	;; and returns an empty line.
	cpi	NAK
	jz	getline_cancel
	
	cpi	BS
	jz	getline_backspace
	cpi	DEL
	jz	getline_backspace

	;; reject other control characters
	cpi	' '
	jc	getline_nope

	;; otherwise: add to buffer.
	mov	m,a
	mov	c,a
	call	ttyput
	inr	l
	jnz	getline_loop
	;; hey, we wrapped. cancel.
	dcr	l
	mvi	m,0
	;; and remove the echoed character from the display
	mvi	c,BS
	call	ttyput
	mvi	c,' '
	call	ttyput
	mvi	c,BS
	call	ttyput
	jmp	getline_nope
	
getline_backspace:
	dcr	l
	jz	getline_backspace_underflow
	mvi	c,BS
	call	ttyput
	mvi	c,' '
	call	ttyput
	mvi	c,BS
	call	ttyput
	mvi	m,0
	jmp	getline_loop

getline_backspace_underflow:
	mvi	l,1
getline_nope:
	mvi	c,BEL
	call	ttyput
	jmp	getline_loop

getline_cancel:
	dcr	l
	rz
	mvi	c,BS
	call	ttyput
	mvi	c,' '
	call	ttyput
	mvi	c,BS
	call	ttyput
	mvi	m,0
	jmp	getline_cancel

	;; === === === === === === === === === === === ===
	;; dispatch: route control to command
	;; (HL) points to the command line,
	;; (A) already has the first charcter from the line.
	;; === === === === === === === === === === === ===
dispatch:

	;; when we get here, the cursor is still where it was
	;; when the user pressed CR, LF, or NAK. Bop down to
	;; the next display line before doing anything.
	push	h
	call	ttyilm
	db	10,13,'dispatch... command is: ',0
	pop	h

	mov	c,m		;recover 1st char of command line

	push	h		;keep command line ptr safe on stack
	call	ttyput
	call	ttyilm
	db	10,13,0
	pop	h

	mov	a,m		;recover 1st char of command line

	push	h		;keep command line ptr safe on stack
	lxi	h, command_table
	mvi	e, command_table_len
tscan:
	cmp	m
	jz	foundit
	inx	h
	inx	h
	inx	h
	dcr	e
	jnz	tscan
notfound:	
	call	ttyilm
	db	"Command not recognized.",10,13,0
	pop	h		;drop command string ptr from stack
	ret

foundit:
	inx	h
	mov	e,m
	inx	h
	mov	d,m
	xchg
	xthl		    ;dest -> stack, stack -> hl
	ret

command_table:
	db	'A'
	dw	do_boot
	db	'B'
	dw	do_boot
	db	'C'
	dw	do_boot
	db	'D'
	dw	do_boot
	db	'H'
	dw	do_halt
command_table_len	equ	($-command_table)/3

	;; === === === === === === === === === === === ===
	;; do_boot: boot from the appropriate drive
	;; === === === === === === === === === === === ===
 
do_boot:
	;; HL points at the command line.
	;; determine the boot drive from the command letter.
	mov	a,m
	sui	'A'
	out	bdres
	out	bddrv
	lxi	h,0080h		;0080h to 00ffh receives boot sector
	sphl			;stack is below 0080h
	push	h		;set up to RET to the loaded data
do_boot_byte:
	in	bddat
	mov	m,a
	inr	l
	jnz	do_boot_byte
	ret

	;; === === === === === === === === === === === ===
        ;; do_halt: halt the CPU.
        ;; assures interrupts are enabled.
        ;; warm start if execution resumes.
	;; === === === === === === === === === === === ===
do_halt:
	call	ttyilm
	db	"Shutting down ...", 10, 13, 0
        ei
	hlt
	jmp	warm

	;; === === === === === === === === === === === ===
	;; ttyilm: put the string at (PC) to the TTY.
	;; the string is terminated by an ASCII NUL.
	;; return to the byte following the NUL.
	;; === === === === === === === === === === === ===
ttyilm:
	pop	h
	call	ttystr
	pchl
	
	;; === === === === === === === === === === === ===
	;; ttystr: put the string at (HL) to the TTY.
	;; the string is terminated by an ASCII NUL.
	;; on return, (HL) points to the byte following the NUL.
	;; modifies A, C
	;; === === === === === === === === === === === ===
ttystr:
	mov	a,m
	inx	h
	ora	a
	rz
	mov	c,a
	call	ttyput
	jmp	ttystr
	
	;; === === === === === === === === === === === ===
	;; ttyout: put the character in (C) to the TTY.
	;; === === === === === === === === === === === ===
ttyput:
	in	ttyc
	ani	04h
	jz	ttyput
	mov	a,c
	out	ttyd
	ret

	;; === === === === === === === === === === === ===
	;; ttyget: get a key from the TTY into A.
	;; wait until one is available.
	;; === === === === === === === === === === === ===
ttyget:
	in	ttyc
	ani	02h
	jz	ttyget
	in	ttyd
	ani	07Fh
	ret

