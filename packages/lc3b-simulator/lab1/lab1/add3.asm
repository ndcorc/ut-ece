        .ORIG x3000
        LEA R0, USPTR
        LDW R6, R0, #0

        XOR R0, R0, R0
        ADD R0, R0, #1
        LEA R1, MOVLOC
        LDW R1, R1, #0
        STB R0, R1, #0

        XOR R0, R0, R0
;the 5-bit immediate number can only be [-16,15]
        ADD R0, R0, #10   ;R0: Counter
        XOR R1, R1, R1    ;R1: Sum
        LEA R2, ADDLOC
        LDW R2, R2, #0    ;R2: Intial Location
 

LOOP    LDB R3, R2, #0
        ADD R2, R2, #1
        ADD R1, R1, R3
        LDB R3, R2, #0
        ADD R2, R2, #1
        ADD R1, R1, R3
        ADD R0, R0, #-1
        BRp LOOP

;.FILL   xBF00

        LEA R2, SUMLOC
        LDW R2, R2, #0
        STW R1, R2, #0
        HALT

USPTR   .FILL xFE00
MOVLOC  .FILL x4000
ADDLOC  .FILL xC000
;SUMLOC  .FILL xC014
;SUMLOC  .FILL xC017
SUMLOC  .FILL x0000
        .END