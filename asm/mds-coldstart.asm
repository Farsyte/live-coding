;;; 
;;; mds cold start loader for cp/m
;;; version 1.4 january, 1978
;;;
BIAS    EQU     100h            ;bias for relocation
FALSE   EQU     0
TRUE    EQU     1
TESTING	EQU     0               ;IF TRUE, THEN GO TO MON80 ON ERRORS
;;;
BDOSB   EQU     BIAS            ;BASE OF DOS LOAD
BDOS    EQU     806H+BIAS       ;ENTRY TO DOS FOR CALLS
BDOSE   EQU     1700H+BIAS      ;END OF DOS LOAD
BOOT    EQU     1500H+BIAS      ;COLD START ENTRY POINT
RBOOT   EQU     BOOT+3          ;WARM START ENTRY POINT
;;;
        ORG     80H             ;LOADED DOWN FROM HARDWARE BOOT AT 3000H
;;;
BDOSL   EQU     BDOSE-BDOSB
NTRKS   EQU     2               ;NUMBER OF TRACKS TO READ
BDOSS   EQU     BDOSL/128       ;NUMBER OF SECTORS IN DOS
BDOS0   EQU     25              ;NUMBER OF BDOS SECTORS ON TRACK 0
BDOS1   EQU     BDOSS-BDOS0     ;NUMBER OF SECTORS ON TRACK 1
;;;
MON80   EQU     0F800H          ;INTEL MONITOR BASE
RMON80  EQU     0FF0FH          ;RESTART LOCATION FOR MON80
BASE    EQU     078H            ;'BASE' USED BY CONTROLLER
RTYPE   EQU     BASE+1          ;RESULT TYPE
RBYTE   EQU     BASE+3          ;RESULT BYTE
RESET   EQU     BASE+7          ;RESET CONTROLLER
;;;
DSTAT   EQU     BASE            ;DISK STATUS PORT
ILOW    EQU     BASE+1          ;LOW IOPB ADDRESS
IHIGH   EQU     BASE+2          ;HIGH IOPB ADDRESS
RECAL   EQU     3H              ;RECALIBRATE SELECTED DRIVE
READF   EQU     4H              ;DISK READ FUNCTION
STACK   EQU     100H            ;USE END OF BOOT FOR STACK
;;;
RSTART:
        LXI     SP,STACK        ;IN CASE OF CALL TO MON80
;;; CLEAR THE CONTROLLER
        OUT     RESET           ;LOGIC CLEARED
;;;
;;;
        MVI     B,NTRKS         ;NUMBER OF TRACKS TO READ
        LXI     H,IOPB0
;;;
START:
;;;
;;; READ FIRST/NEXT TRACK INTO BDOSB
        MOV     A,L
        OUT     ILOW
        MOV     A,H
        OUT     IHIGH
WAIT0:  IN      DSTAT
        ANI     4
        JZ      WAIT0
;;;
;;; CHECK DISK STATUS
        IN      RTYPE
        ANI     11B
        CPI     2
;;; 
        IF      TESTING
        CNC     RMON80          ;GO TO MONITOR IF 11 OR 10
        ELSE
        JNC     RSTART          ;RETRY THE LOAD
        ENDIF
;;;
        IN      RBYTE           ;I/O COMPLETE, CHECK STATUS
;;; IF NOT READY, THEN GO TO MON80
        RAL
        CC      RMON80          ;NOT READY BIT SET
        RAR                     ;RESTORE
        ANI     11110B          ;OVERRUN/ADDR ERR/SEEK/CRC/XXXX
;;;
        IF      TESTING
        CNZ     RMON80          ;GO TO MONITOR
        ELSE
        JNZ     RSTART          ;RETRY THE LOAD
        ENDIF
;;;
;;;
        LXI     D,IOPBL         ;LENGTH OF IOPB
        DAD     D               ;ADDRESSING NEXT IOPB
        DCR     B               ;COUNT DOWN TRACKS
        JNZ     START
;;;
;;;
;;; JMP TO BOOT TO PRINT INITIAL MESSAGE, AND SET UP JMPS
        JMP     BOOT
;;;
;;; PARAMETER BLOCKS
IOPB0:  DB      80H             ;IOCW, NO UPDATE
        DB      READF           ;READ FUNCTION
        DB      BDOS0           ;# SECTORS TO READ ON TRACK 0
        DB      0               ;TRACK 0
        DB      2               ;START WITH SECTOR 2 ON TRACK 0
        DW      BDOSB           ;START AT BASE OF BIOS
IOPBL   EQU     $-IOPB0
;;; 
IOPB1:  DB      80H
        DB      READF
        DB      BDOS1           ;SECTORS TO READ ON TRACK 1
        DB      1               ;TRACK 1
        DB      1               ;SECTOR 1
        DW      BDOSB+BDOS0*128 ;BASE OF SECOND READ
;;;
        END
        
















        
