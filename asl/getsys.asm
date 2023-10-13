;;; GETSYS PROGRAM - READ TRACKS 0 AND 1 TO MEMORY AT 2880H
        .CPU    8080
;;; REGISTER                 USE
;;;    A             (SCRATCH REGISTER)
;;;    B             TRACK COUNT (0, 1)
;;;    C             SECTOR COUNT (1,2,...,26)
;;;    DE            (SCRATCH REGISTER PAIR)
;;;    HL            LOAD ADDRESS
;;;    SP            SET TO STACKJ ADDRESS
;;;
START:  LXI     SP,2880H        ;SET STACK POINTER TO SCRATCH AREA
        LXI     H, 2880H        ;SET BASE LOAD ADDRESS
        MVI     B, 0            ;START WITH TRACK 0
RDTRK:                          ;READ NEXT TRACK (INITIALLY 0)
        MVI     C, 1            ;START READING WITH SECTOR 1
RDSEC:                          ;READ NEXT SECTOR
        CALL    READSEC         ;USER SUPPLIED ROUTINE
        LXI     D,128           ;MOVE LOAD ADDRESS TO NEXT 1/2 PAGE
        DAD     D               ;HL = HL + 128
        INR     C               ;SECTOR = SECTOR + 1
        MOV     A,C             ;CHECK FOR END OF TRACK
        CPI     27
        JC      RDSEC           ;CARRY GENERATED IF SECTOR < 27
;;;
;;; ARRIVE HERE AT ENDOF TRACK, MOVE TO NEXT TRACK
        INR     B
        MOV     A,B             ;TEST FOR LAST TRACK
        CPI     2
        JC      RDTRK           ;CARRY GENERATED IF TRACK < 2
;;;
;;; ARRIVE HERE AT END OF LOAD, HALT FOR NOW
        HLT
;;;
;;; USER-SUPPLIED SUBROUTINE TO READ THE DISK
READSEC:
;;; ENTER WITH TRACK NUMBER IN REGISTER B,
;;;       SECTOR NUMBER IN REGISTER C, AND
;;;       ADDRESS TO FILL IN HL
;;; 
        PUSH    B               ;SAVE B AND C REGISTERS
        PUSH    H               ;SAVE HL REGISTERS
;;;
;;; Perform disk read at thsi point, branch to
;;; label START if an error occurs
;;;
        POP     H               ;RECOVER HL
        POP     B               ;RECOVER B AND C REGISTERS
        RET                     ;BACK TO MAIN PROGRAM

        END     START
        
