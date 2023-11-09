	TITLE	'Customized CP/M 2.2 Bios for the (void *)8080 Simulated Microcomputer'

	.cpu	8080

msize	equ	62		;cp/m ram size in kilobytes

;	"bias" is address offset from 3400h for memory systems larger
;	than 20k (referred to as "b" throughout the text)
;
bias	equ	(msize-20)*1024
ccp	equ	3400h+bias	;base of ccp
bdos	equ	ccp+806h	;base of bdos
bios	equ	ccp+1600h	;base of bios
bioslim	equ	ccp+(51*128)	;end of data loaded from disk

; page zero locations known by BIOS
iobyte	equ	0003h		;intel i/o byte
cdisk	equ	0004h		;current disk number 0=a,... l5=p
;
	org	bios		;origin of this program
nsects	equ	($-ccp)/128	;warm start sector count
;
;	jump vector for individual subroutines
;
	jmp	boot	;cold start
wboote:	jmp	wboot	;warm start

	jmp	const	;console status
	jmp	conin	;console character in
	jmp	conout	;console character out
	jmp	list	;list character out
	jmp	punch	;punch character out
	jmp	reader	;reader character out

	jmp	home	;move head to home position
	jmp	seldsk	;select disk
	jmp	settrk	;set track number
	jmp	setsec	;set sector number
	jmp	setdma	;set dma address
	jmp	read	;read disk
	jmp	write	;write disk

	jmp	listst	;return list status
	jmp	sectran	;sector translate

        NEWPAGE

;;; === === === === === === === === === === === === === === === ===
;;; COLD AND WARM BOOT
;;; === === === === === === === === === === === === === === === ===

boot:	;simplest case is to just perform parameter initialization
	xra	a		;zero in the accum
	sta	iobyte		;clear the iobyte
	sta	cdisk		;select disk zero
	jmp	gocpm		;initialize and go to cp/m
;
wboot:	;simplest case is to read the disk until all sectors loaded
	lxi	sp, 80h		;use space below buffer for stack
	mvi	c, 0		;select disk 0
	call	seldsk
	call	home		;go to track 00
;
	mvi	b, nsects	;b counts * of sectors to load
	mvi	c, 0		;c has the current track number
	mvi	d, 2		;d has the next sector to read
;	note that we begin by reading track 0, sector 2 since sector 1
;	contains the cold start loader, which is skipped in a warm start
	lxi	h, ccp		;base of cp/m (initial load point)
load1:	;load	one more sector
	push	b		;save sector count, current track
	push	d		;save next sector to read
	push	h		;save dma address
	mov	c, d		;get sector address to register C
	call	setsec		;set sector address from register C
	pop	b		;recall dma address to b, C
	push	b		;replace on stack for later recall
	call	setdma		;set dma address from b, C
;
;	drive set to 0, track set, sector set, dma address set
	call	read
	cpi	00h		;any errors?
	jnz	wboot		;retry the entire boot if an error occurs
;
;	no error, move to next sector
	pop	h		;recall dma address
	lxi	d, 128		;dma=dma+128
	dad	d		;new dma address is in h, l
	pop	d		;recall sector address
	pop	b	;recall number of sectors remaining, and current trk
	dcr	b		;sectors=sectors-1
	jz	gocpm		;transfer to cp/m if all have been loaded
;
;	more	sectors remain to load, check for track change
	inr	d
	mov	a,d		;sector=27?, if so, change tracks
	cpi	27
	jc	load1		;carry generated if sector<27
;
;	end of	current track,	go to next track
	mvi	d, 1		;begin with first sector of next track
	inr	c		;track=track+1
;
;	save	register state, and change tracks
	push	b
	push	d
	push	h
	call	settrk		;track address set from register c
	pop	h
	pop	d
	pop	b
	jmp	load1		;for another sector
;
;	end of	load operation, set parameters and go to cp/m
gocpm:
	mvi	a, 0c3h		;c3 is a jmp instruction
	sta	0		;for jmp to wboot
	lxi	h, wboote	;wboot entry point
	shld	1		;set address field for jmp at 0
;
	sta	5		;for jmp to bdos
	lxi	h, bdos		;bdos entry point
	shld	6		;address field of Jump at 5 to bdos
