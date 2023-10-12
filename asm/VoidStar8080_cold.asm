	TITLE	'Cold Start Loader for the (void *)8080 simulated microcomputer'

	;; This cold start loader resides on Track 0 Sector 1 (the
	;; first sector of the diskette). This is loaded into memory
	;; upon system start-up. The cold start loader brings the CP/M
	;; system into memory at "loadp" (3400h+b), where b=0 for 20
	;; KiB systems, then increases with the memory size. after
	;; loading the CP/M system, the cold start loader branches to
	;; the bios at "3E00h+b". The cold start loader is not used
	;; until the system is powered up again, as long as the bios
	;; is not overwritten. the origin is assumed at 0000h, and
	;; must be changed if the controller brings the cold start
	;; loader into another area, or if a read/only memory area is
	;; used.

	org	0		;base of memory

msize	equ	20		;min mem size in kbytes

bias	equ	(msize-20)*1024 ;offset from 20k system
ccp	equ	3400h+bias	;base of the ccp
bios	equ	ccp+1600h	;base of the bios
biosl	equ	0300h		;length of the bios
boot	equ	bios
biose   equ     bios+biosl
size	equ	biose-ccp	;size of cp/m system
spt	equ     26
sectsz  equ     128
sects	equ	size/sectsz	;# of sectors to load

;;; === === === === === === === === === === === === === === === ===
;;; disk controller i/o ports
;;; === === === === === === === === === === === === === === === ===

bdres	equ	10              ;reset controller (drv 0, trk 0, sec 1)
bddrv	equ	11              ;select drive
bdtrk	equ	12              ;select track
bdsec	equ	13              ;select sector
bddat	equ	14              ;read/write next byte

; begin the load operations

        ;; register usage:
        ;;      b       track 0..1
        ;;      c       sector 1..26
        ;;      d       sectors remaining to load (50->0)
        ;;      e       bytes remaining to load (128->0)
        ;;      hl      load address (3400H+b->4D00H+b)

cold:
	lxi	bc,2		;select b=track 0, c=sector 2
	mvi	d,sects		;set up d = number of sectors to load
	lxi	h,ccp		;load point in h,l

        in      bddrv           ;persist selected disk
        out     bdres           ;reset bdev controller
        out     bddrv           ;reselect desired disk
read:   
        mov     a,b 
        out     bdtrk           ;select track
        mov     a,c
        out     bdsec           ;select sector
        mvi     e,sectsz
readl:                          ;this system uses PIO to move disk data.
        in      bddat           ;get byte from disk
        mov     m,a             ;store to memory.
        inx     hl
        dcr     e
        jnz     readl

; go to next sector if load is incomplete
	dcr	d		;sects=sects-1
	jz	boot+bias	;done, go to boot loader at 3e00h+bias
;
; more sectors to load
	inr	c		;sector=sector+1
	mov	a,c		;move sector count to a for compare
	cpi	spt+1		;last sector of track?
	jc	read		;no, go read another
;
; end of track, move to next track
	mvi	c,1		;sector=1
	inr	b		;track=track+1
	jmp	read		;for another sector
;
	end
