;;; boot rom lives in the top 2k of memory; during SHADOW,
;;; also serves memory reads at bottom 2K.

;;; === === === === === === === === === === === === === === === ===

;;; Simulation Control Output Port
;;; - write zero to this port to turn off SHADOW.
;;; - anything else halts the simulation.
COPORT	EQU     255             ;simulation control port

;;; Simulation Status Input Port
;;; - returns status code TBD.
CIPORT	EQU     255             ;simulation control port

MEMKB	EQU     48
MEMTOP  EQU     (MEMKB*1000)

        ORG     0F800h

;;; ZERO: entry point used at RESET, when SHADOW maps this rom
;;; to memory reads at 0x0000

ZERO:
        DI
        JMP     HIGH

;;; HIGH: arrival location in high memory. shut off SHADOW
;;;  and enter the SYSTEM IDLE loop.

HIGH:
        XRA     A
        OUT     COPORT

        LXI     SP,MEMTOP

;;; FUTURE: read boot record from mass storage into memory at known
;;; address, then start the boot sector running; it will load and run
;;; more data from mass storage.

;;; IDLE: Enable interrupts and halt the CPU.
;;; 
;;; RESET will restart us at ZERO.
;;; 
;;; INTERRUPT requests will service the request then resume execution
;;; after the HLT, which will just branch back to IDLE.
;;;
;;; The intent is that an operator will have the ability to assert
;;; RESET, or to issue INT with a "JMP" to code to be executed.

IDLE:
        EI
        HLT
        JMP     IDLE
