        ;; 2023-09-30 librarian note:
        ;;   retrieved from http://www.gaby.de/cpm/manuals/archive/cpm22htm/axa.asm
        ;;   adjusted for assembly with "asm8080"
        ;;   - "IF" is less flexible.

	title   'mds cold start loader at 3000h'
;
;	mds-800 cold start loader for cp/m 2.0
;
;	version 2.0 august, 1979
;
testing	equ	0	    	;if true, then go to mon80 on  errors
;
	if	testing
bias	equ	03400h
	else
bias	equ	0000h
	endif
cpmb	equ	bias		;base of dos load
bdos	equ	806h+bias	;entry to dos for calls
bdose	equ	1880h+bias	;end of dos load
boot	equ	1600h+bias	;cold start entry point
rboot	equ	boot+3		;warm start entry point
;
	org	03000h		;loaded down from hardware boot at 3000H
;
bdosl	equ	bdose-cpmb
ntrks	equ	2		;number of tracks to read
bdoss	equ	bdosl/128	;number of sectors in dos
bdoso	equ	25		;number of bdos sectors on track 0
bdos1	equ	bdoss-bdoso	;number of sectors on track 1
;
mon80	equ	0f800h		;intel monitor base
rmon80	equ	0ff0fh		;restart location for mon80
base	equ	078h		;'base' used by controller
rtype	equ	base+1		;result type
rbyte	equ	base+3		;result byte
reset	equ	base+7		;reset controller
;
;
dstat	equ	base		;disk status port
ilow	equ	base+1		;low iopb address
ihigh	equ	base+2		;high iopb address
bsw	equ	0ffh		;boot switch
recal	equ	3h		;recalibrate selected drive
readf	equ	4h		;disk read function
stack	equ	100h		;use end of boot for stack
;
rstart:
	lxi	sp,stack;	;in case of call to mon80
;	clear disk status
	in	rtype
	in	rbyte
;	check if boot switch is off
coldstart:
	in 	bsw
	ani	02h		;switch on?
	jnz	coldstart
;	clear the controller
	out	reset		;logic cleared
;
;
	mvi	b,ntrks		;number of tracks to read
	lxi	h,iopbo
;
start:
;
;	read first/next track into cpmb
	mov	a,l
	out	ilow
	mov	a,h
	out	ihigh
waito:	in	dstat
	ani	4
	jz	waito
;
;	check	disk status
	in	rtype
	ani	11b
	cpi	2
;
	if	testing
	cnc	rmon80		;go to monitor if 11 or 10
        else
	jnc	rstart		;retry the load
	endif
;
	in	rbyte		;i/o complete, check status
;	if not	ready, then go to mon80
	ral
	cc	rmon80		;not ready bit set
	rar			;restore
	ani	11110b		;overrun/addr err/seek/crc/xxxx
;
	if	testing
	cnz	rmon80		;go to monitor
        else
	jnz	rstart		;retry the load
	endif
;
;
	lxi	d,iopbl		;length of iopb
	dad	d		;addressing next iopb
	dcr	b		;count down tracks
	jnz	start
;
;
;	jmp to boot to print initial message, and set up jmps
	jmp	boot
;
;	parameter blocks
iopbo:	db	80h		;iocw, no update
	db	readf		;read function
	db	bdoso		;*sectors to read on track 0
	db	0		;track 0
	db	2		;start with sector 2 on track 0
	dw	cpmb		;start at base of bdos
iopbl	equ	$-iopbo
;
iopb1:	db	80h
	db	readf
	db	bdos1		;sectors to read on track 1
	db	1		;track 1
	db	1		;sector 1
	dw	cpmb+bdoso*128	;base of second read
;
	end