;
	lxi	b, 80h		;default dma address is 80h
	call	setdma
;
	ei			;enable the interrupt system
	lda	cdisk		;get current disk number
	mov	c, a		;send to the ccp
	jmp	ccp		;go to cp/m for further processing

        NEWPAGE

;;; === === === === === === === === === === === === === === === ===
;;; CDEV selection using IOBYTE
;;; === === === === === === === === === === === === === === === ===

	;; === === === === === === === === === === === ===
	;; return ff if a byte is available on con: or 00 if not.
	;; use i/o byte to determine which device is the console.
	;; switch i/o byte to "pure TTY" if an invalid device is selected
	;; === === === === === === === === === === === ===
const:
	lda	iobyte
	ani	03h
	jz	ttyrdy
	cpi	1
	jz	crtrdy
	cpi	2
	jz	pptrdy
	;; no support for user defined consoles (yet?)
	;; switch iobyte back to CON=TTY and use TTY.
	lda	iobyte
	ani	0FCh
	sta	iobyte
	jmp	ttyrdy

	;; === === === === === === === === === === === ===
	;; get next character from con: into a and clear its parity bit.
	;; use i/o byte to determine which device is the console.
	;; if there is no data available, wait for some.
	;; === === === === === === === === === === === ===
conin:
	lda	iobyte
	ani	03h
	jz	ttyget
	cpi	1
	jz	crtget
	cpi	2
	jz	pptget
	;; no support for user defined consoles (yet?)
	;; switch iobyte back to pure TTY and use TTY
	xra	a
	sta	iobyte
	jmp	ttyget

	;; === === === === === === === === === === === ===
	;; put the character in (C) to CON:
	;; use i/o byte to determine which device is the console.
	;; switch i/o byte to "pure TTY" if an invalid device is selected
	;; if the device can not accept data, wait until it can.
	;; modifies the content of register a.
	;; === === === === === === === === === === === ===
conout:
	lda	iobyte
	ani	03h
	jz	ttyput
	cpi	1
	jz	crtput
	cpi	2
	jz	pptput
	;; no support for user defined consoles (yet?)
	;; switch iobyte back to pure TTY and use tty
	xra	a
	sta	iobyte
	jmp	ttyput

	;; === === === === === === === === === === === ===
	;; put the the character in c to lst: 
	;; use i/o byte to determine which device is the listing.
	;; switch i/o byte to "pure TTY" if an invalid device is selected
	;; if the device can not accept data, wait until it can.
	;; modifies the content of register a.
	;; === === === === === === === === === === === ===

list:
	lda	iobyte
	ani	0C0h
	jz	ttyput
	cpi	040h
	jz	crtput
	cpi	080h
	jz	lptput
	;; no support for user defined consoles (yet?)
	;; switch iobyte back to pure TTY and use tty
	xra	a
	sta	iobyte
	jmp	ttyput

	;; === === === === === === === === === === === ===
	;; return ff if lst: can accept a byte or 00 if not.
	;; use i/o byte to determine which device is the listing.
	;; switch i/o byte to "pure TTY" if an invalid device is selected
	;; === === === === === === === === === === === ===

listst:
	lda	iobyte
	ani	0C0h
	jz	ttypok
	cpi	040h
	jz	crtpok
	cpi	080h
	jz	lptpok
	;; no support for user defined listing devices (yet?)
	;; switch iobyte back to pure TTY and use tty
	xra	a
	sta	iobyte
	jmp	ttypok

	;; === === === === === === === === === === === ===
	;; put the the character in c to pun: 
	;; use i/o byte to determine which device is the listing.
	;; switch i/o byte to "pure TTY" if an invalid device is selected
	;; if the device can not accept data, wait until it can.
	;; modifies the content of register a.
	;; === === === === === === === === === === === ===

punch:
	lda	iobyte
	ani	030h
	jz	ttyput
	cpi	010h
	jz	pptput
	;; no support for user defined punches (yet?)
	;; switch iobyte back to pure TTY and use TTY
	xra	a
	sta	iobyte
	jmp	ttyput

	;; === === === === === === === === === === === ===
	;; get next character from rdr: into a and clear its parity bit.
	;; use i/o byte to determine which device is the listing.
	;; switch i/o byte to "pure TTY" if an invalid device is selected
	;; if there is no data available, wait for some.
	;; no i/o byte support yet
	;; === === === === === === === === === === === ===

