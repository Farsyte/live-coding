	TITLE	'Customized CP/M 2.2 Bios for the (void *)8080 Simulated Microcomputer'

msize	equ	48		;cp/m version memory size in kilobytes

;	"bias" is address offset from 3400h for memory systems larger
;	than 20k (referred to as "b" throughout the text)
;
bias	equ	(msize-20)*1024
ccp	equ	3400h+bias	;base of ccp
bdos	equ	ccp+806h	;base of bdos
bios	equ	ccp+1600h	;base of bios
bioslim	equ     ccp+1900H       ;end of data loaded from disk
uimem	equ	(msize*1024)-bioslim

; page zero locations
cdisk	equ	0004h		;current disk number 0=a,... l5=p
iobyte	equ	0003h		;intel i/o byte
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
;
;	fixed data tables for four-drive standard
;	ibm-compatible 8-inch disks
;
;	disk Parameter header for disk 00
dpbase:	dw	trans, 0000h
	dw	0000h, 0000h
	dw	dirbf, dpblk
	dw	chk00, all00
;	disk parameter header for disk 01
	dw	trans, 0000h
	dw	0000h, 0000h
	dw	dirbf, dpblk
	dw	chk01, all01
;	disk parameter header for disk 02
	dw	trans, 0000h
	dw	0000h, 0000h
	dw	dirbf, dpblk
	dw	chk02, all02
;	disk parameter header for disk 03
	dw	trans, 0000h
	dw	0000h, 0000h
	dw	dirbf, dpblk
	dw	chk03, all03
;
;	sector translate vector
trans:	db	 1,  7, 13, 19	;sectors  1,  2,  3,  4
	db	25,  5, 11, 17	;sectors  5,  6,  7,  6
	db	23,  3,  9, 15	;sectors  9, 10, 11, 12
	db	21,  2,  8, 14	;sectors 13, 14, 15, 16
	db	20, 26,  6, 12	;sectors 17, 18, 19, 20
	db	18, 24,  4, 10	;sectors 21, 22, 23, 24
	db	16, 22		;sectors 25, 26
;
dpblk:	;disk parameter block, common to all disks
	dw	26		;sectors per track
	db	3		;block shift factor
	db	7		;block mask
	db	0		;null mask
	dw	242		;disk size-1
	dw	63		;directory max
	db	192		;alloc 0
	db	0		;alloc 1
	dw	16		;check size
	dw	2		;track offset
;
;	end of fixed tables
;
;	individual subroutines to perform each function
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

;;; === === === === === === === === === === === === === === === ===
;;;	simple i/o handlers (must be filled in by user)
;;;	in each case, the entry point is provided, with space reserved
;;;	to insert your own code
;;; === === === === === === === === === === === === === === === ===

;;; === === === === === === === === === === === === === === === ===
;;; serial device i/o ports
;;; === === === === === === === === === === === === === === === ===

ttyd	equ     0               ;read/write data from/to printing console
ttyc	equ     1               ;printing console status/control port

crtd	equ     2               ;read/write data from/to video console
crtc	equ     3               ;video console status/control port

lptd	equ     4               ;write data to line printer
lptc	equ     5               ;line printer status/control port

pptd	equ     6               ;read/write data from/to paper tape
pptc	equ     7               ;paper tape status/control port

;;; === === === === === === === === === === === === === === === ===
;;; Return FF if a byte is available on CON: or 00 if not.
;;; No I/O byte support yet
;;; === === === === === === === === === === === === === === === ===

const:
        in      ttyc
        ani     02h
        rz
        mvi     a,0ffh
        ret

;;; === === === === === === === === === === === === === === === ===
;;; get next character from con: into a and clear its parity bit.
;;; if there is no data available, wait for some.
;;; no i/o byte support yet
;;; === === === === === === === === === === === === === === === ===

conin:
        in      ttyc
        ani     01h
        jz      conin
        in      ttyd
        ani     7fh
        ret


;;; === === === === === === === === === === === === === === === ===
;;; put the the character in c to con: 
;;; if the device can not accept data, wait until it can.
;;; modifies the content of register a.
;;; no i/o byte support yet
;;; === === === === === === === === === === === === === === === ===

conout:
        in      ttyc
        ani     01h
        jz      conout
        mov     a,c
        out     ttyd
        ret

