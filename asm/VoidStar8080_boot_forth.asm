	TITLE	'FIG-FORTH Cold Start Loader for the (void *)8080 simulated microcomputer'

	;; This cold start loader resides on Track 00 Sector 1, the
	;; first sector of the diskette. It is loaded into memory at
	;; 0080h by the VoidStar8080 boot rom, and brings the FIG-FORTH
	;; system into memory at the proper location (see below), then
	;; starts it running.
	;;
	;; This cold start loader presumes that the BDEV controller
	;; has just read this program from Track 00 Sector 01, and
	;; makes use of its Auto-Increment capabilities to simply
	;; continue reading the disk sequentially at the next sector.
	;;
	;; This cold start loader is not used again, unless the BIOS
	;; is overwritten, at which time this loader will again be
	;; read from the disk into memory.
        ;;
        ;; FIG-FORTH lives at 0100h, and the minimum FIG-FORTH image
        ;; extends to just short of 1C00h, requiring 54 sectors,
        ;; which is larger than the two tracks used by CP/M.

text	equ	0100h		;FIG-FORTH load address
start	equ	0100h           ;FIG-FORTH cold entry point
etext	equ	1C00h		;FIG-FORTH end of disk image
bytes	equ	etext-text	;size of FIG-FORTH image to load, in bytes

bps     equ     128
sects   equ     bytes/bps

bddat	equ	14		;disk controller data port

	org	0000h		;ROM loads us here

cold	lxi     de,bytes
loop	in	bddat           ;get byte from disk
	mov	m,a             ;put byte to memory
	inx	hl
	dcr	e
	jnz	loop
	dcr	d
	jnz	loop
	jmp	start           ;turn control over to FIG-FORTH.

	end
