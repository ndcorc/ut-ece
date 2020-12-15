        .ORIG x3000
        LEA     R0, BYTEMASK    ; R0 has addr of MASK
        LDW     R1, R0, #0 	; R1 has x00FF
        LEA     R0, INPUT       ; R0 has addr of IN
        LDW     R2, R0, #0 	; R2 has x4000
        LDB     R3, R2, #0 	; R3 gets SEXT[x4000]
		BRz     exit1
        AND     R3, R3, R1      ; Mask top byte of R3
        LDB     R4, R2, #1      ; R4 gets SEXT[x4001]
		BRz     exit2
        AND     R4, R4, R1      ; Mask top byte of R4
		AND     R5,R5,#0    ;clear R3. R3 contains the product
l1      ADD     R5,R5,R3
        ADD     R4,R4, #-1
        BRp     l1
        LEA		R6, OUTPUT
        LDW		R7,R6,#0
        STW		R5,R7,#0
        HALT
	
exit1 	LEA		R6, OUTPUT
        LDW		R7,R6,#0
        STW		R3,R7,#0
        HALT
	
exit2 	LEA		R6, OUTPUT
        LDW		R7,R6,#0
        STW		R4,R7,#0
        HALT            

BYTEMASK .FILL   x00FF
INPUT    .FILL   x4000
OUTPUT   .FILL   x4002

	.END
