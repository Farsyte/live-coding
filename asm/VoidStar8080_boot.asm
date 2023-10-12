        title	'Boot ROM for the (void *)8080 simulated microcomputer'

;; === === === === === === === === === === === === === === === ===
;; configuration constants for rom monitor and boot loader
;; current configuration is 48 kib of ram and 16 kib of rom.
;; === === === === === === === === === === === === === === === ===

rambdkb	equ     16              ;ram modules are 16 kib
rambdct	equ     3	        ;installed 3 ram modules

rombdkb	equ     2               ;rom modules are 2 kib
rombdct	equ     8	        ;installed 8 rom modules

ramkb   equ     rambdkb*rambdct
ramtop  equ     1024*ramkb
rombase equ     1024*(64-(rombdkb*rombdct))

;;; === === === === === === === === === === === === === === === ===
;;; Serial Device Ports
;;; === === === === === === === === === === === === === === === ===

ttyd	equ     0               ;read/write data from/to printing console
ttyc	equ     1               ;printing console status/control port

;;; === === === === === === === === === === === === === === === ===
;;; Disk Controller Ports
;;; === === === === === === === === === === === === === === === ===

bdres	equ	10              ;reset controller (drv 0, trk 0, sec 1)
bddrv	equ	11              ;select drive
bdtrk	equ	12              ;select track
bdsec	equ	13              ;select sector
bddat	equ	14              ;read/write next byte

;;; === === === === === === === === === === === === === === === ===
;;; scratch memory
;;; === === === === === === === === === === === === === === === ===

kbbuf   equ     ramtop-0100h

;;; The BOOT ROM provides very very very basic services and is
;;; not called by FORTH, COLD, or BIOS. It does not present a
;;; service entry point or a callable services vector.
;;; 
;;; At power on reset, the VoidStar8080 activates SHADOW, causing
;;; the BOOT ROM to service all memory reads. This will start execution
;;; with the code at the "cold" entry point but with PC=0.

	org     rombase
        

;;; The cold start entry point disables interrupts, then jumps to the
;;; warm start code. If reached via power-on RESET, this jump will
;;; cause the SHADOW mechanism to release.
cold:
        di
        jmp     warm

;;; The warm start entry point does a soft reset of the BDEV
;;; controller to terminate any partial read or write that was in
;;; progress, and displays a greeting message on the primary serial
;;; port (connected to a printing console).
warm:   
        lxi     sp, ramtop
        xra     a
        out     bdres

        call    conmsg
        db      "VoidStar8080 BOOT version 0.0.1",10,13,0

repl:
        call    conmsg
        db      "(void *) ",0

        lxi     hl, kbbuf
        call    gets
        cpi     0Ah
        jz      docmd
        cpi     0Dh
        jz      docmd
        call    conmsg
        db      10,13,"<cancel>",10,13,0

        ;; kbbuf will have NUL, then the typed line.
        ;; HL points at the
        ;; TODO dispatch to the command handler for the typed command

        jmp     repl


docmd:
        ei
        hlt
        jmp     repl

;;; === === === === === === === === === === === === === === === ===
;;; read a line of text from the console to (HL)
;;; returns to the byte after the nul.
;;; calling this "gets" because it has no buffer limit check.
;;; === === === === === === === === === === === === === === === ===
gets:
        mvi     m,0
        inx     h
        call    gets1
        mvi     m,0
        ret
gets1:  
        call    conget
        cpi     0Ah             ;LF -> line complete
        rz
        cpi     0Dh             ;CR -> line complete
        rz
        cpi     15h             ;SO (^U) -> kill line
        rz
        cpi     08h             ;BS (^H) -> backspace
        jz      getsbs
        cpi     7Fh             ;DEL (^?) -> backspace
        jz      getsbs

getsbs:
        dcx     hl
        mov     a,m             ;check for left margin
        inx     hl
        cpi     0
        rz                      ;bs at left margin -> return

        dcx     hl
        mvi     m,0             ;NUL the character from the kbbuf

        ;; our tty is special: if you print a space over something,
        ;; the something you printed before goes away.

        call    conmsg
        db      8,32,8,0
        jmp     gets
        

