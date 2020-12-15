/*
 Name 1: Nolan Corcoran
 UTEID 1: ndc466
 */

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
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

#define WORDS_IN_MEM    0x08000
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
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int Cycle_Count;

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
    
    Cycle_Count++;
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
    printf("Cycle Count  : %d\n", Cycle_Count);
    printf("PC           : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : %d\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%.4x\n", BUS);
    printf("MDR          : 0x%.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");
    
    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", Cycle_Count);
    fprintf(dumpsim_file, "PC           : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%.4x\n", CURRENT_LATCHES.MAR);
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

/************************************************************/
/*                                                          */
/* Procedure : init_memory                                  */
/*                                                          */
/* Purpose   : Zero out the memory array                    */
/*                                                          */
/************************************************************/
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
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;
    
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
    
    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) {
    int i;
    init_control_store(ucode_filename);
    
    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    
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

/*
    if (argc < 3) {
        printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    
    printf("LC-3b Simulator\n\n");
    
    initialize(argv[1], argv[2], argc - 2);
*/

    char* arg1 = "ucode3";
    char* arg2 = "add3.hex";
    initialize(arg1, arg2, 1);

    
    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }
    
    while (1)
        get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
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
#define bits5_4(x) ((x & 0x0030) >> 4)
#define bits8_6(x) ((x & 0x01C0) >> 6)
#define bits11_9(x) ((x & 0x0E00) >> 9)
#define bits15_8(x) ((x * 0xFF00) >> 8)

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


int ready, branch, addr_mode,
    next_state, mdr_data,
    sr1_out, sr2_out, dr_out, addr1_out, addr2_out, pc_out,
    mar_out, alu_out, shf_out, mdr_out,
    dr, sr, srVal, shift, immediate, i;

int cycle_count = 1;

int j_state() {
    ready = (!GetCOND1() & GetCOND0() & GetR()) << 1;       /*  */
    branch = (GetCOND1() & !GetCOND0() & GetBEN()) << 2;    /*  */
    addr_mode = (GetCOND1() & GetCOND0() & GetIR_11());     /*  */
    return (ready | branch | addr_mode | GetJ());           /*  */
}

int get_opcode() {
    return (CURRENT_LATCHES.IR & 0xF000) >> 12;
}

void latch_instruction(int state) {
    NEXT_LATCHES.STATE_NUMBER = state;
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[state], sizeof(int)*CONTROL_STORE_BITS);
}

void set_CC() {
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
    } else {     /* zero */
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.P = 0;
        NEXT_LATCHES.Z = 1;
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

    if (CURRENT_LATCHES.STATE_NUMBER == 2) {
        printf("At LDB instruction\n");
    }
    
    if (CURRENT_LATCHES.STATE_NUMBER == 1) {
        printf("At ADD instruction\n");
    }
    
    /* Check IRD bit to determine whether to use the opcode or J bits */
    if (!GetIRD()) {        /* use J bits */
        next_state = j_state();
    }
    else {                  /* use opcode bits */
        next_state = get_opcode();
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
        
        if (cycle_count == 5) { /* finished, start over */
            cycle_count = 1;
            NEXT_LATCHES.READY = 0;
        }
        else {
            if (cycle_count == 4){      /* ready to read or write */
            
                if (GetR_W()) {
                    if (CURRENT_LATCHES.MAR & 1) {  /* unaligned write */
                    
                        if (GetDATA_SIZE()) {       /* word */
                            exit(0);                /* error: unaligned word access */
                        } else {                    /* high byte */
                            MEMORY[CURRENT_LATCHES.MAR>>1][1] = (CURRENT_LATCHES.MDR & 0xFF00) >> 8;
                        }
                    
                    } else {                        /* aligned write */
                        if (GetDATA_SIZE()) {       /* word */
                            MEMORY[CURRENT_LATCHES.MAR>>1][1] = (CURRENT_LATCHES.MDR & 0xFF00) >> 8;
                            MEMORY[CURRENT_LATCHES.MAR>>1][0] = CURRENT_LATCHES.MDR & 0x00FF;
                        } else {                    /* low byte */
                            MEMORY[CURRENT_LATCHES.MAR>>1][0] = CURRENT_LATCHES.MDR & 0x00FF;
                        }
                    }
                
                } else {
                    if (CURRENT_LATCHES.MAR & 1) {  /* unaligned read */
                        if (GetDATA_SIZE()) {       /* word */
                            exit(0);                /* error: unaligned word access */
                        } else {                    /* high byte */
                            mdr_data = Low16bits((MEMORY[CURRENT_LATCHES.MAR >> 1][1] & 0x00FF) << 8);
                        }
                    } else {                        /* MDR takes word no matter what */
                        mdr_data = Low16bits((MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) & 0xFF00) +
                                            ((MEMORY[CURRENT_LATCHES.MAR >> 1][0]) & 0x00FF);
                    }
                }
                NEXT_LATCHES.READY = 1;
            }
            cycle_count += 1;
        }
    }
    
}



