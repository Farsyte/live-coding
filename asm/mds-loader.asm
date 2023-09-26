;;; MDS LOADER MOVE PROGRAM, PLACES COLD START BOOT AT BOOTB
        ORG     3000H           ;WE ARE LOADED HERE ON COLD START
BOOTB   EQU     80H             ;START OF COLD BOOT PROGRAM
BOOTL   EQU     80H             ;LENGTH OF BOOT
MBIAS	EQU     900H-$          ;BIAS TO ADD DURING LOAD
BASE    EQU     078H            ;'BASE' USED BY DISK CONTROLLER
RTYPE   EQU     BASE+1          ;RESULT TYPE
RBYTE   EQU     BASE+3          ;RESULT TYPE
;;;
BSW     EQU     0FFH            ;BOOT SWITCH
;;;
;;; CLEAR DISK STATUS
        IN      RTYPE
        IN      RBYTE
;;;
COLDSTART:
        IN      BSW
        ANI     2H              ;SWITCH ON?
        JNZ     COLDSTART
;;;
        LXI     H,BOOTV         ;VIRTUAL BASE
        MVI     B,BOOTL         ;LENGTH OF BOOT
        LXI     D,BOOTB         ;DESTINATION OF BOOT
MOVE:   MOV     A,M
        STAX    D               ;TRANSFERRED ONE BYTE
        INX     H
        INX     D
        DCR     B
        JNZ     MOVE
        JMP     BOOTB           ;TO BOOT SYSTEM
;;;
BOOTV:	;BOOT LOADER PLACE HERE AT SYSTEM GENERATION
LBIAS   EQU     $-80H+MBIAS     ;COLD START BOOT BEGINS AT 80H
        END