;;; === === === === === === === === === === === === === === === ===
;;; put the the character in c to lst: 
;;; if the device can not accept data, wait until it can.
;;; modifies the content of register a.
;;; no i/o byte support yet
;;; === === === === === === === === === === === === === === === ===

list:
        in      lptc
        ani     01h
        jz      list
        mov     a,c
        out     lptd
        ret

;;; === === === === === === === === === === === === === === === ===
;;; return ff if lst: can accept a byte or 00 if not.
;;; no i/o byte support yet
;;; === === === === === === === === === === === === === === === ===

listst:
        in      lptc
        ani     04h
        rz
        mvi     a,0ffh
        ret

;;; === === === === === === === === === === === === === === === ===
;;; put the the character in c to pun: 
;;; if the device can not accept data, wait until it can.
;;; modifies the content of register a.
;;; no i/o byte support yet
;;; === === === === === === === === === === === === === === === ===

punch:
        in      pptc
        ani     01h
        jz      punch
        mov     a,c
        out     pptd
        ret

;;; === === === === === === === === === === === === === === === ===
;;; get next character from rdr: into a and clear its parity bit.
;;; if there is no data available, wait for some.
;;; no i/o byte support yet
;;; === === === === === === === === === === === === === === === ===

reader:
        in      pptc
        ani     01h
        jz      reader
        in      pptd
	ani	7fh		;remember to strip parity bit
	ret

;;; === === === === === === === === === === === === === === === ===
;;;	i/o drivers for the disk follow
;;;	for now, we will simply store the parameters away for use
;;;	in the read and write subroutines
;;; === === === === === === === === === === === === === === === ===

;;; === === === === === === === === === === === === === === === ===
;;; disk controller i/o ports
;;; === === === === === === === === === === === === === === === ===

bdres	equ	10              ;reset controller (drv 0, trk 0, sec 1)
bddrv	equ	11              ;select drive
bdtrk	equ	12              ;select track
bdsec	equ	13              ;select sector
bddat	equ	14              ;read/write next byte

;;; === === === === === === === === === === === === === === === ===
;;; return the disk head of the currently selected disk (initially
;;; disk a) to the track 00 position. if your controller allows
;;; access to the track 0 flag from the device, step the head until
;;; the track 0 flag is detected. if your controller does not
;;; support this feature, you can translate the home call into a
;;; call on settrk with a parameter of 0.
;;; === === === === === === === === === === === === === === === ===

home:
        mvi     c,0
        call    settrk
        ret


;;; === === === === === === === === === === === === === === === ===
;;; select the disk drive given by register c for further opera-
;;; tions, where register c contains 0 for drive a, 1 for drive b,
;;; 2 for drive c, and 3 for drive d. (the standard cp/m
;;; distribution supports a maximum of four drives). if
;;; your system has less than 4 drives, you may wish to give an
;;; error message at the console, and terminate execution. it is
;;; advisable to postpone the actual disk operation until
;;; an i/o function (seek, read or write) is actually performed,
;;; since disk selects often occur without ultimately performing
;;; any disk i/o, and many controllers will unload the head of the
;;; current disk before selecting the new drive. this would
;;; cause an excessive amount of noise and disk wear.
;;;
;;; voidstar8080: this call resets the controller, in case a previous
;;; read or write operation did not complete properly. This is a "soft"
;;; reset; the disk array (real or simulated) state is not modified
;;; until we actually initiate a read or write.
;;;
;;; CP/M 2.2: locate and return the DPB for this drive in HL,
;;; or set HL to 0000h if the drive is not within our DPB table.
;;; === === === === === === === === === === === === === === === ===
seldsk:
	lxi	h, 0000h	;error return code
	mov	a, c
        out     bdres           ;reset bdev controller
        out     bddrv           ;select bdev drive number

	cpi	4		;must be between 0 and 3
	rnc			;no carry if 4, 5,...
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

;;; === === === === === === === === === === === === === === === ===
;;; register c contains the track number for subsequent disk
;;; accesses on the currently selected drive. you can choose to
;;; seek the selected track at this time, or delay the seek until
;;; the next read or write actually occurs. register c can take
;;; on values in the range 0-76 corresponding to valid track
;;; numbers.
;;;
;;; voidstar8080: pass the track number along to the controller.
;;; === === === === === === === === === === === === === === === ===

settrk:
	mov	a, c
        out     bdtrk
	ret

