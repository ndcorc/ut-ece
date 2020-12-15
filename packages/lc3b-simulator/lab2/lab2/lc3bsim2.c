/*
 Name 1: Nolan Corcoran
 UTEID 1: ndc466
 */

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
 MEMORY[A][1] stores the most significant byte of word at word address A
 */

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];
int s;
/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
FILE * prog;    /* file variable */


typedef struct System_Latches_Struct{

    int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf(". for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char* program_filename) {
    /*FILE * prog;*/
    int ii, word, program_base;

    /* Open program file. */
    /*
    char path[50] = "../../../hw2tests/hw2_base_v3/";
    prog = fopen(strcat(path, program_filename), "r");
     */
    
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    /*printf("\n\nProgram: %s\n", program_filename);*/
    printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
    int i;

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 2) {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }
    while (1)
        get_command(dumpsim_file);
    
    /*
    printf("LC-3b Simulator\n\n");
    char* test = "webpage.hex";
    initialize(test, 1);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }
    while (1)
        get_command(dumpsim_file);
     */
}

/***************************************************************/
/*
 Do not modify the above code.
 You are allowed to use the following global variables in your
 code. These are defined above.

 MEMORY

 CURRENT_LATCHES
 NEXT_LATCHES

 You may define your own local/global variables and functions.
 You may use the functions to get at the control bits defined
 above.

 Begin your code here
*/

/***************************************************************/

#define bits2_0(x) (x & 0x0007)
#define bits3_0(x) (x & 0x000F)
#define bits4_0(x) (x & 0x001F)
#define bits5_0(x) (x & 0x003F)
#define bits8_0(x) (x & 0x01FF)
#define bits10_0(x) (x & 0x07FF)
#define bits7_0(x) (x & 0x00FF)
#define bit5(x) (x & 0x0020)
#define bit11(x) (x & 0x0800)
#define bits5_4(x) ((x & 0x0030) >> 4)
#define bits8_6(x) ((x & 0x01C0) >> 6)
#define bits11_9(x) ((x & 0x0E00) >> 9)


enum
{
    BR, ADD, LDB, STB, JSR, AND, LDW, STW,
    RTI, XOR, NA1, NA2, JMP, SHF, LEA, TRAP
};

enum
{
    BR_, BRP, BRZ, BRZP, BRN, BRNP, BRNZ, BRNZP
};

enum
{
    LSHF, RSHFL, NA, RSHFA
};

int instruction,
    opcode,
    dr,
    sr1,
    sr2,
    sr,
    srVal,
    offset,
    immediate,
    condition,
    shift,
    loc,
    i,
    temp,
    base,
    baseAddress,
    vector;

int SEXT_5(int val)
{
    if (val & 0x0010) val |= 0xFFFFFFE0;
    return val;
}

int SEXT_6(int val)
{
    if (val & 0x0020) val |= 0xFFFFFFC0;
    return val;
}

int SEXT_8(int val)
{
    if (val & 0x0080) val |= 0xFFFFFF00;
    return val;
}

int SEXT_9(int val)
{
    if (val & 0x0100) val |= 0xFFFFFE00;
    return val;
}

int SEXT_11(int val)
{
    if (val & 0x0400) val |= 0xFFFFF800;
    return val;
}

int SEXT(int val)
{
    if (val & 0x8000) val |= 0xFFFF0000;
    return val;
}

