/*
 Name 1: Nolan Corcoran
 UTEID 1: ndc466
 */

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

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
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {
    IRD,
    COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX,
    SR1MUX,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    /* MODIFY: you have to add all your new control signals */
    COND2,
    LD_R6,
    LD_E,
    LD_SSP,
    LD_USP,
    LD_PSR,
    GATE_PSR,
    GATE_VECTOR,
    PSRMUX1, PSRMUX0,
    OLDMUX,
    R6MUX1, R6MUX0,
    CCMUX,
    RESET_I,
    RESET_E,

    LD_VA,
    LD_J,
    LD_TEMP,
    LD_VPE,
    LD_R,
    GATE_PTBR,
    GATE_VA,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
 MEMORY[A][1] stores the most significant byte of word at word address A
 There are two write enable signals, one for each byte. WE0 is used for
 the least significant byte of a word. WE1 is used for the most significant
 byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{
    
    int PC,		/* program counter */
        MDR,	/* memory data register */
        MAR,	/* memory address register */
        IR,		/* instruction register */
        N,		/* n condition bit */
        Z,		/* z condition bit */
        P,		/* p condition bit */
        BEN;        /* ben register */
    
    int READY;	/* ready bit */
    /* The ready bit is also latched as you dont want the memory system to assert it
     at a bad point in the cycle*/
    
    int REGS[LC_3b_REGS]; /* register file. */
    
    int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */
    
    int STATE_NUMBER; /* Current State Number - Provided for debugging */
    
    /* For lab 4 */
    int INTV;       /* Interrupt vector register */
    int EXCV;       /* Exception vector register */
    int SSP;        /* System stack pointer */
    int USP;        /* User stack pointer */
    int PSR;        /* Program status register */
    int I;          /* Interrupt condition bit */
    int E;          /* Exception condition bit */
    /* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */
    
    /* For lab 5 */
    int PTBR; /* This is initialized when we load the page table */
    int VA;   /* Temporary VA register */
    int TEMP;
    
    /* MODIFY: you should add here any other registers you need to implement virtual memory */
    
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD() {
    return (CURRENT_LATCHES.MICROINSTRUCTION[IRD]);
}
int GetCOND0() {
    return (CURRENT_LATCHES.MICROINSTRUCTION[COND0]);
}
int GetCOND1() {
    return (CURRENT_LATCHES.MICROINSTRUCTION[COND1]);
}
int GetR() {
    return (CURRENT_LATCHES.READY);
}
int GetBEN() {
    return (CURRENT_LATCHES.BEN);
}
int GetIR_11() {
    return (CURRENT_LATCHES.IR & 0x0800) >> 11;
}
int GetJ() {
    return((CURRENT_LATCHES.MICROINSTRUCTION[J5] << 5) +
           (CURRENT_LATCHES.MICROINSTRUCTION[J4] << 4) +
           (CURRENT_LATCHES.MICROINSTRUCTION[J3] << 3) +
           (CURRENT_LATCHES.MICROINSTRUCTION[J2] << 2) +
           (CURRENT_LATCHES.MICROINSTRUCTION[J1] << 1) +
           (CURRENT_LATCHES.MICROINSTRUCTION[J0]));
}
int GetMIO_EN() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[MIO_EN]);
}
int GetR_W() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[R_W]);
}
int GetDATA_SIZE() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[DATA_SIZE]);
}
int GetSR1MUX() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[SR1MUX]);
}
int GetADDR1MUX() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[ADDR1MUX]);
}
int GetADDR2MUX() {
    return((CURRENT_LATCHES.MICROINSTRUCTION[ADDR2MUX1] << 1) + CURRENT_LATCHES.MICROINSTRUCTION[ADDR2MUX0]);
}
int GetLSHF1() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LSHF1]);
}
int GetLD_MAR() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_MAR]);
}
int GetLD_MDR() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_MDR]);
}
int GetLD_IR() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_IR]);
}
int GetLD_BEN() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_BEN]);
}
int GetLD_REG() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_REG]);
}
int GetLD_CC() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_CC]);
}
int GetLD_PC() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_PC]);
}
int GetGATE_PC() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[GATE_PC]);
}
int GetGATE_MDR() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[GATE_MDR]);
}
int GetGATE_ALU() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[GATE_ALU]);
}
int GetGATE_MARMUX() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[GATE_MARMUX]);
}
int GetGATE_SHF() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[GATE_SHF]);
}
int GetPCMUX() {
    return((CURRENT_LATCHES.MICROINSTRUCTION[PCMUX1] << 1) + CURRENT_LATCHES.MICROINSTRUCTION[PCMUX0]);
}
int GetDRMUX() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[DRMUX]);
}
int GetMARMUX() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[MARMUX]);
}
int GetALUK() {
    return((CURRENT_LATCHES.MICROINSTRUCTION[ALUK1] << 1) + CURRENT_LATCHES.MICROINSTRUCTION[ALUK0]);
}
/* Functions to get the new control signal bits. */
int GetCOND2() {
    return (CURRENT_LATCHES.MICROINSTRUCTION[COND2]);
}
int GetGATE_PSR() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[GATE_PSR]);
}
int GetGATE_VECTOR() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[GATE_VECTOR]);
}
int GetLD_R6() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_R6]);
}
int GetLD_E() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_E]);
}
int GetLD_SSP() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_SSP]);
}
int GetLD_USP() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_USP]);
}
int GetLD_PSR() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_PSR]);
}
int GetPSRMUX() {
    return((CURRENT_LATCHES.MICROINSTRUCTION[PSRMUX1] << 1) + CURRENT_LATCHES.MICROINSTRUCTION[PSRMUX0]);
}
int GetOLDMUX() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[OLDMUX]);
}
int GetR6MUX() {
    return((CURRENT_LATCHES.MICROINSTRUCTION[R6MUX1] << 1) + CURRENT_LATCHES.MICROINSTRUCTION[R6MUX0]);
}
int GetCCMUX() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[CCMUX]);
}
int GetRESET_I() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[RESET_I]);
}
int GetRESET_E() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[RESET_E]);
}
int GetLD_VA() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_VA]);
}
int GetLD_J() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_J]);
}
int GetLD_TEMP() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_TEMP]);
}
int GetLD_VPE() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_VPE]);
}
int GetLD_R() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[LD_R]);
}
int GetGATE_PTBR() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[GATE_PTBR]);
}
int GetGATE_VA() {
    return(CURRENT_LATCHES.MICROINSTRUCTION[GATE_VA]);
}
/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {
    
    eval_micro_sequencer();
    cycle_memory();
    eval_bus_drivers();
    drive_bus();
    latch_datapath_values();
    
    CURRENT_LATCHES = NEXT_LATCHES;
    
    CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;
    
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }
    
    printf("Simulating for %d cycles...\n\n", num_cycles);
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
/* Purpose   : Simulate the LC-3b until HALTed.                 */
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
    
    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");
    
    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");
    
    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
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
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {
    FILE *ucode;
    int i, j, index;
    char line[200];
    
    printf("Loading Control Store from file: %s\n", ucode_filename);
    
    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
        printf("Error: Can't open micro-code file %s\n", ucode_filename);
        exit(-1);
    }
    
    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
        if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
            printf("Error: Too few lines (%d) in micro-code file: %s\n",
                   i, ucode_filename);
            exit(-1);
        }
        
        /* Put in bits one at a time. */
        index = 0;
        
        for (j = 0; j < CONTROL_STORE_BITS; j++) {
            /* Needs to find enough bits in line. */
            if (line[index] == '\0') {
                printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
                       ucode_filename, i);
                exit(-1);
            }
            if (line[index] != '0' && line[index] != '1') {
                printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
                       ucode_filename, i, j);
                exit(-1);
            }
            
            /* Set the bit in the Control Store. */
            CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
            index++;
        }
        
        /* Warn about extra bits in line. */
        if (line[index] != '\0')
            printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
                   ucode_filename, i);
    }
    printf("\n");
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
void load_program(char *program_filename, int is_virtual_base) {
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;
    
    /* Open program file. */
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
    
    if (is_virtual_base) {
        if (CURRENT_LATCHES.PTBR == 0) {
            printf("Error: Page table base not loaded %s\n", program_filename);
            exit(-1);
        }
        
        /* convert virtual_base to physical_base */
        virtual_pc = program_base << 1;
        pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) |
        MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];
        
        printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
        if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
            program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
            printf("physical base of program: %x\n\n", program_base);
            program_base = program_base >> 1;
        } else {
            printf("attempting to load a program into an invalid (non-resident) page\n\n");
            exit(-1);
        }
    }
    else {
        /* is page table */
        CURRENT_LATCHES.PTBR = program_base << 1;
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
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
        ii++;
    }
    
    if (CURRENT_LATCHES.PC == 0 && is_virtual_base)
        CURRENT_LATCHES.PC = virtual_pc;
    
    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *pagetable_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_store(ucode_filename);
    
    init_memory();
    load_program(pagetable_filename,0);
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename,1);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.PSR = 0x8002;   /* user mode, Z=1 */
    CURRENT_LATCHES.SSP = 0x3000;   /* Initial value of system stack pointer */
    CURRENT_LATCHES.USP = 0xFE00;   /* Initial value of user stack pointer */
    
    /* MODIFY: you can add more initialization code HERE */
    
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

    if (argc < 4) {
        printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }
    
    printf("LC-3b Simulator\n\n");
    
    initialize(argv[1], argv[2], argv[3], argc - 3);

