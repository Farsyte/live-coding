        ;; Minimal BDEV Boot Rom for VoidStar8080
        ;; must appear at 0080h when we come out of RESET
        ;; mechanism to release SHADOW is currently TBD

        .CPU    8080

        ORG     0000h

BDRES	EQU	10              ;RESET CONTROLLER (DRV 0, TRK 0, SEC 1)
BDDAT	EQU	14              ;READ/WRITE NEXT BYTE

HIGH:   LXI     H,0080H
        SPHL
        PUSH    H
        OUT     BDRES
LOOP:   IN      BDDAT
        MOV     M,A
        INR     L
        JNZ     LOOP
        RET