void set_condition_bits()
{
    condition = SEXT(NEXT_LATCHES.REGS[dr]);
    if (condition > 0)
    {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;
    }
    else if (condition < 0)
    {
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }
    else
    {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
}


void process_instruction(){
    /*  function: process_instruction
     *
     *    Process one instruction at a time
     *       -Fetch one instruction
     *       -Decode
     *       -Execute
     *       -Update NEXT_LATCHES
     */
    instruction = MEMORY[CURRENT_LATCHES.PC >> 1][0] + (MEMORY[CURRENT_LATCHES.PC >> 1][1] << 8);
    opcode = (instruction & 0xF000) >> 12;
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
    
    switch(opcode)
    {
        case ADD:
            dr = bits11_9(instruction);
            sr1 = bits8_6(instruction);
            if (bit5(instruction))
            {
                /* immediate */
                immediate = SEXT_5(bits4_0(instruction));
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT(CURRENT_LATCHES.REGS[sr1]) + immediate);
                printf("ADD\t\tR%d, R%d, #%d\n", dr, sr1, immediate);
            }
            else
            {
                /* register */
                sr2 = bits2_0(instruction);
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT(CURRENT_LATCHES.REGS[sr1]) +
                                                  SEXT(CURRENT_LATCHES.REGS[sr2]));
                printf("ADD\t\tR%d, R%d, R%d\n", dr, sr1, sr2);
            }
            set_condition_bits();
            break;

        case AND:
            dr = bits11_9(instruction);
            sr1 = bits8_6(instruction);
            if (bit5(instruction))
            {
                /* immediate */
                immediate = SEXT_5(bits4_0(instruction));
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT(CURRENT_LATCHES.REGS[sr1]) & immediate);
                printf("AND\t\tR%d, R%d, #%d\n", dr, sr1, immediate);
            }
            else
            {
                /* register */
                sr2 = bits2_0(instruction);
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT(CURRENT_LATCHES.REGS[sr1]) &
                                                  SEXT(CURRENT_LATCHES.REGS[sr2]));
                printf("AND\t\tR%d, R%d, R%d\n", dr, sr1, sr2);
            }
            set_condition_bits();
            break;

        case BR:
            condition = bits11_9(instruction);
            offset = SEXT_9(bits8_0(instruction)) << 1;
            switch (condition)
            {
                case BR_:
                case BRNZP:
                    NEXT_LATCHES.PC += offset;
                    printf("BR\t\t#%d\n", offset);
                    break;
                case BRN:
                    if (CURRENT_LATCHES.N == 1) NEXT_LATCHES.PC += offset;
                    printf("BRn\t\t#%d\n", offset);
                    break;
                case BRZ:
                    if (CURRENT_LATCHES.Z == 1) NEXT_LATCHES.PC += offset;
                    printf("BRz\t\t#%d\n", offset);
                    break;
                case BRP:
                    if (CURRENT_LATCHES.P == 1) NEXT_LATCHES.PC += offset;
                    printf("BRp\t\t#%d\n", offset);
                    break;
                case BRNZ:
                    if (CURRENT_LATCHES.N == 1 || CURRENT_LATCHES.Z == 1) NEXT_LATCHES.PC += offset;
                    printf("BRnz\t\t#%d\n", offset);
                    break;
                case BRZP:
                    if (CURRENT_LATCHES.Z == 1 || CURRENT_LATCHES.P == 1) NEXT_LATCHES.PC += offset;
                    printf("BRzp\t#%d\n", offset);
                    break;
                case BRNP:
                    if (CURRENT_LATCHES.N == 1 || CURRENT_LATCHES.P == 1) NEXT_LATCHES.PC += offset;
                    printf("BRnp\t#%d\n", offset);
                    break;
            }
            break;

        case JMP:
            base = bits8_6(instruction);
            printf("JMP\t\tR%d\n", base);
            NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[base];
            break;

        case JSR:
            temp = NEXT_LATCHES.PC;
            if (bit11(instruction))   /* JSR */
            {
                offset = SEXT_11(bits10_0(instruction)) << 1;
                NEXT_LATCHES.PC += offset;
                printf("JSR\t\t#%d\n", offset);
            }
            else                        /* JSRR */
            {
                base = bits8_6(instruction);
                NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[base];
                printf("JSRR\tR%d\n", base);
            }
            NEXT_LATCHES.REGS[7] = temp;
            break;

        case LDB:
            dr = bits11_9(instruction);
            base = bits8_6(instruction);
            offset = SEXT_6(bits5_0(instruction));
            baseAddress = CURRENT_LATCHES.REGS[base];
            NEXT_LATCHES.REGS[dr] = Low16bits(SEXT_8(MEMORY[(baseAddress + offset) >> 1][(baseAddress + offset) & 0x0001]));
            printf("LDB\t\tR%d, R%d, #%d\n", dr, base, offset);
            set_condition_bits();
            break;

        case LDW:
            dr = bits11_9(instruction);
            base = bits8_6(instruction);
            offset = SEXT_6(bits5_0(instruction)) << 1;
            baseAddress = CURRENT_LATCHES.REGS[bits8_6(instruction)];
            NEXT_LATCHES.REGS[dr] = MEMORY[(baseAddress + offset) >> 1][0];
            NEXT_LATCHES.REGS[dr] += (MEMORY[(baseAddress + offset) >> 1][1] << 8);
            printf("LDW\t\tR%d, R%d, #%d\n", dr, base, offset);
            set_condition_bits();
            break;

        case LEA:
            dr = bits11_9(instruction);
            offset = SEXT_9((bits8_0(instruction))) << 1;
            NEXT_LATCHES.REGS[dr] = offset + NEXT_LATCHES.PC;
            printf("LEA\t\tR%d, #%d\n", dr, offset);
            break;

        case XOR:
            dr = bits11_9(instruction);
            sr1 = bits8_6(instruction);
            if (bit5(instruction))
            {
                /* immediate */
                immediate = SEXT_5(bits4_0(instruction));
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT(CURRENT_LATCHES.REGS[sr1]) ^ immediate);
                printf("XOR\t\tR%d, R%d, #%d\n", dr, sr1, immediate);
            }
            else
            {
                /* register */
                sr2 = bits2_0(instruction);
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT(CURRENT_LATCHES.REGS[sr1]) ^
                                                  SEXT(CURRENT_LATCHES.REGS[sr2]));
                printf("XOR\t\tR%d, R%d, R%d\n", dr, sr1, sr2);
            }
            set_condition_bits();
            break;

        case SHF:
            dr = bits11_9(instruction);
            sr = bits8_6(instruction);
            srVal = CURRENT_LATCHES.REGS[sr];
            shift = bits5_4(instruction);
            immediate = bits3_0(instruction);
            switch (shift)
            {
                case LSHF:
                    NEXT_LATCHES.REGS[dr] = Low16bits(srVal << immediate);
                    set_condition_bits();
                    printf("LSHF\tR%d, R%d, #%d\n", dr, sr, immediate);
                    break;
                case RSHFL:
                    for (i = 0; i < immediate; i++)
                    {
                        srVal = ~(1 << 15) & (srVal >> 1);
                    }
                    NEXT_LATCHES.REGS[dr] = Low16bits(srVal);
                    set_condition_bits();
                    printf("RSHFL\tR%d, R%d, #%d\n", dr, sr, immediate);
                    break;
                case RSHFA:
                    for (i = 0; i < immediate; i++)
                    {
                        srVal = (srVal & 0x8000) + (srVal >> 1);
                    }
                    NEXT_LATCHES.REGS[dr] = Low16bits(srVal);
                    set_condition_bits();
                    printf("RSHFA\tR%d, R%d, #%d\n", dr, sr, immediate);
                    break;
            }
            break;

        case STB:
            sr = bits11_9(instruction);
            base = bits8_6(instruction);
            offset = SEXT_6(bits5_0(instruction));
            /* execute */
            srVal = CURRENT_LATCHES.REGS[sr];
            baseAddress = CURRENT_LATCHES.REGS[base];
            MEMORY[(baseAddress + offset) >> 1][(baseAddress + offset) & 0x0001] = srVal & 0x00FF;
            printf("STB\t\tR%d, R%d, #%d\n", sr, base, offset);
            break;

        case STW:
            sr = bits11_9(instruction);
            base = bits8_6(instruction);
            offset = SEXT_6(bits5_0(instruction)) << 1;
            /* execute */
            srVal = CURRENT_LATCHES.REGS[sr];
            baseAddress = CURRENT_LATCHES.REGS[base];
            MEMORY[(baseAddress + offset) >> 1][0] = srVal & 0x00FF;
            MEMORY[(baseAddress + offset) >> 1][1] = (srVal & 0xFF00) >> 8;
            printf("STW\t\tR%d, R%d, #%d\n", sr, base, offset);
            break;

        case TRAP:
            NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
            vector = bits7_0(instruction);
            NEXT_LATCHES.PC = MEMORY[vector][0] + (MEMORY[vector][1] << 8);
            printf("TRAP\tx%x\n", vector);
            break;

        case NA1:
        case NA2:
            break;
    }

}