reader:
	lda	iobyte
	ani	00Ch
	jz	ttyget
	cpi	004h
	jz	pptget
	;; no support for user defined readers (yet?)
	;; switch iobyte back to pure TTY and use TTY
	xra	a
	sta	iobyte
	jmp	ttyget

        NEWPAGE

;;; === === === === === === === === === === === === === === === ===
;;; DIRECT INTERACTION WITH CDEV DEVICES
;;; === === === === === === === === === === === === === === === ===

ttyd	equ	0		;read/write data from/to printing console
ttyc	equ	1		;printing console status/control port

cdev_cn	equ     01h		;cdev status: connected
cdev_rx	equ     02h		;cdev status: receive data ready
cdev_tx	equ     04h		;cdev status: ready to transmit

	;; === === === === === === === === === === === ===
	;; return ff if a byte is available on tty: or 00 if not.
	;; === === === === === === === === === === === ===
ttyrdy:
	in	ttyc
	ani	cdev_rx
	rz
	mvi	a,0ffh
	ret

	;; === === === === === === === === === === === ===
	;; return ff if tty: ready to accept data, or 00 if not.
	;; === === === === === === === === === === === ===
ttypok:
	in	ttyc
	ani	cdev_tx
	rz
	mvi	a,0ffh
	ret

	;; === === === === === === === === === === === ===
	;; get next character from tty: into a and clear its parity bit.
	;; if there is no data available, wait for some.
	;; === === === === === === === === === === === ===
ttyget:
	in	ttyc
	ani	cdev_rx
	jz	ttyget
	in	ttyd
	ani	7fh
	ret

	;; === === === === === === === === === === === ===
	;; put the the character in c to tty: 
	;; if the device can not accept data, wait until it can.
	;; modifies the content of register a.
	;; === === === === === === === === === === === ===
ttyput:
	in	ttyc
	ani	cdev_tx
	jz	ttyput
	mov	a,c
	out	ttyd
	ret

crtd	equ	2		;read/write data from/to video console
crtc	equ	3		;video console status/control port

	;; === === === === === === === === === === === ===
	;; return ff if a byte is available on crt: or 00 if not.
	;; === === === === === === === === === === === ===
crtrdy:
	in	crtc
	ani     cdev_rx
	rz
	mvi	a,0ffh
	ret

	;; === === === === === === === === === === === ===
	;; return ff if crt: ready to accept data, or 00 if not.
	;; === === === === === === === === === === === ===
crtpok:
	in	crtc
	ani	cdev_tx
	rz
	mvi	a,0ffh
	ret

	;; === === === === === === === === === === === ===
	;; get next character from crt: into a and clear its parity bit.
	;; if there is no data available, wait for some.
	;; === === === === === === === === === === === ===
crtget:
	in	crtc
	ani	cdev_rx
	jz	crtget
	in	crtd
	ani	7fh
	ret

	;; === === === === === === === === === === === ===
	;; put the the character in c to crt: 
	;; if the device can not accept data, wait until it can.
	;; modifies the content of register a.
	;; === === === === === === === === === === === ===
crtput:
	in	crtc
	ani	cdev_tx
	jz	crtput
	mov	a,c
	out	crtd
	ret

pptd	equ	4		;read/write data from/to paper tape
pptc	equ	5		;paper tape status/control port

	;; === === === === === === === === === === === ===
	;; return ff if a byte is available on ppt: or 00 if not.
	;; === === === === === === === === === === === ===
pptrdy:
	in	pptc
	ani	cdev_rx
	rz
	mvi	a,0ffh
	ret

	;; === === === === === === === === === === === ===
	;; return ff if ppt: ready to accept data, or 00 if not.
	;; === === === === === === === === === === === ===
pptpok:
	in	pptc
	ani	cdev_tx
	rz
	mvi	a,0ffh
	ret

	;; === === === === === === === === === === === ===
	;; get next character from ppt: into a and clear its parity bit.
	;; if there is no data available, wait for some.
	;; === === === === === === === === === === === ===