/*
    char* arg2[] = {
        "ucode5", "pagetable.hex", "add.hex", "data.hex", "vector_table.hex", "int.hex",
        "except_prot.hex", "except_unaligned.hex", "except_unknown.hex"
    };
    initialize(arg2[0], arg2[1], arg2[2], 7);
*/

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }
    
    while (1)
        get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code, except for the places indicated
 with a "MODIFY:" comment.
 You are allowed to use the following global variables in your
 code. These are defined above.
 
 CONTROL_STORE
 MEMORY
 BUS
 
 CURRENT_LATCHES
 NEXT_LATCHES
 
 You may define your own local/global variables and functions.
 You may use the functions to get at the control bits defined
 above.
 
 Begin your code here 	  			       */
/***************************************************************/
#define bit0(x)    (x & 0x0001)
#define bit1(x)    (x & 0x0002)
#define bit2(x)    (x & 0x0004)
#define bits2_0(x) (x & 0x0007)
#define bits3_0(x) (x & 0x000F)
#define bits4_0(x) (x & 0x001F)
#define bits5_0(x) (x & 0x003F)
#define bits8_0(x) (x & 0x01FF)
#define bits10_0(x) (x & 0x07FF)
#define bits7_0(x) (x & 0x00FF)
#define bit5(x) (x & 0x0020)
#define bit9(x) (x & 0x0200)
#define bit10(x) (x & 0x0400)
#define bit11(x) (x & 0x0800)
#define bit15(x) (x & 0x8000)
#define bits5_4(x) ((x & 0x0030) >> 4)
#define bits8_6(x) ((x & 0x01C0) >> 6)
#define bits11_9(x) ((x & 0x0E00) >> 9)
#define bits13_9(x) ((x & )
#define bits15_8(x) ((x & 0xFF00) >> 8)
#define bits15_12(x) ((x & 0xF000) >> 12)

