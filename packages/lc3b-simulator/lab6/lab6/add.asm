		.ORIG	x3000
		LEA		r5, BPTR
		LDW		r2, r5, #0
		LDW		r3, r5, #1
		LDW		r6, r5, #2
		AND		r0, r0, #0
L1		LDB		r1,	r2, #0
		ADD		r0, r0, r1
		ADD		r2, r2, #1
		ADD		r3, r3, #-1
		BRP		L1
		STW		r0, r2, #0
		TRAP	x25
BPTR	.FILL	xC000
Ctr		.FILL	x14
USP		.FILL	xFE00
		.END
		