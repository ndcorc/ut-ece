                .ORIG x1200

                STW R0, R6, #-1         
                STW R1, R6, #-2       

                ; PUSH
                LEA     R0, COUNT           ; COUNT
                LDW     R0, R0, #0          ; x4000
                LDW     R1, R0, #0          ; MEM[R0]
                ADD     R1, R1, #1          ; R1 +=1
                STW     R1, R0, #0 

                ; POP
                LDW R1, R6, #-2         ; R1 = MEM[R6-4]
                LDW R0, R6, #-1         ; R0 = MEM[R6-2]

                RTI

COUNT           .FILL x4000
                .END