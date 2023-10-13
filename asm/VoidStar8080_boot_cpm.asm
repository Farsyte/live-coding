	TITLE	'CP/M Cold Start Loader for the (void *)8080 simulated microcomputer'

	;; This cold start loader resides on Track 00 Sector 1, the
	;; first sector of the diskette. It is loaded into memory at
	;; 0080h by the VoidStar8080 boot rom, and brings the CP/M
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
	;; CP/M 2.2 requires at least 20 KiB of RAM, which places the
	;; top of memory at 5000H. It can be relocated for larger
	;; memory systems, and the customary way is to compute a bias
	;; "b" which is simply how much more memory is available.
	;;
	;; The CP/M image containing the CCP, BDOS, and BIOS starting
	;; in Track 00 Sector 2 expects to reside at 3400h+b. The BIOS
	;; starting in Track 01 Sector 20 will reside at 4A00h+b, and
	;; the loaded image extends up to 4D00h+b. Memory above that
	;; point is left uninitialized by the loader.

	org	0		;base of memory

msize	equ	62		;min mem size in kbytes

bias	equ	(msize-20)*1024 ;offset from 20k system

text	equ	3400h+bias	;base of the ccp
start	equ	4A00h+bias	;base of the bios
etext	equ	4D00h+bias	;end of the bios
bytes	equ	etext-text	;size of cp/m image to load, in bytes

bddat	equ	14		;disk controller data port

	org	0080h		;ROM loads us here
cold	lxi     de,bytes
loop	in	bddat           ;get byte from disk
	mov	m,a             ;put byte to memory
	inx	hl
	dcr	e
	jnz	loop
	dcr	d
	jnz	loop
	jmp	start           ;turn control over to CP/M BIOS

	end