void eval_bus_drivers() {
    
    /* 
     * Datapath routine emulating operations before driving the bus.
     * Evaluate the input of tristate drivers 
     *       Gate_MARMUX,
     *		 Gate_PC,
     *		 Gate_ALU,
     *		 Gate_SHF,
     *		 Gate_MDR.
     */

    
    /*********************
     *                   *
     *    Gate_MARMUX    *
     *                   *
     *********************/
    sr1_out = GetSR1MUX() ? CURRENT_LATCHES.REGS[bits8_6(CURRENT_LATCHES.IR)] : CURRENT_LATCHES.REGS[bits11_9(CURRENT_LATCHES.IR)];
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
        if (GetDATA_SIZE()) { exit(0); }
        else {
            mdr_out = Low16bits(SEXT_8(bits7_0(CURRENT_LATCHES.MDR >> 8)));
        }
    } else {
        mdr_out = GetDATA_SIZE() ? Low16bits(CURRENT_LATCHES.MDR) : SEXT_8(bits7_0(CURRENT_LATCHES.MDR));
    }
}


void drive_bus() {
    
    /*
     * Datapath routine for driving the bus from one of the 5 possible
     * tristate drivers.
     */
    
    if (GetGATE_PC()) {
        BUS = CURRENT_LATCHES.PC;
    }
    else if (GetGATE_MDR()) {
        BUS = mdr_out;
    }
    else if (GetGATE_ALU()) {
        BUS = alu_out;
    }
    else if (GetGATE_MARMUX()) {
        BUS = mar_out;
    }
    else if (GetGATE_SHF()) {
        BUS = shf_out;
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
    }
    if (GetLD_MDR()) {
        if (GetMIO_EN()) {
            NEXT_LATCHES.MDR = Low16bits(mdr_data);
        } else {
            if (GetDATA_SIZE()) {
                NEXT_LATCHES.MDR = Low16bits(BUS);
            } else { /* MDR separated into different high and low bytes */
                NEXT_LATCHES.MDR = Low16bits(((BUS & 0x00FF) << 8) + (BUS & 0x00FF));
            }
        }
    }
    if (GetLD_IR()) {
        NEXT_LATCHES.IR = Low16bits(BUS);
    }
    if (GetLD_REG()) {
        dr_out = GetDRMUX() ? 7 : bits2_0(CURRENT_LATCHES.IR >> 9);
        NEXT_LATCHES.REGS[dr_out] = Low16bits(BUS);
    }
    if (GetLD_PC()) {
        switch (GetPCMUX()) {
            case 0:
                pc_out = CURRENT_LATCHES.PC + 2;
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
        }
        NEXT_LATCHES.PC = pc_out;
    }
    if (GetLD_CC()) {
        set_CC();
    }
    if (GetLD_BEN()) {
        set_BEN();
    }
    
}