pptget:
	in	pptc
	ani	cdev_rx
	jz	pptget
	in	pptd
	ani	7fh
	ret

	;; === === === === === === === === === === === ===
	;; put the the character in c to ppt: 
	;; if the device can not accept data, wait until it can.
	;; modifies the content of register a.
	;; === === === === === === === === === === === ===
pptput:
	in	pptc
	ani	cdev_tx
	jz	pptput
	mov	a,c
	out	pptd
	ret

lptd	equ	6		;write data to line printer
lptc	equ	7		;line printer status/control port

	;; === === === === === === === === === === === ===
	;; return ff if lpt: ready to accept data, or 00 if not.
	;; === === === === === === === === === === === ===
lptpok:
	in	lptc
	ani	cdev_tx
	rz
	mvi	a,0ffh
	ret

	;; === === === === === === === === === === === ===
	;; put the the character in c to lpt: 
	;; if the device can not accept data, wait until it can.
	;; modifies the content of register a.
	;; === === === === === === === === === === === ===
lptput:
	in	lptc
	ani	cdev_tx
	jz	lptput
	mov	a,c
	out	lptd
	ret

        NEWPAGE

;;; === === === === === === === === === === === === === === === ===
;;; DIRECT INTERACTION WITH BDEV DEVICES
;;; === === === === === === === === === === === === === === === ===

	;; === === === === === === === === === === === ===
	;; disk controller i/o ports

bdres	equ	10		;reset controller (drv 0, trk 0, sec 1)
bddrv	equ	11		;select drive
bdtrk	equ	12		;select track
bdsec	equ	13		;select sector
bddat	equ	14		;read/write next byte

	;; === === === === === === === === === === === ===
	;; return the disk head of the currently selected disk
	;; (initially disk a) to the track 00 position. if your
	;; controller allows access to the track 0 flag from the
	;; device, step the head until the track 0 flag is detected.
	;; if your controller does not  support this feature, you can
	;; translate the home call into a call on settrk with a
	;; parameter of 0.
	;; === === === === === === === === === === === ===

home:
	mvi	c,0
	call	settrk
	ret


	;; === === === === === === === === === === === ===
	;; select the disk drive given by register c for further
	;; opera- tions, where register c contains 0 for drive a, 1
	;; for drive b, 2 for drive c, and 3 for drive d. (the
	;; standard cp/m distribution supports a maximum of four
	;; drives). if your system has less than 4 drives, you may
	;; wish to give an error message at the console, and terminate
	;; execution. it is advisable to postpone the actual disk
	;; operation until an i/o function (seek, read or write) is
	;; actually performed, since disk selects often occur without
	;; ultimately performing any disk i/o, and many controllers
	;; will unload the head of the current disk before selecting
	;; the new drive. this would cause an excessive amount of
	;; noise and disk wear.
        ;;
	;; voidstar8080: this call resets the controller, in case a
	;; previous read or write operation did not complete properly.
	;; This is a "soft" reset; the disk array (real or simulated)
	;; state is not modified until we actually initiate a read or
	;; write.
        ;;
	;; CP/M 2.2: locate and return the DPB for this drive in HL,
	;; or set HL to 0000h if the drive is not within our DPB table.
	;; HOWEVER NOTE: large disk numbers still get sent to BDEV controller.
	;; === === === === === === === === === === === ===

seldsk:
	lxi	h, 0000h	;error return code
	mov	a, c
	out	bdres		;reset bdev controller
	out	bddrv		;select bdev drive number

	cpi	ndisks		;check for disk number in table range
	rnc			;return with HL=0000 if off end of table
;	disk number is in the proper range

;	compute proper disk Parameter header address
	mov	l, a		;l=disk number 0, 1, 2, 3
	mvi	h, 0		;high order zero
	dad	h		;*2
	dad	h		;*4
	dad	h		;*8
	dad	h		;*16 (size of each header)
	lxi	d, dpbase
	dad	d		;hl=.dpbase (diskno*16)
	ret

	;; === === === === === === === === === === === ===
        ;;
	;; register c contains the track number for subsequent disk
	;; accesses on the currently selected drive. you can choose to
	;; seek the selected track at this time, or delay the seek
	;; until the next read or write actually occurs. register c
	;; can take on values in the range 0-76 corresponding to valid
	;; track numbers.
        ;;
	;; voidstar8080: pass the track number along to the controller.
	;; === === === === === === === === === === === ===