;;; === === === === === === === === === === === === === === === ===
;;; register c contains the sector number (1 through 26) for sub-
;;; sequent disk accesses on the currently selected drive. you
;;; can choose to send this information to the controller at this
;;; point, or instead delay sector selectino until a read
;;; or write opration occurs.
;;; === === === === === === === === === === === === === === === ===

setsec:
        mov     a,c
        out     bdsec
        ret

;;; === === === === === === === === === === === === === === === ===
;;; translate the sector given by bc using the
;;; translate table given by de
;;; === === === === === === === === === === === === === === === ===

sectran:
	xchg			;hl=.trans
	dad	b		;hl=.trans (sector)
	mov	l, m		;l=trans (sector)
	mvi	h, 0		;hl=trans (sector)
	ret			;with value in hl

;;; === === === === === === === === === === === === === === === ===
;;; registers b and c (high-order 8 bits in b, low-order 8 bits
;;; in c) contain the dma (direct memory access) address for sub-
;;; sequent read or write operations. for example, if b = 00h
;;; and c = 80h when setdma is called, then all subsequent read
;;; operations read their data into 80h through 0ffh, and all
;;; subsequent write operations get their data from 80h through
;;; 0ffh, until the next call to setdma occurs. the initial
;;; dma address is assumed to be 80h. note that the controller
;;; need not actually support direct memory access. if, for
;;; example, all data is received and sent through i/o ports, the
;;; cbios which you construct will use he 128-byte area starting
;;; at the selected dma address for the memory buffer during the
;;; following read or write operations.
;;; === === === === === === === === === === === === === === === ===

setdma:
	mov	l, c		;low order address
	mov	h, b		;high order address
	shld	dmaad		;save the address
	ret

;;; === === === === === === === === === === === === === === === ===
;;; assuming the drive has been selected, the track has been set,
;;; the sector has been set, and the dma address has been speci-
;;; fied, the read subroutine attempts to read one sector based
;;; upon these parameters, and returns the following error codes
;;; in register a:
;;;
;;;            0      no errors occurred
;;;            1      non-recoverable error condition occurred
;;;
;;; currently, cp/m responds only to a zero or non-zero value as
;;; the return code. that is, if the value in register a is 0
;;; then cp/m assumes that the disk operation completed properly.
;;; if an error occurs, however, the cbios should attempt at
;;; least 10 re-tries to see if the error is recoverable. when an
;;; error is reported the bdos will print the message "bdos err
;;; on x: bad sector." the operator then has the option of
;;; typing <cr> to ignore the error, or control-c to abort.
;;; === === === === === === === === === === === === === === === ===
read:   
        lhld    dmaad
        mvi     c,128
readl:
        in      bddat
        mov     m,a
        inx     hl
        dcr     c
        jnz     readl
        xra     a
        ret

;;; === === === === === === === === === === === === === === === ===
;;; write the data from the currently selected dma address to the
;;; currently selected drive, track, and sector. the data should
;;; be marked as "non deleted data" to maintain compatibility
;;; with other cp/m systems. the error codes given in the read
;;; command are returned in register a, with error recovery at-
;;; tempts as described above.
;;; === === === === === === === === === === === === === === === ===
write:  
        lhld    dmaad
        mvi     c,128
writel:
        mov     a,m
        out     bddat
        inx     hl
        dcr     c
        jnz     writel
        xra     a
        ret

endbios	equ	$

;;; === === === === === === === === === === === === === === === ===
;	the remainder of the cbios is reserved uninitialized
;	data area, and does not need to be a part of the
;	system memory image (the space must be available,
;	however, between the BIOS and the end of memory).
;
        org	bioslim
bss:    	 	;beginning of data area

dmaad:	ds	2	;direct memory address

dirbf:	ds	128	;scratch directory area
all00:	ds	31	;allocation vector 0
all01:	ds	31	;allocation vector 1
all02:	ds	31	;allocation vector 2
all03:	ds	31	;allocation vector 3
chk00:	ds	16	;check vector 0
chk01:	ds	16	;check vector 1
chk02:	ds	16	;check vector 2
chk03:	ds	16	;check vector 3
endbss:			;end of data area
        
bioslen	equ     endbios-bios        
biosavl	equ	bioslim-endbios	;can add this much to the BIOS segment

bsslen	equ	endbss-bss;	;size of bssa area
bssavl	equ	0280H-bsslen	;can add this much to the BSS segment

	end