#define pbit(x) ((x & 0x0008) >> 3)
#define vbit(x) ((x & 0x0004) >> 2)
#define mbit(x) ((x & 0x0002) >> 1)
#define rbit(x) (x & 0x0001)

#define vpn(x) ((x & 0xFE00) >> 9)
#define offset(x) (x & 0x01FF)
#define pfn(x) ((x & 0x3E00) >> 9)


int sumI = 0;

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

enum {
    ADD, AND, XOR, PASS
};

enum {
    LSHF, RSHFL, NA, RSHFA
};


int ready, branch, addr_mode, interrupt, J,
next_state, mdr_data, protection_exc, unaligned_exc, unknown_exc,
sr1_out, sr2_out, dr_out, addr1_out, addr2_out, pc_out, ptbr_out,
mar_out, alu_out, shf_out, mdr_out, psr_out, vector_out, va_out,
dr, sr, r6, srVal, shift, immediate, i, currentPC;

int mem_cycle = 1;

int j_state() {
    addr_mode = (GetCOND1() & GetCOND0() & GetIR_11());
    ready = (!GetCOND1() & GetCOND0() & GetR()) << 1;
    branch = (GetCOND1() & !GetCOND0() & GetBEN()) << 2;
    interrupt = (GetCOND2() & CURRENT_LATCHES.I) << 4;
    return (ready | branch | addr_mode | interrupt | J);
}

