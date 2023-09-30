;;; boot rom lives in the top 2k of memory; during SHADOW,
;;; also serves memory reads at bottom 2K.

;;; === === === === === === === === === === === === === === === ===

        ORG     0F800h

rom:    JMP     .+3
        DI
        LXI     SP,0080h
