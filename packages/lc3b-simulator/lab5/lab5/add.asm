        .ORIG x3000
        LEA R0, PTR
        LDW R6, R0, #0

        XOR R0, R0, R0

        ADD R0, R0, #10   ;COUNT
        XOR R1, R1, R1    ;SUM
        LEA R2, ADDER
        LDW R2, R2, #0

LOOPA    LDB R3, R2, #0
        ADD R2, R2, #1
        ADD R1, R1, R3
        LDB R3, R2, #0
        ADD R2, R2, #1
        ADD R1, R1, R3
        ADD R0, R0, #-1
        BRp LOOPA

        ;.FILL   xBF00

        LEA R2, TOTAL
        LDW R2, R2, #0
        STW R1, R2, #0

        JMP R1

        HALT

PTR         .FILL xFE00
ADDER       .FILL xC000
TOTAL       .FILL xC014
;TOTAL      .FILL xC017
;TOTAL      .FILL x0000
            .END