int get_opcode() {
    return (CURRENT_LATCHES.IR & 0xF000) >> 12;
}

void latch_instruction(int state) {
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[state], sizeof(int)*CONTROL_STORE_BITS);
    NEXT_LATCHES.STATE_NUMBER = state;
    /*
    if (CURRENT_LATCHES.STATE_NUMBER == 49) {
        printf("Entering service routine\n");
        int t;
        printf("Current Microinstruction: ");
        for (t = 0; t < 51; t++) {
            printf("%d", CURRENT_LATCHES.MICROINSTRUCTION[t]);
        }
        printf("\n");
        printf("Next Latches Microinstruction: ");
        for (t = 0; t < 51; t++) {
            printf("%d", NEXT_LATCHES.MICROINSTRUCTION[t]);
        }
        printf("\n");
    }
     */
}

void set_CC() {
    if (!GetCCMUX()) {                      /* set cc as normal */
        if (Low16bits(BUS)) {               /* either positive or negative */
            if ((BUS & 0x8000) >> 15) {     /* MSB */
                NEXT_LATCHES.N = 1;
                NEXT_LATCHES.P = 0;
                NEXT_LATCHES.Z = 0;
            }else {
                NEXT_LATCHES.N = 0;
                NEXT_LATCHES.P = 1;
                NEXT_LATCHES.Z = 0;
            }
        } else {                            /* zero */
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.P = 0;
            NEXT_LATCHES.Z = 1;
        }
    } else {                                /* set cc to PSR[2:0] */
        NEXT_LATCHES.N = bit2(BUS) >> 2;
        NEXT_LATCHES.Z = bit1(BUS) >> 1;
        NEXT_LATCHES.P = bit0(BUS);
    }
    
}

void set_BEN() {
    NEXT_LATCHES.BEN = (CURRENT_LATCHES.N & bit11(CURRENT_LATCHES.IR) >> 11)  |
    (CURRENT_LATCHES.Z & bit10(CURRENT_LATCHES.IR) >> 10) |
    (CURRENT_LATCHES.P & bit9(CURRENT_LATCHES.IR) >> 9);
}

void eval_micro_sequencer() {
    
    /*
     * Evaluate the address of the next state according to the
     * micro sequencer logic. Latch the next microinstruction.
     */
    
  /*
    if (CURRENT_LATCHES.STATE_NUMBER == 47) {
        printf("Entering system program\n");
    }
    
    if (CURRENT_LATCHES.STATE_NUMBER == 8) {
        printf("RTI\n");
    }
    
    if (CURRENT_LATCHES.STATE_NUMBER == 50) {
        printf("Returning to user program\n");
    }
 */
    if (CURRENT_LATCHES.E) {  /* if EXC bit set, next state is 49 */
        next_state = 49;
    }
    else if (GetLD_TEMP() && !(GetCOND2() & CURRENT_LATCHES.I)) {
        next_state = 54;
        NEXT_LATCHES.TEMP = GetJ();
    }
    else if (GetIRD() == 0) {    /* use J bits */
        J = GetLD_J() ? CURRENT_LATCHES.TEMP : GetJ();
        next_state = j_state();
    }
    else {
        next_state = get_opcode();
        unknown_exc = (next_state == 10 || next_state == 11) ? 1 : 0;
        /*
         if (!unknown_exc && ((CYCLE_COUNT > 154 && CYCLE_COUNT < 340) || CYCLE_COUNT > 490)) {
         
         switch (next_state) {
         case 0:
         printf("BRp:\n");
         break;
         case 1:
         switch(sumI) {
         case 0:
         case 2:
         printf("ADD R2, R2, #1\n");
         sumI += 1;
         break;
         case 1:
         case 3:
         printf("ADD R1, R1, R3\n");
         sumI += 1;
         break;
         case 4:
         printf("ADD R0, R0, #-1\n");
         sumI = 0;
         break;
         }
         break;
         case 2:
         printf("LDB R3, R2, #0:\n");
         break;
         case 3:
         printf("STB:\n");
         break;
         case 4:
         printf("JSR(R):\n");
         break;
         case 5:
         printf("AND:\n");
         break;
         case 6:
         printf("LDW:\n");
         break;
         case 7:
         printf("STW:\n");
         break;
         case 8:
         printf("RTI:\n");
         break;
         case 9:
         printf("XOR:\n");
         break;
         case 12:
         printf("JMP:\n");
         break;
         case 13:
         printf("SHF:\n");
         break;
         case 14:
         printf("LEA:\n");
         break;
         case 15:
         printf("TRAP:\n");
         break;
         }
         
         printf("Current PC: 0x%x\n", currentPC);
         printf("R0: %d (COUNTER)\n", CURRENT_LATCHES.REGS[0]);
         printf("R1: 0x%x (SUM)\n", CURRENT_LATCHES.REGS[1]);
         printf("R2: 0x%x (MEMORY ADDRESS)\n", CURRENT_LATCHES.REGS[2]);
         printf("R3: 0x%x (MEM[R2])\n", CURRENT_LATCHES.REGS[3]);
         printf("\n");
         }*/
    }
    latch_instruction(next_state);
}


