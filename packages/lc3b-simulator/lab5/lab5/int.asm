            .ORIG x1200

            ;   PUSH
            ADD R6, R6, #-2
            STW R0, R6, #0
            ADD R6, R6, #-2
            STW R1, R6, #0
            ADD R6, R6, #-2
            STW R2, R6, #0

            LEA R0, TABLE
            LDW R0, R0, #0
            LEA R1, LEN
            LDW R1, R1, #0

LOOPB       LDW R2, R0, #0
            AND R2, R2, #-2
            STW R2, R0, #0
            ADD R0, R0, #2
            ADD R1, R1, #-1
            BRp LOOPB

            ;   POP
            LDW R2, R6, #0
            ADD R6, R6, #2
            LDW R1, R6, #0
            ADD R6, R6, #2
            LDW R0, R6, #0
            ADD R6, R6, #2
            RTI

TABLE       .FILL x1000
LEN         .FILL x0080
            .END
