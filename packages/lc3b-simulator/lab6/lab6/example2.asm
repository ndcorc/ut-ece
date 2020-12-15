            .ORIG x3000
            LEA R0, ANSWER
            ADD R1, R1, #5
            ADD R2, R2, #4
            ADD R3, R3, #3
            ADD R4, R4, #2
            LDW R5, R0, #0
            ADD R6, R3, R3	
            HALT
ANSWER      .FILL x1234
            .END