settrk:
	mov	a, c
	out	bdtrk
	ret

	;; === === === === === === === === === === === ===
	;; register c contains the sector number (1 through 26) for
	;; subsequent disk accesses on the currently selected drive.
	;; you can choose to send this information to the controller
	;; at this point, or instead delay sector selection until a
	;; read or write opration occurs.
	;; === === === === === === === === === === === ===

setsec:
	mov	a,c
	out	bdsec
	ret

	;; === === === === === === === === === === === ===
	;; translate the sector given by bc using the
        ;; translate table given by de
	;; === === === === === === === === === === === ===

sectran:
	xchg			;hl=.trans
	dad	b		;hl=.trans (sector)
	mov	l, m		;l=trans (sector)
	mvi	h, 0		;hl=trans (sector)
	ret			;with value in hl

	;; === === === === === === === === === === === ===
	;; registers b and c (high-order 8 bits in b, low-order 8 bits
	;; in c) contain the dma (direct memory access) address for sub-
	;; sequent read or write operations. for example, if b = 00h
	;; and c = 80h when setdma is called, then all subsequent read
	;; operations read their data into 80h through 0ffh, and all
	;; subsequent write operations get their data from 80h through
	;; 0ffh, until the next call to setdma occurs. the initial
	;; dma address is assumed to be 80h. note that the controller
	;; need not actually support direct memory access. if, for
	;; example, all data is received and sent through i/o ports, the
	;; cbios which you construct will use he 128-byte area starting
	;; at the selected dma address for the memory buffer during the
	;; following read or write operations.
	;; === === === === === === === === === === === ===

setdma:
	mov	l, c		;low order address
	mov	h, b		;high order address
	shld	dmaad		;save the address
	ret

	;; === === === === === === === === === === === ===
	;; assuming the drive has been selected, the track has been set,
	;; the sector has been set, and the dma address has been speci-
	;; fied, the read subroutine attempts to read one sector based
	;; upon these parameters, and returns the following error codes
	;; in register a:
	;;
	;;	       0      no errors occurred
	;;	       1      non-recoverable error condition occurred
	;;
	;; currently, cp/m responds only to a zero or non-zero value as
	;; the return code. that is, if the value in register a is 0
	;; then cp/m assumes that the disk operation completed properly.
	;; if an error occurs, however, the cbios should attempt at
	;; least 10 re-tries to see if the error is recoverable. when an
	;; error is reported the bdos will print the message "bdos err
	;; on x: bad sector." the operator then has the option of
	;; typing <cr> to ignore the error, or control-c to abort.
	;; === === === === === === === === === === === ===
read:	
	lhld	dmaad
	mvi	c,128
readl:
	in	bddat
	mov	m,a
	inx	h
	dcr	c
	jnz	readl
	xra	a
	ret

	;; === === === === === === === === === === === ===
	;; write the data from the currently selected dma address to the
	;; currently selected drive, track, and sector. the data should
	;; be marked as "non deleted data" to maintain compatibility
	;; with other cp/m systems. the error codes given in the read
	;; command are returned in register a, with error recovery at-
	;; tempts as described above.
	;; === === === === === === === === === === === ===
write:	
	lhld	dmaad
	mvi	c,128
writel:
	mov	a,m
	out	bddat
	inx	h
	dcr	c
	jnz	writel
	xra	a
	ret

endtext	equ     $

        NEWPAGE

;;; === === === === === === === === === === === === === === === ===
;;; FIXED TABLES
;;; === === === === === === === === === === === === === === === ===

	;;; === === === === === === === === === === === ===
	;;; Disk Parameter Table for 4 Drives
	;;; === === === === === === === === === === === ===


ndisks	set	4		;for later reference
dpbase	equ	$		;base of disk parameter blocks

dpe0:	dw	xlt0,0000h	;translate table
	dw	0000h,0000h	;scratch area
	dw	dirbuf,dpb0	;dir buff,parm block
	dw	csv0,alv0	;check, alloc vectors

dpe1:	dw	xlt1,0000h	;translate table
	dw	0000h,0000h	;scratch area
	dw	dirbuf,dpb1	;dir buff,parm block
	dw	csv1,alv1	;check, alloc vectors

