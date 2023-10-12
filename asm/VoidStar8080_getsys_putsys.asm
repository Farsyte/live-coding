        TITLE   'CP/M 2.2 combined GETSYS and PUTSYS programs for VoidStar8080'

	ORG	0100H
        
msize   equ     20              ;CP/M memory size

        ;; "bias" is the amount to add to addresses for > 20k
        ;;      (referred to as "b" throughout the text)

bias    equ     (msize-20)*1024
ccp     equ     3400h+bias
bdos    equ     ccp+0800h
bios    equ     ccp+1600h

	;; getsys reads tracks 0 and 1 to memory at 3380h+b
        ;; 
	;;	reg	use
	;;	a	-- scratch --
	;;	b	track count (0,1)
	;;	c	sector count (1,2,...,26)
	;;	de	-- scratch --
	;;	hl	load address
	;;	sp	set to stack address

getsys:
	lxi	sp,ccp-0080h    ; set stack pointer to scratch area
	lxi	hl,ccp-0080h    ; set base load address
	mvi	b,0             ; start with track 0
rdtrk:                          ; read next track (initially 0)
	mvi	c,1             ; read starting with sector 1
rdsec:                          ; read next sector
	call	secrd           ; user supplied subroutine
	lxi	de,128          ; move load address to next 1/2 page
	dad	de              ; hl = hl + 128
	inr	c               ; sector = sector + 1
	mov	a,c             ; check for end of track
	cpi	27
	jc	rdsec           ; carry generated if sector <27

;    arrive here at end of track, move to next track

	inr	b
	mov	a,b             ; test for last track
	cpi	2
	jc	rdtrk           ; carry generated if track <2

	ei
        hlt

        ;; putsys places memory image starting at 3880h+b
        ;; back to tracks 0 and 1
        ;; start this program at the next page boundary

        org     ($+0100h) and 0FF00h

	lxi	sp,ccp-0080h    ; set stack pointer to scratch area
	lxi	hl,ccp-0080h    ; set base load address
	mvi	b,0             ; start with track 0
wrtrk:                          ; read next track (initially 0)
	mvi	c,1             ; read starting with sector 1
wrsec:                          ; read next sector
	call	secwr           ; user supplied subroutine
	lxi	de,128          ; move load address to next 1/2 page
	dad	de              ; hl = hl + 128
	inr	c               ; sector = sector + 1
	mov	a,c             ; check for end of track
	cpi	27
	jc	wrsec           ; carry generated if sector <27

;    arrive here at end of track, move to next track

	inr	b
	mov	a,b             ; test for last track
	cpi	2
	jc	wrtrk           ; carry generated if track <2

	ei
        hlt

;    user supplied subroutines for sector read and write

;;; === === === === === === === === === === === === === === === ===
;;; disk controller i/o ports
;;; === === === === === === === === === === === === === === === ===

bdres	equ	10              ;reset controller (drv 0, trk 0, sec 1)
bddrv	equ	11              ;select drive
bdtrk	equ	12              ;select track
bdsec	equ	13              ;select sector
bddat	equ	14              ;read/write next byte

        ;; move to next page boundary

        org     ($+0100h) and 0FF00h

secrd:
        ;; read the next sector
        ;; track in <b>,
        ;; sector in <c>,
        ;; dmaaddr in <hl>

	push	bc              ; save b and c registers
	push	hl              ; save hl registers

        ;; user defined read operation
        in      bddrv           ;persist the current disk number
        out     bdres           ;reset the bdev controller
        out     bddrv           ;reselect the desired disk
        mov     a,b
        out     bdtrk           ;select the desired track
        mov     a,c
        out     bdsec           ;select the desired sector
        mvi     c,128
secrdl: 
        in      bddat
        mov     m,a
        inx     hl
        dcr     c
        jnz     secrdl
	pop	hl              ; recover hl
	pop	bc              ; recover b and c registers
	ret                     ; back to main program

        ;; move to next page boundary

        org     ($+0100h) and 0FF00h

secwr:  
        ;; write the next sector
        ;; track in <b>,
        ;; sector in <c>,
        ;; dmaaddr in <hl>

	push	bc              ; save b and c registers
	push	hl              ; save hl registers

        ;; user defined read operation
        in      bddrv           ;persist the current disk number
        out     bdres           ;reset the bdev controller
        out     bddrv           ;reselect the desired disk
        mov     a,b
        out     bdtrk           ;select the desired track
        mov     a,c
        out     bdsec           ;select the desired sector
        mvi     c,128
secwrl: 
        mov     a,m
        inx     hl
        out     bddat
        mov     m,a
        inx     hl
        dcr     c
        jnz     secwrl
	pop	hl              ; recover hl
	pop	bc              ; recover b and c registers
	ret                     ; back to main program

	end

