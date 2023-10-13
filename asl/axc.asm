        ;; 2023-09-30 librarian note:
        ;;   retrieved from http://www.gaby.de/cpm/manuals/archive/cpm22htm/axc.asm
        ;;   adjusted for assembly with "asm8080"
        ;;   - stock assembler allows $ in label names, asm8080 does not.
        ;; add .cpu 8080 to allow assembly with ASL
        .cpu    8080
        

;		combined getsys and putsys programs from
;		Sec 6.4
;
;	Start the programs at the base of the TPA
	org 0100h

msize	equ 20			;size of cp/m in Kbytes

;"bias" is the amount to add to addresses for > 20K
;	(referred to as"b" throughout the text)
bias	equ	(msize-20)*1024
ccp	equ	3400h+bias
bdos	equ	ccp+0800h
bios	equ	ccp+1600h

;	getsys programs tracks 0 and 1 to memory at 3880h + bias
;	register	     usage
;	a		(scratch register)
;	b		track count (0...76)
;	c		sector count (1...26)
;	d,e		(scratch register pair)
;	h,l		load address
;	sp		set to track address

gstart:	;start of getsys
	lxi	sp,ccp-0080h	;convenient place
	lxi	h,ccp-0080h	;set initial load
	mvi	b,0		;start with track
rd_trk:	;read next track
	mvi	c,1		;each track start
rd_sec:
	call	read_sec	;get the next sector
	lxi	d,128		;offset by one sector
	dad	d		; (hl=hl+128)
	inr	c		;next sector
	mov	a,c		;fetch sector number
	cpi	27		;and see if last
	jc	rd_sec		;<, do one more

;arrive here at end of track, move to next track

	inr	b		;track = track+1
	mov	a,b		;check for last
	cpi	2		;track = 2 ?
	jc	rd_trk		;<, do another

;arrive here at end of load, halt for lack of anything 
;better

	ei
	hlt

;	putsys program, places memory image
;	starting at
;	3880h + bias back to tracks 0 and 1
;	start this program at the next page boundary
	org 0200h
put_sys:
	lxi 	sp,ccp-0080h 	;convenient place
	lxi 	h,ccp-0080h 	;start of dump
	mvi 	b,0 		;start with track 0 (first track)
wr_trk:
	mvi 	c,1 		;start with sector 1 (first sector)
wr_sec:
	call	write_sec	;write one sector
	lxi 	d,128 		;length of each
	dad	d		;<hl>=<hl> + 128
	inr	c		; <c>=<c> + 1
	mov	a,c		;see if
	cpi 	27 		;past end of track
	jc  	wr_sec  	;no, do another

;arrive here at end of track, move to next track

	inr	b		;track = track+1
	mov	a,b		;see if
	cpi	2		;last track
	jc	wr_trk		;no, do another


;	done with putsys, halt for lack of anything
;	better
	ei
	hlt


;user supplied subroutines for sector read and write

;	move to next page boundary
	org 0300h

read_sec:
	;read the next sector 
	;track in <b>, 
	;sector in <c> 
	;dmaaddr in<hl>

	push	b
	push	h

;user defined read operation goes here
	ds	64
	pop	h
	pop	b
	ret

	org ($+100h) && 0ff00h ;another page 
				; boundary
write_sec:

	;same parameters as read_sec

	push 	b
	push	h

;user defined write operation goes here
	ds	64
	pop	h
	pop	b
	ret

;end of getsys/putsys program

	end