void cycle_memory() {
    
    /*
     * This function emulates memory and the WE logic.
     * Keep track of which cycle of MEMEN we are dealing with.
     * If fourth, we need to latch Ready bit at the end of
     * cycle to prepare microsequencer for the fifth cycle.
     */
    if (GetMIO_EN()) {
        if (CURRENT_LATCHES.READY) {            /* ready to read or write */
            NEXT_LATCHES.READY = 0;             /* reset ready bit and cycle count */
            mem_cycle = 1;
        }
        else {
            if (mem_cycle == 4) {               /* latch ready bit */
                NEXT_LATCHES.READY = 1;
            }
            mem_cycle += 1;
            mdr_data = CURRENT_LATCHES.MDR;
        }
    }
    
    if (CYCLE_COUNT == 300) {               /* interrupt */
        NEXT_LATCHES.I = 1;
        NEXT_LATCHES.INTV = 0x01;
        printf("Interrupt triggered - State: %d\n", CURRENT_LATCHES.STATE_NUMBER);
        /*NEXT_LATCHES.IE = 0;*/            /* temporarily disabled for all new interrupts */
    }
    
}



void eval_bus_drivers() {
    
    /*
     * Datapath routine emulating operations before driving the bus.
     * Evaluate the input of tristate drivers
     *       Gate_MARMUX,
     *       Gate_PC,
     *       Gate_ALU,
     *       Gate_SHF,
     *       Gate_MDR,
     *       Gate_PSR,
     *       Gate_VECTOR
     *       Gate_PTBR
     *       Gate_VA
     */
    
    /*********************
     *                   *
     *    Gate_MARMUX    *
     *                   *
     *********************/
    if (GetLD_R6()) {
        sr1_out = CURRENT_LATCHES.REGS[6];
    }
    else {
        sr1_out = GetSR1MUX() ? CURRENT_LATCHES.REGS[bits8_6(CURRENT_LATCHES.IR)] : CURRENT_LATCHES.REGS[bits11_9(CURRENT_LATCHES.IR)];
    }
    addr1_out = GetADDR1MUX() ? sr1_out : CURRENT_LATCHES.PC;
    switch(GetADDR2MUX()) {
        case 0:
            addr2_out = 0;
            break;
        case 1:
            addr2_out = Low16bits(SEXT_6(bits5_0(CURRENT_LATCHES.IR)));
            break;
        case 2:
            addr2_out = Low16bits(SEXT_9(bits8_0(CURRENT_LATCHES.IR)));
            break;
        case 3:
            addr2_out = Low16bits(SEXT_11(bits10_0(CURRENT_LATCHES.IR)));
            break;
    }
    if (GetMARMUX()) {
        if (GetLSHF1()) {
            mar_out = addr1_out + (addr2_out << 1);
        } else {
            mar_out = addr1_out + addr2_out;
        }
    } else {
        mar_out = Low16bits(bits7_0(CURRENT_LATCHES.IR) << 1);
    }
    
    /*******************
     *                 *
     *    Gate_ALU     *
     *                 *
     *******************/
    
    sr2_out = bit5(CURRENT_LATCHES.IR) ? Low16bits(SEXT_5(bits4_0(CURRENT_LATCHES.IR))) : CURRENT_LATCHES.REGS[bits2_0(CURRENT_LATCHES.IR)];
    
    switch(GetALUK()) {
        case ADD:
            alu_out = Low16bits(sr1_out + sr2_out);
            break;
        case AND:
            alu_out = Low16bits(sr1_out & sr2_out);
            break;
        case XOR:
            alu_out = Low16bits(sr1_out ^ sr2_out);
            break;
        case PASS:
            alu_out = Low16bits(sr1_out);
            break;
    }
    
    /*******************
     *                 *
     *    Gate_SHF     *
     *                 *
     *******************/
    
    sr = sr1_out;
    immediate = bits3_0(CURRENT_LATCHES.IR);
    switch (bits5_4(CURRENT_LATCHES.IR)) {
        case LSHF:
            shf_out = Low16bits(sr << immediate);
            break;
        case RSHFL:
            for (i = 0; i < immediate; i++){
                sr = ~(1 << 15) & (sr >> 1);
            }
            shf_out = Low16bits(sr);
            break;
        case RSHFA:
            for (i = 0; i < immediate; i++)
            {
                sr = (sr & 0x8000) + (sr >> 1);
            }
            shf_out = Low16bits(sr);
            break;
    }
    
    /*******************
     *                 *
     *    Gate_MDR     *
     *                 *
     *******************/
    
    if (bit0(CURRENT_LATCHES.MAR)) { /* Odd Address */
        if (GetDATA_SIZE()) {
            mdr_out = CURRENT_LATCHES.MDR;
        }
        else {
            mdr_out = Low16bits(SEXT_8(bits7_0(CURRENT_LATCHES.MDR >> 8)));
        }
    } else {
        mdr_out = GetDATA_SIZE() ? Low16bits(CURRENT_LATCHES.MDR) : SEXT_8(bits7_0(CURRENT_LATCHES.MDR));
    }
    
    
    /********************
     *                  *
     *    Gate_VECTOR   *
     *                  *
     ********************/
    
    if (CURRENT_LATCHES.I) {          /* interrupt condition set */
        vector_out = Low16bits(0x0200 + (CURRENT_LATCHES.INTV << 1));   /* vector_out = 0x0202 */
    } else if (CURRENT_LATCHES.E) {   /* exception condition set */
        vector_out = Low16bits(0x0200 + (CURRENT_LATCHES.EXCV << 1));   /* vector_out = 0x0204, 0x0206, or 0x0208 */
    }
    
    /********************
     *                  *
     *    Gate_PTBR     *
     *                  *
     ********************/
    
    ptbr_out = Low16bits(CURRENT_LATCHES.PTBR + (vpn(CURRENT_LATCHES.VA) << 1));
    
    /********************
     *                  *
     *     Gate_VA      *
     *                  *
     ********************/
    
    va_out = (pfn(CURRENT_LATCHES.MDR) << 9) + offset(CURRENT_LATCHES.VA);
    
}


