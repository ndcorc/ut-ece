	.ORIG x3000
	LEA r5, T1
	LDW r7, r5, #0 
	ADD r0, r1, #7
	ADD r2, r0, r0
	AND r3, r0, r2
	AND r4, r3, #3
	JMP r7

	TRAP x25 ;TRAP
    NOT r1, r3      ; x3010
	LSHF r6, r1, #4
	RSHFL r0, r6, #2
	RSHFA r1, r6, #2
	LEA r2, T2
	LDW r7, r2, #0
    STB r1, r7, #1  ; MEM[R7+1] = R1
    STB r3, r7, #0  ; MEM[R7] = R3

    ; MEM[X4000] == x

	TRAP x25
T1	.FILL x3010
T2	.FILL x4000
	.END