dpe2:	dw	xlt2,0000h	;translate table
	dw	0000h,0000h	;scratch area
	dw	dirbuf,dpb2	;dir buff,parm block
	dw	csv2,alv2	;check, alloc vectors

dpe3:	dw	xlt3,0000h	;translate table
	dw	0000h,0000h	;scratch area
	dw	dirbuf,dpb3	;dir buff,parm block
	dw	csv3,alv3	;check, alloc vectors

	;;; === === === === === === === === === === === ===
	;;; 	Drive A: IBM Single Density 8-inch diskette
	;;; 	DISKDEF	[1, 26, 6, 1024, 243, 64, 64, 2]
	;;; === === === === === === === === === === === ===

als0	set	31		;size of allocation vector
css0	set	16		;number of checksum elements

dpb0	equ	$		;disk parm block
	dw	26		;sec per track
	db	3		;block shift
	db	7		;block mask
	db	0		;extnt mask
	dw	242		;disk size-1
	dw	63		;directory max
	db	192		;alloc0
	db	0		;alloc1
	dw	16		;check size
	dw	2		;offset

xlt0	equ	$		;skew factor 6
	db	1,7,13,19,25,5,11,17,23,3,9,15,21
	db	2,8,14,20,26,6,12,18,24,4,10,16,22

	;; === === === === === === === === === === === ===
	;; 	Drive B is the same as Drive A

dpb1	equ	dpb0	;equivalent parameters
als1	equ	als0	;same allocation vector size
css1	equ	css0	;same checksum vector size
xlt1	equ	xlt0	;same translate table

	;;; === === === === === === === === === === === ===
	;;; 	Drive C: Hypothetical Maximum Size Hard Drive
	;;; 	DISKDEF	[1, 255, 0, 8192, 1016, 1024, 0, 1]
	;;; === === === === === === === === === === === ===

als2	set	127		;size of allocation vector
css2	set	0		;number of checksum elements

dpb2	equ	$		;disk parm block
	dw	255		;sec per track
	db	6		;block shift
	db	63		;block mask
	db	3		;extnt mask
	dw	1015		;disk size-1
	dw	1023		;directory max
	db	240		;alloc0
	db	0		;alloc1
	dw	0		;check size
	dw	1		;offset

xlt2	equ	0		;no xlate table

	;; === === === === === === === === === === === ===
	;; 	Drive D is the same as Drive C

dpb3	equ	dpb2	;equivalent parameters
als3	equ	als2	;same allocation vector size
css3	equ	css2	;same checksum vector size
xlt3	equ	xlt2	;same translate table

;;; === === === === === === === === === === === === === === === ===
;;; END OF FIXED TABLES
;;; === === === === === === === === === === === === === === === ===

enddata	equ	$

;;; === === === === === === === === === === === === === === === ===
;;; MAXIMUM EXTENT OF DISK IMAGE
;;; === === === === === === === === === === === === === === === ===

endbios	equ	$

	NEWPAGE

;;; === === === === === === === === === === === === === === === ===
;;; the remainder of the cbios is reserved uninitialized
;;; data area, and does not need to be a part of the
;;; system memory image (the space must be available,
;;; however, between the BIOS and the end of memory).
;;; === === === === === === === === === === === === === === === ===

	org	bioslim
bss:			;beginning of data area

dmaad:	ds	2	;direct memory address

	;;; === === === === === === === === === === === ===
	;;; RAM backing for the Disk Parameter Table for 4 Drives
	;;; === === === === === === === === === === === ===

begdat	equ	$
dirbuf:	ds	128	;directory access buffer
alv0:	ds	als0
csv0:	ds	css0
alv1:	ds	als1
csv1:	ds	css1
alv2:	ds	als2
csv2	equ	0		;css2 is 'no space'
alv3:	ds	als3
csv3	equ	0		;css3 is 'no space'
enddat	equ	$
datsiz	equ	$-begdat

endbss:			;end of data area
	
bioslen	equ	endbios-bios	    
biosavl	equ	bioslim-endbios	;can add this much to the BIOS segment

bsslen	equ	endbss-bss;	;size of bssa area
bssavl	equ	0280H-bsslen	;can add this much to the BSS segment

	end
