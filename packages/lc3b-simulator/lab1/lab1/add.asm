				.ORIG	x3000

				AND 	R0, R0, #0
				ADD 	R0, R0, #1
				LEA 	R1, INIT
				LDW		R1, R1, #0		; R1 = x4000
				STW 	R0, R1, #0

				AND 	R0, R0, #0		; R0 = accumulator
				AND		R1, R1, #0
				ADD 	R1, R1, #10		; counter = 20 bytes / 10 words
				LEA 	R2, DATA
				LDW		R2, R2, #0		; R2 = xC000

NEXT			LDB 	R3, R2, #0		; DATA[0:7]
				ADD 	R0, R0, R3
				LDB		R3, R2, #1		; DATA[15:8]
				ADD		R0, R0, R3
				ADD		R2, R2, #2		; address += 2
				ADD 	R1, R1, #-1		; counter -= 1
				BRp		NEXT

;UNKNOWN		.FILL	x000A

				LEA		R2, SUM			
				STW		R0, R2, #0

				HALT

INIT			.FILL	x4000
DATA			.FILL	xC000
;SUM				.FILL	xC014
;SUM		.FILL	x0000				; protection exception
SUM				.FILL	xC017	; unaligned exception
				.END