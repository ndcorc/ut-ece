------------------------
Grading for ncorcoran
------------------------
An "x" in the "Incorrect" column indicates a test has failed

=========================================================================
Part A: Tests for individual instructions (1.75 points each)
Each of these compares the result of rdump & mdump after run 1
=========================================================================
Name of     Instruction      Number of   Incorrect   Points
Test Case                    Differences
=========================================================================
initial                      0                 (Not a test case)
1           ADD R1, R2, R3   0                       1.75
2           ADD R1, R2, x5   0                       1.75
3           ADD R1, R1, x-5  0                       1.75
4           ADD R1, R1, x-1  0                       1.75
5           AND R0, R5, R6   0                       1.75
6           AND R3, R6, #-6  0                       1.75
7           XOR R1, R2, R3   0                       1.75
8           XOR R1, R2, xa   0                       1.75
9           NOT R1, R1       0                       1.75
10          NOT R7, R1       0                       1.75
11          TRAP x25         0                       1.75
12          TRAP 255         0                       1.75
13          STW R7, R6, #0   1              x        1
14          STB R7, R6, #0   0                       1.75
15          STW R7, R6, #1   1              x        1
16          STB R7, R6, #1   0                       1.75
17          LDW R0, R6, #0   0                       1.75
18          LDB R0, R6, #0   0                       1.75
19          LDW R0, R6, #1   0                       1.75
20          LDB R0, R6, #1   0                       1.75
21          LEA R2, label    0                       1.75
22          JMP R2           0                       1.75
23          JSRR R2          0                       1.75
24          JSRR R7          1              x        1
25          JSR  label       1              x        1
26          RET              1              x        1
27          LSHF R2, R3, #4  0                       1.75
28          LSHF R2, R3, #0  0                       1.75
29          RSHFL R2, R6, #4 0                       1.75
30          RSHFA R2, R6, #4 0                       1.75
31          BRn label        0                       1.75
32          BRz label        0                       1.75
33          BRp label        0                       1.75
34          BRnp label       0                       1.75
35          BRzp label       0                       1.75
36          BR label         0                       1.75
=========================================================================
Part B: Tests for functionality of programs (2.5 points each, except lea)
=========================================================================
Instruction     Number of       Incorrect       Points
Differences
=========================================================================
add             0                               2.5
and             0                               2.5
br_noz          3                  x            1
jmp             1                  x            2
jsr             2                  x            1.5
jsrr            2                  x            1.5
ldb             0                               2.5
ldw             0                               2.5
lea             0                               2
lshf            3                  x            1
rshfa           3                  x            1
rshfl           4                  x            0.5
stb             2                  x            1.5
stw             2                  x            1.5
xor             0                               2.5
=========================================================================
Assignment 2: Total points 85 / 100