void drive_bus() {
    
    /*
     * Datapath routine for driving the bus from one of the 7 possible
     * tristate drivers.
     *       Gate_MARMUX,
     *       Gate_PC,
     *       Gate_ALU,
     *       Gate_SHF,
     *       Gate_MDR,
     *       Gate_PSR,
     *       Gate_VECTOR
     */
    
    if (GetGATE_MARMUX()) {
        BUS = mar_out;
    }
    else if (GetGATE_PC()) {
        BUS = CURRENT_LATCHES.PC;
    }
    else if (GetGATE_ALU()) {
        BUS = alu_out;
    }
    else if (GetGATE_SHF()) {
        BUS = shf_out;
    }
    else if (GetGATE_MDR()) {
        BUS = mdr_out;
    }
    else if (GetGATE_PSR()) {
        BUS = CURRENT_LATCHES.PSR;
    }
    else if (GetGATE_VECTOR()) {
        BUS = vector_out;
    }
    else if (GetGATE_PTBR()) {
        BUS = ptbr_out;
    }
    else if (GetGATE_VA()) {
        BUS = va_out;
    }
    else {
        BUS = 0;
    }
}


void latch_datapath_values() {
    
    /*
     * Datapath routine for computing all functions that need to latch
     * values in the data path at the end of this cycle.  Some values
     * require sourcing the bus; therefore, this routine has to come
     * after drive_bus.
     */

    
    if (GetLD_MAR()) {
        NEXT_LATCHES.MAR = Low16bits(BUS);
        if (GetLD_E()) {
            if ((CURRENT_LATCHES.STATE_NUMBER != 2) &&
                (CURRENT_LATCHES.STATE_NUMBER != 3) ) {                            /* not LDB or STB */
                if (BUS & 1) {                                                          /* unaligned access exception */
                    printf("UNALIGNED, State: %d\n", CURRENT_LATCHES.STATE_NUMBER);
                    NEXT_LATCHES.E = 1;
                    NEXT_LATCHES.EXCV = 0x03;
                }
            }
        }
    }
    
    if (GetLD_VPE()) {
        if (bit15(CURRENT_LATCHES.PSR) && !pbit(CURRENT_LATCHES.MDR) && (get_opcode() != 15)) {
            printf("PROTECTED, State: %d\n", CURRENT_LATCHES.STATE_NUMBER);
            NEXT_LATCHES.E = 1;
            NEXT_LATCHES.EXCV = 0x04;
        }
        else if (!vbit(CURRENT_LATCHES.MDR)) {
            printf("PAGE FAULT, State: %d\n", CURRENT_LATCHES.STATE_NUMBER);
            NEXT_LATCHES.E = 1;
            NEXT_LATCHES.EXCV = 0x02;
        }
    }
    
    if (GetLD_MDR()) {
        if (GetMIO_EN()) {
            if (CURRENT_LATCHES.READY) {
                NEXT_LATCHES.MDR = Low16bits((MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) & 0xFF00) +
                ((MEMORY[CURRENT_LATCHES.MAR >> 1][0]) & 0x00FF);
            }
            else {
                NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR;
            }
        }
        else {
            if (GetDATA_SIZE()) {
                NEXT_LATCHES.MDR = Low16bits(BUS);
            }
            else {          /* MDR separated into different high and low bytes */
                NEXT_LATCHES.MDR = Low16bits(((BUS & 0x00FF) << 8) + (BUS & 0x00FF));
            }
        }
    }
    
    if (GetMIO_EN() && GetR_W()) {          /* WE */
        if (GetDATA_SIZE()) {               /* word */
            MEMORY[CURRENT_LATCHES.MAR>>1][1] = (CURRENT_LATCHES.MDR & 0xFF00) >> 8;
            MEMORY[CURRENT_LATCHES.MAR>>1][0] = CURRENT_LATCHES.MDR & 0x00FF;
        } else {                            /* byte */
            if (CURRENT_LATCHES.MAR & 1) {  /* high byte */
                MEMORY[CURRENT_LATCHES.MAR>>1][1] = (CURRENT_LATCHES.MDR & 0xFF00) >> 8;
            }
            else {                          /* low byte */
                MEMORY[CURRENT_LATCHES.MAR>>1][0] = CURRENT_LATCHES.MDR & 0x00FF;
            }
        }
    }
    
    if (GetLD_IR()) {
        NEXT_LATCHES.IR = Low16bits(BUS);
    }
    
    if (GetLD_REG()) {
        if (GetLD_R6()) {
            switch(GetR6MUX()) {
                case 0:
                    r6 = CURRENT_LATCHES.REGS[6];
                    NEXT_LATCHES.REGS[6] = GetOLDMUX() ? r6+2 : r6-2;
                    break;
                case 1:
                    NEXT_LATCHES.REGS[6] = Low16bits(CURRENT_LATCHES.SSP);
                    break;
                case 2:
                    NEXT_LATCHES.REGS[6] = Low16bits(CURRENT_LATCHES.USP);
                    break;
            }
        }
        else {
            dr_out = GetDRMUX() ? 7 : bits2_0(CURRENT_LATCHES.IR >> 9);
            NEXT_LATCHES.REGS[dr_out] = Low16bits(BUS);
        }
    }
    if (GetLD_PC()) {
        switch (GetPCMUX()) {
            case 0:
                pc_out = Low16bits(CURRENT_LATCHES.PC + 2);
                currentPC = CURRENT_LATCHES.PC;
                break;
            case 1:
                pc_out = BUS;
                break;
            case 2:
                if(GetLSHF1()) {
                    pc_out = addr1_out + (addr2_out << 1);
                }
                else {
                    pc_out = addr1_out + addr2_out;
                }
                break;
            case 3:
                pc_out = Low16bits(CURRENT_LATCHES.PC - 2);
                break;
        }
        NEXT_LATCHES.PC = pc_out;
    }
    if (GetLD_CC()) {
        set_CC();
    }
    if (GetLD_BEN()) {
        set_BEN();
        if (GetLD_E() && unknown_exc) {   /* unknown opcode exception */
            printf("UNKNOWN OP - State: %d\n", CURRENT_LATCHES.STATE_NUMBER);
            NEXT_LATCHES.E = 1;
            NEXT_LATCHES.EXCV = 0x04;
        }
    }
    
    if (GetLD_SSP()) {
        NEXT_LATCHES.SSP = Low16bits(BUS);
    }
    
    if (GetLD_USP()) {
        NEXT_LATCHES.USP = Low16bits(BUS);
    }
    
    if (GetLD_PSR()) {
        switch(GetPSRMUX()) {
            case 0:
                NEXT_LATCHES.PSR = Low16bits(BUS);
                break;
            case 1:
                NEXT_LATCHES.PSR = Low16bits(CURRENT_LATCHES.PSR & 0x7FFF);  /* set to privilege to SV mode */
                break;
            case 2:
                NEXT_LATCHES.PSR = (CURRENT_LATCHES.PSR & 0xFFF8) + (NEXT_LATCHES.N << 2) + (NEXT_LATCHES.Z << 1) + (NEXT_LATCHES.P);
                break;
        }
    }
    
    if (GetLD_VA()) {
        NEXT_LATCHES.VA = Low16bits(CURRENT_LATCHES.MAR);
    }
    
    if (GetLD_R()) {    /* R=1, M = MOD[IR[15:10]] */
        NEXT_LATCHES.MDR = Low16bits(CURRENT_LATCHES.MDR | 1);
        if ((get_opcode() == 3 || get_opcode() == 7) && (CURRENT_LATCHES.TEMP != 33)) {
            NEXT_LATCHES.MDR = Low16bits(CURRENT_LATCHES.MDR | 0x03);
        }
    }
    
    /*
     if (CYCLE_COUNT == 300 && NEXT_LATCHES.EXC != 1) {
     NEXT_LATCHES.INT = 1;
     NEXT_LATCHES.INTV = 0x01;
     printf("Interrupt triggered - State: %d\n", CURRENT_LATCHES.STATE_NUMBER);
     }
     */
    
    
    if (CURRENT_LATCHES.STATE_NUMBER == 47) {
        printf("Entering service routine ... \n");
        printf("Cycle Count: %d\n", CYCLE_COUNT);
        /*        printf("Cycle Count: %d\n", CYCLE_COUNT);
         printf("Current PC: 0x%x\n", CURRENT_LATCHES.PC);
         printf("R0: %d (COUNTER)\n", CURRENT_LATCHES.REGS[0]);
         printf("R1: 0x%x (SUM)\n", CURRENT_LATCHES.REGS[1]);
         printf("R2: 0x%x (MEMORY ADDRESS)\n", CURRENT_LATCHES.REGS[2]);
         printf("R3: 0x%x (MEM[R2])\n\n", CURRENT_LATCHES.REGS[3]);
         */
    }
/*
    if (CURRENT_LATCHES.STATE_NUMBER == 59) {
        printf("Done translating, next state: %d\n", CURRENT_LATCHES.TEMP);
        printf("Cycle Count: %d\n", CYCLE_COUNT);
    }
*/    
    if (NEXT_LATCHES.STATE_NUMBER == 8) {
        printf("RTI next ... \n");
    }
    
    if (CURRENT_LATCHES.STATE_NUMBER == 50) {
        printf("Leaving service routine ... \n");
        printf("Cycle Count: %d\n", CYCLE_COUNT);
    }
    
    if (GetRESET_I()) {
        NEXT_LATCHES.I = 0;
    }
    
    if (GetRESET_E()) {
        NEXT_LATCHES.E = 0;
    }
    
    
}