;;; === === === === === === === === === === === === === === === ===
;;; get next character from tty: into a and clear its parity bit.
;;; if there is no data available, wait for some.
;;; === === === === === === === === === === === === === === === ===
conget:
        in      ttyc
        ani     01h
        jz      conget
        in      ttyd
        ani     7fh
        ret


;;; === === === === === === === === === === === === === === === ===
;;; print NUL terminated string at (pc) to con:
;;; returns to the byte after the nul.
;;; === === === === === === === === === === === === === === === ===
conmsg:
        pop     hl
        call    constr
        pchl


;;; === === === === === === === === === === === === === === === ===
;;; print nul terminated string at (hl) to con:
;;; leaves hl pointing after the nul.
;;; === === === === === === === === === === === === === === === ===
constr:
        mov     a,m
        inx     h
        cpi     0
        rz
        mov     c,a
        call    conout
        jmp     constr


;;; === === === === === === === === === === === === === === === ===
;;; put the the character in c to the printing console
;;; if the device can not accept data, wait until it can.
;;; modifies the content of register a.
;;; === === === === === === === === === === === === === === === ===
conout: 
        in      ttyc
        ani     01h
        jz      conout
        mov     a,c
        out     ttyd
        ret



;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; device i/o ports
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	
;;;	ttyd	equ     0               ;read/write data from/to printing console
;;;	ttyc	equ     1               ;printing console status/control port
;;;	
;;;	crtd	equ     2               ;read/write data from/to video console
;;;	crtc	equ     3               ;video console status/control port
;;;	
;;;	lptd	equ     4               ;write data to line printer
;;;	lptc	equ     5               ;line printer status/control port
;;;	
;;;	pptd	equ     6               ;read/write data from/to paper tape
;;;	pptc	equ     7               ;paper tape status/control port
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; ram addresses known to the rom monitor
;;;	;;; note: should agree with cbios
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	
;;;	iobyte  equ     3               ;intel compatible i/o byte
;;;	cdisk	equ     4               ;current disk drive
;;;	
;;;	dmaad	equ	0040h		;last selected dma address
;;;	scr	equ     0080h           ;scratch buffer start
;;;	scrend	equ     00ffh           ;scratch buffer end
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; voidstar8080 reset behavior:
;;;	;;;
;;;	;;; when the system starts exectution after reset, the shadow
;;;	;;; facility is enabled in the address decoder, which routes all
;;;	;;; memory read operations to the boot rom. this facility is
;;;	;;; turned off the first time that the cpu issues a memory read
;;;	;;; to an address that maps normally into the boot rom.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; rom entry points currently match the cp/m bios
;;;	;;; but this is not actually needed in any way.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	
;;;	        org     rombase         ;boot operations
;;;	        jmp     cold            ;cold start
;;;	        jmp     warm            ;warm start
;;;	
;;;	        jmp     const           ;con status 00/ff to reg a
;;;	        jmp     conin           ;con get char to reg a
;;;	        jmp     conout          ;con put char from reg c
;;;	        jmp     list            ;lst put char from reg c
;;;	        jmp     punch           ;pun put char from reg c
;;;	        jmp     reader          ;rdr get char to reg a
;;;	
;;;	        jmp     home            ;move head to home position
;;;		jmp	seldsk          ;select disk
;;;		jmp	settrk          ;set track number
;;;		jmp	setsec          ;set sector number
;;;	        jmp     setdma          ;set "dma" address
;;;	        jmp     read            ;read disk
;;;	        jmp     write           ;write disk
;;;	
;;;		jmp	listst		;return list status
;;;		jmp	sectran		;sector translate
;;;	
;;;	        ;; unique to voidstar8080 rom: 
;;;	        jmp     constr           ;con dev message at (hl)
;;;	        jmp     conmsg           ;con dev message at (pc)
;;;	        jmp     lsstr           ;lst dev message at (hl)
;;;	        jmp     lsmsg           ;lst dev message at (pc)
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; monitor entry points
;;;	;;; - prompt for input
;;;	;;; - input a line of text with modest line editing
;;;	;;; - dispatch to command handler based on first character on line
;;;	;;; need this to poke around in memory before we have FORTH or CP/M
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	cold:
;;;	        ;; assure iobyte is sane.
;;;	        ;; 00h uses tty for console, reader, punch, and list.
;;;	        ;; 94h uses tty for console, ppt for reader and punch, and lpt for list.
;;;	        ;; 95h uses crt for console, ppt for reader and punch, and lpt for list.
;;;	        mvi     a,094h
;;;	        sta     iobyte
;;;	warm:
;;;	        call    conmsg
;;;	        db      10,13,10,"voidstar8080 version 0.0.1 [console]",10,13,0
;;;	        call    lsmsg
;;;	        db      10,13,10,"voidstar8080 version 0.0.1 [listing]",10,13,0
;;;	
;;;	        ;; todo copy cbios down to be00
;;;	        ;; todo copy forth down to 0100
;;;	
;;;	        ei
;;;	        hlt
;;;	        jmp     cold
;;;	        
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; return ff if a byte is available on tty: or 00 if not.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	ttyrdy:
;;;	        in      ttyc
;;;	        ani     02h
;;;	        rz
;;;	        mvi     a,0ffh
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; return ff if tty: can accept a byte or 00 if not.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	ttycw:
;;;	        in      ttyc
;;;	        ani     04h
;;;	        rz
;;;	        mvi     a,0ffh
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; return ff if a byte is available on crt: or 00 if not.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	crtrdy:
;;;	        in      crtc
;;;	        ani     02h
;;;	        rz
;;;	        mvi     a,0ffh
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; return ff if crt: can accept a byte or 00 if not.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	crtcw:
;;;	        in      crtc
;;;	        ani     04h
;;;	        rz
;;;	        mvi     a,0ffh
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; get next character from crt: into a and clear its parity bit.
;;;	;;; if there is no data available, wait for some.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	crtget:
;;;	        in      crtc
;;;	        ani     01h
;;;	        jz      crtget
;;;	        in      crtd
;;;	        ani     7fh
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; put the the character in c to crt: 
;;;	;;; if the device can not accept data, wait until it can.
;;;	;;; modifies the content of register a.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	crtput:
;;;	        in      crtc
;;;	        ani     01h
;;;	        jz      crtput
;;;	        mov     a,c
;;;	        out     crtd
;;;	        ret
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; return ff if a byte is available on ppt: or 00 if not.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	pptrdy:
;;;	        in      pptc
;;;	        ani     02h
;;;	        rz
;;;	        mvi     a,0ffh
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; get next character from ppt: into a and clear its parity bit.
;;;	;;; if there is no data available, wait for some.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	pptget:
;;;	        in      pptc
;;;	        ani     01h
;;;	        jz      pptget
;;;	        in      pptd
;;;	        ani     7fh
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; put the the character in c to ppt: 
;;;	;;; if the device can not accept data, wait until it can.
;;;	;;; modifies the content of register a.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	pptput:
;;;	        in      pptc
;;;	        ani     01h
;;;	        jz      pptput
;;;	        mov     a,c
;;;	        out     pptd
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; return ff if lpt: can accept a byte or 00 if not.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	lptcw:
;;;	        in      lptc
;;;	        ani     04h
;;;	        rz
;;;	        mvi     a,0ffh
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; put the the character in c to lpt: 
;;;	;;; if the device can not accept data, wait until it can.
;;;	;;; modifies the content of register a.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	lptput:
;;;	        in      lptc
;;;	        ani     01h
;;;	        jz      lptput
;;;	        mov     a,c
;;;	        out     lptd
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; return ff if a byte is available on con: or 00 if not.
;;;	;;; use i/o byte to determine which device is the console.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	const:
;;;	        lda     iobyte
;;;	        ani     03h
;;;	        jz      ttyrdy
;;;	        cpi     1
;;;	        jz      crtrdy
;;;	        cpi     2
;;;	        jz      pptrdy
;;;	;;; user defined console 1 not implemented
;;;	        ei
;;;	        hlt
;;;	        ret
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; get next character from con: into a and clear its parity bit.
;;;	;;; if there is no data available, wait for some.
;;;	;;; use i/o byte to determine which device is the console.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	conin:
;;;	        lda     iobyte
;;;	        ani     03h
;;;	        jz      ttyget
;;;	        cpi     1
;;;	        jz      crtget
;;;	        cpi     2
;;;	        jz      pptget
;;;	;;; user defined console 1 not implemented
;;;	        ei
;;;	        hlt
;;;	        ret
;;;	
;;;	
;;;	=== === === === === === === === === === === === === === === ===
;;;	;;; get next character from rdr: into a and clear its parity bit.
;;;	;;; if there is no data available, wait for some.
;;;	;;; use i/o byte to determine which device is the reader.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	reader:
;;;	        lda     iobyte
;;;	        ani     0ch
;;;	        jz      ttyget
;;;	        cpi     08h
;;;	        jz      pptget
;;;	;;; user defined readers 1 and 2 not implemented
;;;	        ei
;;;	        hlt
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; put the the character in c to pun: 
;;;	;;; if the device can not accept data, wait until it can.
;;;	;;; modifies the content of register a.
;;;	;;; use i/o byte to determine which device is the console.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	punch:
;;;	        lda     iobyte
;;;	        ani     030h
;;;	        jz      ttyput
;;;	        cpi     010h
;;;	        jz      pptput
;;;	;;; user defined punches 1 and 2 not implemented
;;;	        ei
;;;	        hlt
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; put the the character in c to lst: 
;;;	;;; if the device can not accept data, wait until it can.
;;;	;;; modifies the content of register a.
;;;	;;; use i/o byte to determine which device is the console.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	list:
;;;	        lda     iobyte
;;;	        ani     0c0h
;;;	        jz      ttyput
;;;	        cpi     040h
;;;	        jz      crtput
;;;	        cpi     080h
;;;	        jz      lptput
;;;	;;; user defined list device 1 not implemented
;;;	        ei
;;;	        hlt
;;;	        ret
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; put nul terminated string at (hl) to lst.
;;;	;;; leaves hl pointing after the nul.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	lsstr:
;;;	        mov     a,m
;;;	        inx     h
;;;	        cpi     0
;;;	        rz
;;;	        mov     c,a
;;;	        call    list
;;;	        jmp     lsstr
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; put nul terminated string at (pc) to ls.
;;;	;;; returns to the byte after the nul.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	lsmsg:
;;;	        pop     hl
;;;	        call    lsstr
;;;	        pchl
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; return the disk head of the currently selected disk (initially
;;;	;;; disk a) to the track 00 position. if your controller allows
;;;	;;; access to the track 0 flag from the device, step the head until
;;;	;;; the track 0 flag is detected. if your controller does not
;;;	;;; support this feature, you can translate the home call into a
;;;	;;; call on settrk with a parameter of 0.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	
;;;	home:
;;;	        mvi     c,0
;;;	        call    settrk
;;;	        ret
;;;	
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; select the disk drive given by register c for further opera-
;;;	;;; tions, where register c contains 0 for drive a, 1 for drive b,
;;;	;;; 2 for drive c, and 3 for drive d. (the standard cp/m
;;;	;;; distribution supports a maximum of four drives). if
;;;	;;; your system has less than 4 drives, you may wish to give an
;;;	;;; error message at the console, and terminate execution. it is
;;;	;;; advisable to postpone the actual disk operation until
;;;	;;; an i/o function (seek, read or write) is actually performed,
;;;	;;; since disk selects often occur without ultimately performing
;;;	;;; any disk i/o, and many controllers will unload the head of the
;;;	;;; current disk before selecting the new drive. this would
;;;	;;; cause an excessive amount of noise and disk wear.
;;;	;;;
;;;	;;; voidstar8080: this call resets the controller, in case a previous
;;;	;;; read or write operation did not complete properly.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	seldsk:
;;;	        mov     a,c
;;;	        out     bdres           ;reset bdev controller
;;;	        out     bddrv           ;select bdev drive number
;;;	        ret
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; register c contains the track number for subsequent disk
;;;	;;; accesses on the currently selected drive. you can choose to
;;;	;;; seek the selected track at this time, or delay the seek until
;;;	;;; the next read or write actually occurs. register c can take
;;;	;;; on values in the range 0-76 corresponding to valid track
;;;	;;; numbers.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	settrk:
;;;	        mov     a,c
;;;	        out     bdtrk
;;;	        ret
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; register c contains the sector number (1 through 26) for sub-
;;;	;;; sequent disk accesses on the currently selected drive. you
;;;	;;; can choose to send this information to the controller at this
;;;	;;; point, or instead delay sector selectino until a read
;;;	;;; or write opration occurs.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	setsec:
;;;	        mov     a,c
;;;	        out     bdsec
;;;	        ret
;;;	
;;;	;;; registers b and c (high-order 8 bits in b, low-order 8 bits
;;;	;;; in c) contain the dma (direct memory access) address for sub-
;;;	;;; sequent read or write operations. for example, if b = 00h
;;;	;;; and c = 80h when setdma is called, then all subsequent read
;;;	;;; operations read their data into 80h through 0ffh, and all
;;;	;;; subsequent write operations get their data from 80h through
;;;	;;; 0ffh, until the next call to setdma occurs. the initial
;;;	;;; dma address is assumed to be 80h. note that the controller
;;;	;;; need not actually support direct memory access. if, for
;;;	;;; example, all data is received and sent through i/o ports, the
;;;	;;; cbios which you construct will use he 128-byte area starting
;;;	;;; at the selected dma address for the memory buffer during the
;;;	;;; following read or write operations.
;;;	
;;;	setdma:
;;;	        mov     l,c
;;;	        mov     h,b
;;;	        shld    dmaad
;;;	        ret
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; assuming the drive has been selected, the track has been set,
;;;	;;; the sector has been set, and the dma address has been speci-
;;;	;;; fied, the read subroutine attempts to read one sector based
;;;	;;; upon these parameters, and returns the following error codes
;;;	;;; in register a:
;;;	;;;
;;;	;;;            0      no errors occurred
;;;	;;;            1      non-recoverable error condition occurred
;;;	;;;
;;;	;;; currently, cp/m responds only to a zero or non-zero value as
;;;	;;; the return code. that is, if the value in register a is 0
;;;	;;; then cp/m assumes that the disk operation completed properly.
;;;	;;; if an error occurs, however, the cbios should attempt at
;;;	;;; least 10 re-tries to see if the error is recoverable. when an
;;;	;;; error is reported the bdos will print the message "bdos err
;;;	;;; on x: bad sector." the operator then has the option of
;;;	;;; typing <cr> to ignore the error, or control-c to abort.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	read:   
;;;	        lhld    dmaad
;;;	        mvi     c,128
;;;	readl:
;;;	        in      bddat
;;;	        mov     m,a
;;;	        inx     hl
;;;	        dcr     c
;;;	        jnz     readl
;;;	        xra     a
;;;	        ret
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; write the data from the currently selected dma address to the
;;;	;;; currently selected drive, track, and sector. the data should
;;;	;;; be marked as "non deleted data" to maintain compatibility
;;;	;;; with other cp/m systems. the error codes given in the read
;;;	;;; command are returned in register a, with error recovery at-
;;;	;;; tempts as described above.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	write:  
;;;	        lhld    dmaad
;;;	        mvi     c,128
;;;	writel:
;;;	        mov     a,m
;;;	        out     bddat
;;;	        inx     hl
;;;	        dcr     c
;;;	        jnz     writel
;;;	        xra     a
;;;	        ret
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; return the ready status of the list device. used by
;;;	;;; the despool program to improve console response during
;;;	;;; its operation. the value 00 is returned in a it the
;;;	;;; list device is not ready to accept a character, and
;;;	;;; 0ffh if a character can be sent to the printer. note
;;;	;;; that a 00 value always suffices.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	listst:
;;;	        lda     iobyte
;;;	        ani     0c0h
;;;	        jz      ttycw
;;;	        cpi     040h
;;;	        jz      crtcw
;;;	        cpi     080h
;;;	        jz      lptcw
;;;	;;; user defined list device 1 not implemented
;;;	        ei
;;;	        hlt
;;;	        ret
;;;	
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	;;; performs sector logical to physical sector translation.
;;;	;;; this is the simplest possible version.
;;;	;;; === === === === === === === === === === === === === === === ===
;;;	sectran:
;;;	        ei
;;;	        hlt
;;;	        ret
;;;	
