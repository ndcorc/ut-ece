/*
 Name 1: Your Name
 UTEID 1: Your UTEID
 */

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator - Lab 6                                   */
/*                                                             */
/*   EE 460N -- Spring 2013                                    */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/
void FETCH_stage();
void DE_stage();
void AGEX_stage();
void MEM_stage();
void SR_stage();
/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define TRUE  1
#define FALSE 0

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
/* control signals from the control store */
enum CS_BITS {
    SR1_NEEDED,
    SR2_NEEDED,
    DRMUX,
    
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    LSHF1,
    ADDRESSMUX,
    SR2MUX,
    ALUK1, ALUK0,
    ALU_RESULTMUX,
    
    BR_OP,
    UNCOND_OP,
    TRAP_OP,
    BR_STALL,
    
    DCACHE_EN,
    DCACHE_RW,
    DATA_SIZE,
    
    DR_VALUEMUX1, DR_VALUEMUX0,
    LD_REG,
    LD_CC,
    NUM_CONTROL_STORE_BITS
    
} CS_BITS;


enum AGEX_CS_BITS {
    
    AGEX_ADDR1MUX,
    AGEX_ADDR2MUX1, AGEX_ADDR2MUX0,
    AGEX_LSHF1,
    AGEX_ADDRESSMUX,
    AGEX_SR2MUX,
    AGEX_ALUK1, AGEX_ALUK0,
    AGEX_ALU_RESULTMUX,
    
    AGEX_BR_OP,
    AGEX_UNCOND_OP,
    AGEX_TRAP_OP,
    AGEX_BR_STALL,
    AGEX_DCACHE_EN,
    AGEX_DCACHE_RW,
    AGEX_DATA_SIZE,
    
    AGEX_DR_VALUEMUX1, AGEX_DR_VALUEMUX0,
    AGEX_LD_REG,
    AGEX_LD_CC,
    NUM_AGEX_CS_BITS
} AGEX_CS_BITS;

enum MEM_CS_BITS {
    MEM_BR_OP,
    MEM_UNCOND_OP,
    MEM_TRAP_OP,
    MEM_BR_STALL,
    MEM_DCACHE_EN,
    MEM_DCACHE_RW,
    MEM_DATA_SIZE,
    
    MEM_DR_VALUEMUX1, MEM_DR_VALUEMUX0,
    MEM_LD_REG,
    MEM_LD_CC,
    NUM_MEM_CS_BITS
} MEM_CS_BITS;

enum SR_CS_BITS {
    SR_DR_VALUEMUX1, SR_DR_VALUEMUX0,
    SR_LD_REG,
    SR_LD_CC,
    NUM_SR_CS_BITS
} SR_CS_BITS;


/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int Get_SR1_NEEDED(int *x)     { return (x[SR1_NEEDED]); }
int Get_SR2_NEEDED(int *x)     { return (x[SR2_NEEDED]); }
int Get_DRMUX(int *x)          { return (x[DRMUX]);}
int Get_DE_BR_OP(int *x)       { return (x[BR_OP]); }
int Get_ADDR1MUX(int *x)       { return (x[AGEX_ADDR1MUX]); }
int Get_ADDR2MUX(int *x)       { return ((x[AGEX_ADDR2MUX1] << 1) + x[AGEX_ADDR2MUX0]); }
int Get_LSHF1(int *x)          { return (x[AGEX_LSHF1]); }
int Get_ADDRESSMUX(int *x)     { return (x[AGEX_ADDRESSMUX]); }
int Get_SR2MUX(int *x)          { return (x[AGEX_SR2MUX]); }
int Get_ALUK(int *x)           { return ((x[AGEX_ALUK1] << 1) + x[AGEX_ALUK0]); }
int Get_ALU_RESULTMUX(int *x)  { return (x[AGEX_ALU_RESULTMUX]); }
int Get_BR_OP(int *x)          { return (x[MEM_BR_OP]); }
int Get_UNCOND_OP(int *x)      { return (x[MEM_UNCOND_OP]); }
int Get_TRAP_OP(int *x)        { return (x[MEM_TRAP_OP]); }
int Get_DCACHE_EN(int *x)      { return (x[MEM_DCACHE_EN]); }
int Get_DCACHE_RW(int *x)      { return (x[MEM_DCACHE_RW]); }
int Get_DATA_SIZE(int *x)      { return (x[MEM_DATA_SIZE]); }
int Get_DR_VALUEMUX1(int *x)   { return ((x[SR_DR_VALUEMUX1] << 1 ) + x[SR_DR_VALUEMUX0]); }
int Get_AGEX_LD_REG(int *x)    { return (x[AGEX_LD_REG]); }
int Get_AGEX_LD_CC(int *x)     { return (x[AGEX_LD_CC]); }
int Get_MEM_LD_REG(int *x)     { return (x[MEM_LD_REG]); }
int Get_MEM_LD_CC(int *x)      { return (x[MEM_LD_CC]); }
int Get_SR_LD_REG(int *x)      { return (x[SR_LD_REG]); }
int Get_SR_LD_CC(int *x)       { return (x[SR_LD_CC]); }
int Get_DE_BR_STALL(int *x)    { return (x[BR_STALL]); }
int Get_AGEX_BR_STALL(int *x)  { return (x[AGEX_BR_STALL]); }
int Get_MEM_BR_STALL(int *x)   { return (x[MEM_BR_STALL]); }



/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][NUM_CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
 MEMORY[A][1] stores the most significant byte of word at word address A
 There are two write enable signals, one for each byte. WE0 is used for
 the least significant byte of a word. WE1 is used for the most significant
 byte of a word. */

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/
/* The LC-3b register file.                                      */
/***************************************************************/
#define LC3b_REGS 8
int REGS[LC3b_REGS];
/***************************************************************/
/* architectural state */
/***************************************************************/
int  PC,  	/* program counter */
N,		/* n condition bit */
Z = 1,	/* z condition bit */
P;		/* p condition bit */
/***************************************************************/
/* LC-3b State info.                                             */
/***************************************************************/

typedef struct PipeState_Entry_Struct{
    
    /* DE latches */
    int DE_NPC,
    DE_IR,
    DE_V,
    /* AGEX lateches */
    AGEX_NPC,
    AGEX_SR1,
    AGEX_SR2,
    AGEX_CC,
    AGEX_IR,
    AGEX_DRID,
    AGEX_V,
    AGEX_CS[NUM_AGEX_CS_BITS],
    /* MEM latches */
    MEM_NPC,
    MEM_ALU_RESULT,
    MEM_ADDRESS,
    MEM_CC,
    MEM_IR,
    MEM_DRID,
    MEM_V,
    MEM_CS[NUM_MEM_CS_BITS],
    /* SR latches */
    SR_NPC,
    SR_DATA,
    SR_ALU_RESULT,
    SR_ADDRESS,
    SR_IR,
    SR_DRID,
    SR_V,
    SR_CS[NUM_SR_CS_BITS];
    
} PipeState_Entry;

/* data structure for latch */
PipeState_Entry PS, NEW_PS;

/* simulator signal */
int RUN_BIT;

/* Internal stall signals */
int   dep_stall,
v_de_br_stall,
v_agex_br_stall,
v_mem_br_stall,
mem_stall,
icache_r;

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
    printf("rdump            -  dump the architectural state    \n");
    printf("idump            -  dump the internal state         \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

void print_CS(int *CS, int num)
{
    int ii ;
    for ( ii = 0 ; ii < num; ii++) {
        printf("%d",CS[ii]);
    }
    printf("\n");
}
/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {
    NEW_PS = PS;
    SR_stage();
    MEM_stage();
    AGEX_stage();
    DE_stage();
    FETCH_stage();
    PS = NEW_PS;
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
        if (PC == 0x0000) {
            cycle();
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
    if ((RUN_BIT == FALSE) || (PC == 0x0000)) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }
    printf("Simulating...\n\n");
    /* initialization */
    while (PC != 0x0000)
        cycle();
    cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a region of memory to the output file.     */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */
    
    printf("\nMemory content [0x%04x..0x%04x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");
    
    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%04x..0x%04x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%04x (%d) : 0x%02x%02x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current architectural state  to the       */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;
    
    printf("\nCurrent architectural state :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count : %d\n", CYCLE_COUNT);
    printf("PC          : 0x%04x\n", PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    printf("Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
        printf("%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    printf("\n");
    
    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent architectural state :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC          : 0x%04x\n", PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : idump                                           */
/*                                                             */
/* Purpose   : Dump current internal state to the              */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void idump(FILE * dumpsim_file) {
    int k;
    
    printf("\nCurrent architectural state :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count     : %d\n", CYCLE_COUNT);
    printf("PC              : 0x%04x\n", PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    printf("Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
        printf("%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    printf("\n");
    
    printf("------------- Stall Signals -------------\n");
    printf("ICACHE_R        :  %d\n", icache_r);
    printf("DEP_STALL       :  %d\n", dep_stall);
    printf("V_DE_BR_STALL   :  %d\n", v_de_br_stall);
    printf("V_AGEX_BR_STALL :  %d\n", v_agex_br_stall);
    printf("MEM_STALL       :  %d\n", mem_stall);
    printf("V_MEM_BR_STALL  :  %d\n", v_mem_br_stall);
    printf("\n");
    
    printf("------------- DE   Latches --------------\n");
    printf("DE_NPC          :  0x%04x\n", PS.DE_NPC );
    printf("DE_IR           :  0x%04x\n", PS.DE_IR );
    printf("DE_V            :  %d\n", PS.DE_V);
    printf("\n");
    
    printf("------------- AGEX Latches --------------\n");
    printf("AGEX_NPC        :  0x%04x\n", PS.AGEX_NPC );
    printf("AGEX_SR1        :  0x%04x\n", PS.AGEX_SR1 );
    printf("AGEX_SR2        :  0x%04x\n", PS.AGEX_SR2 );
    printf("AGEX_CC         :  %d\n", PS.AGEX_CC );
    printf("AGEX_IR         :  0x%04x\n", PS.AGEX_IR );
    printf("AGEX_DRID       :  %d\n", PS.AGEX_DRID);
    printf("AGEX_CS         :  ");
    for ( k = 0 ; k < NUM_AGEX_CS_BITS; k++) {
        printf("%d",PS.AGEX_CS[k]);
    }
    printf("\n");
    printf("AGEX_V          :  %d\n", PS.AGEX_V);
    printf("\n");
    
    printf("------------- MEM  Latches --------------\n");
    printf("MEM_NPC         :  0x%04x\n", PS.MEM_NPC );
    printf("MEM_ALU_RESULT  :  0x%04x\n", PS.MEM_ALU_RESULT );
    printf("MEM_ADDRESS     :  0x%04x\n", PS.MEM_ADDRESS );
    printf("MEM_CC          :  %d\n", PS.MEM_CC );
    printf("MEM_IR          :  0x%04x\n", PS.MEM_IR );
    printf("MEM_DRID        :  %d\n", PS.MEM_DRID);
    printf("MEM_CS          :  ");
    for ( k = 0 ; k < NUM_MEM_CS_BITS; k++) {
        printf("%d",PS.MEM_CS[k]);
    }
    printf("\n");
    printf("MEM_V           :  %d\n", PS.MEM_V);
    printf("\n");
    
    printf("------------- SR   Latches --------------\n");
    printf("SR_NPC          :  0x%04x\n", PS.SR_NPC );
    printf("SR_DATA         :  0x%04x\n", PS.SR_DATA );
    printf("SR_ALU_RESULT   :  0x%04x\n", PS.SR_ALU_RESULT );
    printf("SR_ADDRESS      :  0x%04x\n", PS.SR_ADDRESS );
    printf("SR_IR           :  0x%04x\n", PS.SR_IR );
    printf("SR_DRID         :  %d\n", PS.SR_DRID);
    printf("SR_CS           :  ");
    for ( k = 0 ; k < NUM_SR_CS_BITS; k++) {
        printf("%d",PS.SR_CS[k]);
    }
    printf("\n");
    printf("SR_V            :  %d\n", PS.SR_V);
    
    printf("\n");
    
    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file,"\nCurrent architectural state :\n");
    fprintf(dumpsim_file,"-------------------------------------\n");
    fprintf(dumpsim_file,"Cycle Count     : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file,"PC              : 0x%04x\n", PC);
    fprintf(dumpsim_file,"CCs: N = %d  Z = %d  P = %d\n", N, Z, P);
    fprintf(dumpsim_file,"Registers:\n");
    for (k = 0; k < LC3b_REGS; k++)
        fprintf(dumpsim_file,"%d: 0x%04x\n", k, (REGS[k] & 0xFFFF));
    fprintf(dumpsim_file,"\n");
    
    fprintf(dumpsim_file,"------------- Stall Signals -------------\n");
    fprintf(dumpsim_file,"ICACHE_R        :  %d\n", icache_r);
    fprintf(dumpsim_file,"DEP_STALL       :  %d\n", dep_stall);
    fprintf(dumpsim_file,"V_DE_BR_STALL   :  %d\n", v_de_br_stall);
    fprintf(dumpsim_file,"V_AGEX_BR_STALL :  %d\n", v_agex_br_stall);
    fprintf(dumpsim_file,"MEM_STALL       :  %d\n", mem_stall);
    fprintf(dumpsim_file,"V_MEM_BR_STALL  :  %d\n", v_mem_br_stall);
    fprintf(dumpsim_file,"\n");
    
    fprintf(dumpsim_file,"------------- DE   Latches --------------\n");
    fprintf(dumpsim_file,"DE_NPC          :  0x%04x\n", PS.DE_NPC );
    fprintf(dumpsim_file,"DE_IR           :  0x%04x\n", PS.DE_IR );
    fprintf(dumpsim_file,"DE_V            :  %d\n", PS.DE_V);
    fprintf(dumpsim_file,"\n");
    
    fprintf(dumpsim_file,"------------- AGEX Latches --------------\n");
    fprintf(dumpsim_file,"AGEX_NPC        :  0x%04x\n", PS.AGEX_NPC );
    fprintf(dumpsim_file,"AGEX_SR1        :  0x%04x\n", PS.AGEX_SR1 );
    fprintf(dumpsim_file,"AGEX_SR2        :  0x%04x\n", PS.AGEX_SR2 );
    fprintf(dumpsim_file,"AGEX_CC         :  %d\n", PS.AGEX_CC );
    fprintf(dumpsim_file,"AGEX_IR         :  0x%04x\n", PS.AGEX_IR );
    fprintf(dumpsim_file,"AGEX_DRID       :  %d\n", PS.AGEX_DRID);
    fprintf(dumpsim_file,"AGEX_CS         :  ");
    for ( k = 0 ; k < NUM_AGEX_CS_BITS; k++) {
        fprintf(dumpsim_file,"%d",PS.AGEX_CS[k]);
    }
    fprintf(dumpsim_file,"\n");
    fprintf(dumpsim_file,"AGEX_V          :  %d\n", PS.AGEX_V);
    fprintf(dumpsim_file,"\n");
    
    fprintf(dumpsim_file,"------------- MEM  Latches --------------\n");
    fprintf(dumpsim_file,"MEM_NPC         :  0x%04x\n", PS.MEM_NPC );
    fprintf(dumpsim_file,"MEM_ALU_RESULT  :  0x%04x\n", PS.MEM_ALU_RESULT );
    fprintf(dumpsim_file,"MEM_ADDRESS     :  0x%04x\n", PS.MEM_ADDRESS );
    fprintf(dumpsim_file,"MEM_CC          :  %d\n", PS.MEM_CC );
    fprintf(dumpsim_file,"MEM_IR          :  0x%04x\n", PS.MEM_IR );
    fprintf(dumpsim_file,"MEM_DRID        :  %d\n", PS.MEM_DRID);
    fprintf(dumpsim_file,"MEM_CS          :  ");
    for ( k = 0 ; k < NUM_MEM_CS_BITS; k++) {
        fprintf(dumpsim_file,"%d",PS.MEM_CS[k]);
    }
    fprintf(dumpsim_file,"\n");
    fprintf(dumpsim_file,"MEM_V           :  %d\n", PS.MEM_V);
    fprintf(dumpsim_file,"\n");
    
    fprintf(dumpsim_file,"------------- SR   Latches --------------\n");
    fprintf(dumpsim_file,"SR_NPC          :  0x%04x\n", PS.SR_NPC );
    fprintf(dumpsim_file,"SR_DATA         :  0x%04x\n",PS.SR_DATA );
    fprintf(dumpsim_file,"SR_ALU_RESULT   :  0x%04x\n", PS.SR_ALU_RESULT );
    fprintf(dumpsim_file,"SR_ADDRESS      :  0x%04x\n", PS.SR_ADDRESS );
    fprintf(dumpsim_file,"SR_IR           :  0x%04x\n", PS.SR_IR );
    fprintf(dumpsim_file,"SR_DRID         :  %d\n", PS.SR_DRID);
    fprintf(dumpsim_file,"SR_CS           :  ");
    for ( k = 0 ; k < NUM_SR_CS_BITS; k++) {
        fprintf(dumpsim_file, "%d",PS.SR_CS[k]);
    }
    fprintf(dumpsim_file,"\n");
    fprintf(dumpsim_file,"SR_V            :  %d\n", PS.SR_V);
    
    fprintf(dumpsim_file,"\n");
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
            
        case 'I':
        case 'i':
            idump(dumpsim_file);
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
        
        for (j = 0; j < NUM_CONTROL_STORE_BITS; j++) {
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


/***************************************************************/
/*                                                             */
/* Procedure : init_state                                      */
/*                                                             */
/* Purpose   : Zero out all latches and registers              */
/*                                                             */
/***************************************************************/
void init_state() {
    
    memset(&PS, 0 ,sizeof(PipeState_Entry));
    memset(&NEW_PS, 0 , sizeof(PipeState_Entry));
    
    dep_stall       = 0;
    v_de_br_stall   = 0;
    v_agex_br_stall = 0;
    v_mem_br_stall  = 0;
    mem_stall       = 0;
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
        program_base = word >> 1 ;
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
    
    if (PC == 0) PC  = program_base << 1 ;
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
    init_state();
    
    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* dcache_access                                               */
/*                                                             */
/***************************************************************/
void dcache_access(int dcache_addr, int *read_word, int write_word, int *dcache_r,
                   int mem_w0, int mem_w1) {
    
    int addr = dcache_addr >> 1 ;
    int random = CYCLE_COUNT % 9;
    
    if (!random) {
        *dcache_r = 0;
        *read_word = 0xfeed ;
    }
    else {
        *dcache_r = 1;
        
        *read_word = (MEMORY[addr][1] << 8) | (MEMORY[addr][0] & 0x00FF);
        if(mem_w0) MEMORY[addr][0] = write_word & 0x00FF;
        if(mem_w1) MEMORY[addr][1] = (write_word & 0xFF00) >> 8;
    }
}
/***************************************************************/
/*                                                             */
/* icache_access                                               */
/*                                                             */
/***************************************************************/
void icache_access(int icache_addr, int *read_word, int *icache_r) {
    
    int addr = icache_addr >> 1 ; 
    int random = CYCLE_COUNT % 13;
    
    if (!random) {
        *icache_r = 0;
        *read_word = 0xfeed;
    }
    else {
        *icache_r = 1;
        *read_word = MEMORY[addr][1] << 8 | MEMORY[addr][0];
    }
}
/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;
    
    /* Error Checking */
    if (argc < 3) {
        printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }
    
    printf("LC-3b Simulator\n\n");
    
    initialize(argv[1], argv[2], argc - 2);
    
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
 
 RUN_BIT
 REGS
 MEMORY
 
 PC
 N
 Z
 P
 
 DEP_STALL
 V_DE_BR_STALL
 V_AGEX_BR_STALL
 V_MEM_BR_STALL
 MEM_STALL
 ICACHE_R
 
 PS
 NEW_PS
 
 
 You may define your own local/global variables and functions.
 You may use the functions to get at the control bits defined
 above.
 
 
 Begin your code here 	  			       */
/***************************************************************/
#define COPY_AGEX_CS_START 3
#define COPY_MEM_CS_START 9
#define COPY_SR_CS_START  7

#define bit0(x)     (x & 0x0001)
#define bit1(x)     ((x & 0x0002) >> 1)
#define bit2(x)     ((x & 0x0004) >> 2)
#define bit5(x)     ((x & 0x0020) >> 5)
#define bit9(x)     ((x & 0x0200) >> 9)
#define bit10(x)    ((x & 0x0400) >> 10)
#define bit11(x)    ((x & 0x0800) >> 11)
#define bit13(x)    ((x & 0x2000) >> 13)
#define bit15(x)    ((x & 0x8000) >> 15)

#define bits2_0(x) (x & 0x0007)
#define bits3_0(x) (x & 0x000F)
#define bits4_0(x) (x & 0x001F)
#define bits5_0(x) (x & 0x003F)
#define bits7_0(x) (x & 0x00FF)
#define bits8_0(x) (x & 0x01FF)
#define bits10_0(x) (x & 0x07FF)

#define bits5_4(x) ((x & 0x0030) >> 4)
#define bits8_6(x) ((x & 0x01C0) >> 6)
#define bits15_8(x) ((x & 0xFF00) >> 8)
#define bits11_9(x) ((x & 0x0E00) >> 9)
#define bits15_11(x) ((x & 0xF800) >> 11)
#define bits15_12(x) ((x & 0xF000) >> 12)

#define pbit(x) ((x & 0x0008) >> 3)
#define vbit(x) ((x & 0x0004) >> 2)
#define mbit(x) ((x & 0x0002) >> 1)
#define rbit(x) (x & 0x0001)

#define vpn(x) ((x & 0xFE00) >> 9)
#define offset(x) (x & 0x01FF)
#define pfn(x) ((x & 0x3E00) >> 9)

/* Signals generated by SR stage and needed by previous stages in the
 pipeline are declared below. */
int SR_REG_DATA,
    SR_N, SR_Z, SR_P,
    V_SR_LD_CC,
    V_SR_LD_REG,
    SR_REG_ID;

/* Signals generated by MEM stage and needed by previous stages in the
 pipeline are declared below. */
int V_MEM_LD_CC, V_MEM_LD_REG;
int DCACHE_R;
int WE0, WE1;
int DATA_IN, DATA_OUT;
int V_DCACHE_EN;
int TRAP_PC, TARGET_PC, MEM_PCMUX, MEM_DRID;

/* Signals generated by AGEX stage and needed by previous stages in the
 pipeline are declared below. */
int ADDR1MUX_OUT, ADDR2MUX_OUT, LSHF1_OUT, ADDRESSMUX_OUT;
int SR2MUX_OUT, ALU_OUT, SHIFTER_OUT, ALU_RESULTMUX_OUT;
int V_AGEX_LD_CC, V_AGEX_LD_REG;
int AGEX_DRID, LD_MEM;

/* Signals generated by DE stage and needed by previous stages in the
 pipeline are declared below. */
int SR1_N, SR2_N;
int SR1_ID, SR2_ID;
int CC_OUT, DRMUX_OUT, V_OUT;

/* Signals generated by DE stage and needed by previous stages in the
 pipeline are declared below. */
int READ_WORD, V_OUT, LD_DE;
int PC_OUT, PCMUX_OUT, LD_PC;

int br_cc_logic() {
    if (((Get_BR_OP(PS.MEM_CS)) &&
         ((bit11(PS.MEM_IR) && bit2(PS.MEM_CC)) ||
          (bit10(PS.MEM_IR) && bit1(PS.MEM_CC)) ||
          (bit9(PS.MEM_IR) && bit0(PS.MEM_CC))) )) {
        return 1;
    }
    return 0;
}

int inputs_written(int SRID) {
    return ((V_SR_LD_REG && SR_REG_ID == SRID) || (V_MEM_LD_REG && MEM_DRID == SRID) || (V_AGEX_LD_REG && AGEX_DRID == SRID)) ? 1 : 0;
}

int SEXT32(int val) {

    if (val & 0x8000) val |= 0xFFFF0000;
    return val;
}

int SEXT(int val, int msb) {
    switch(msb) {
        case 4:
            return (val & 0x0010) ? (val |= 0xFFFFFFE0) : val;
            break;
        case 5:
            return (val & 0x0020) ? (val |= 0xFFFFFFC0) : val;
            break;
        case 7:
            return (val & 0x0080) ? (val |= 0xFFFFFF00) : val;
            break;
        case 8:
            return (val & 0x0100) ? (val |= 0xFFFFFE00) : val;
            break;
        case 10:
            return (val & 0x0400) ? (val |= 0xFFFFF800) : val;
            break;
    }
    return val;
}

char* get_instr(int store) {
    switch(store) {
        case 0:
        case 1:
        case 2:
        case 3:
            return "BR";
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            return "ADD";
            break;
        case 8:
        case 9:
        case 10:
        case 11:
            return "LDB";
            break;
        case 20:
        case 21:
        case 22:
        case 23:
            return "AND";
            break;
        case 24:
        case 25:
        case 26:
        case 27:
            return "LDW";
            break;
        case 28:
        case 29:
        case 30:
        case 31:
            return "STW";
            break;
        case 56:
        case 57:
        case 58:
        case 59:
            return "LEA";
            break;
        case 60:
        case 61:
        case 62:
        case 63:
            return "TRAP";
            break;
    }
    return "N/A";
}

/************************* SR_stage() *************************/
void SR_stage() {
    
    /* You are given the code for SR_stage to get you started. Look at
     the figure for SR stage to see how this code is implemented. */
    
    switch (Get_DR_VALUEMUX1(PS.SR_CS))
    {
        case 0:
            SR_REG_DATA = PS.SR_ADDRESS ;
            break;
        case 1:
            SR_REG_DATA = PS.SR_DATA ;
            break;
        case 2:
            SR_REG_DATA = PS.SR_NPC ;
            break;
        case 3:
            SR_REG_DATA = PS.SR_ALU_RESULT ;
            break;
    }
    
    SR_REG_ID = PS.SR_DRID;
    V_SR_LD_REG = Get_SR_LD_REG(PS.SR_CS) & PS.SR_V;
    V_SR_LD_CC = Get_SR_LD_CC(PS.SR_CS) & PS.SR_V ;
    
    /* CC LOGIC  */
    SR_N = ((SR_REG_DATA & 0x8000) ? 1 : 0);
    SR_Z = ((SR_REG_DATA & 0xFFFF) ? 0 : 1);
    SR_P = 0;
    if ((!SR_N) && (!SR_Z)) SR_P = 1;
    
}


/************************* MEM_stage() *************************/
void MEM_stage() {
    
    int ii,jj = 0;
    
    
    /* your code for MEM stage goes here */
    
    /* V.DCACHE.EN */
    V_DCACHE_EN = Get_DCACHE_EN(PS.MEM_CS) && PS.MEM_V;      /* POSSIBLE ERROR!!! */
    
    /* DATA_IN LOGIC */
    if (Get_DATA_SIZE(PS.MEM_CS)) {                     /* WORD */
        DATA_IN = PS.MEM_ALU_RESULT;
    } else {                                            /* BYTE */
        DATA_IN = bit0(PS.MEM_ADDRESS) ? bits15_8(PS.MEM_ALU_RESULT) << 8 : bits7_0(PS.MEM_ALU_RESULT);
    }
    
    /* BR LOGIC */
    if (PS.MEM_V) {
        if (Get_TRAP_OP(PS.MEM_CS)) {
            MEM_PCMUX = 2;
        } else if (Get_UNCOND_OP(PS.MEM_CS)) {
            MEM_PCMUX = 1;
        } else if (br_cc_logic()) {
            MEM_PCMUX = 1;
        } else {
            MEM_PCMUX = 0;
        }
    } else {
        MEM_PCMUX = 0;
    }

    /* WE LOGIC */
    if (Get_DCACHE_RW(PS.MEM_CS)) {                 /* WRITE */
        if (Get_DATA_SIZE(PS.MEM_CS)) {             /* WORD */
            WE0 = bit0(PS.MEM_ADDRESS) ? 0 : 1;
            WE1 = bit0(PS.MEM_ADDRESS) ? 0 : 1;
        } else {                                    /* BYTE */
            WE0 = bit0(PS.MEM_ADDRESS) ? 0 : 1;
            WE1 = bit0(PS.MEM_ADDRESS) ? 1 : 0;
        }
    } else {                                        /* READ */
        WE0 = 0;
        WE1 = 0;
    }
    
    /* D-CACHE LOGIC */
    if (V_DCACHE_EN) {
        dcache_access(PS.MEM_ADDRESS, &DATA_OUT, DATA_IN, &DCACHE_R, WE0, WE1);
    } else {
        DATA_OUT = 0;
    }
    
    /* TRAP_PC, TARGET.PC, MEM.STALL, MEM.DRID */
    TRAP_PC = Get_DATA_SIZE(PS.MEM_CS) ? DATA_OUT : (bit0(PS.MEM_ADDRESS) ? bits15_8(DATA_OUT) : bits7_0(DATA_OUT));    /* POSSIBLE ERROR!!! */
    TARGET_PC = PS.MEM_ADDRESS;
    mem_stall = !DCACHE_R && V_DCACHE_EN;
    MEM_DRID = PS.MEM_DRID;
    
    /* DEPENDENCY CHECK SIGNALS */
    V_MEM_LD_CC = (PS.MEM_V) & (Get_MEM_LD_CC(PS.MEM_CS));
    V_MEM_LD_REG = (PS.MEM_V) & (Get_MEM_LD_REG(PS.MEM_CS));
    v_mem_br_stall = (PS.MEM_V) & (Get_MEM_BR_STALL(PS.MEM_CS));
    
    NEW_PS.SR_V = (!PS.MEM_V || mem_stall) ? 0 : 1;     /* SR.V */
    NEW_PS.SR_DATA = TRAP_PC;
    NEW_PS.SR_ADDRESS = PS.MEM_ADDRESS;
    NEW_PS.SR_NPC = PS.MEM_NPC;
    NEW_PS.SR_ALU_RESULT = PS.MEM_ALU_RESULT;
    NEW_PS.SR_IR = PS.MEM_IR;
    NEW_PS.SR_DRID = PS.MEM_DRID;
    
    /* The code below propagates the control signals from MEM.CS latch
     to SR.CS latch. You still need to latch other values into the
     other SR latches. */
    for (ii=COPY_SR_CS_START; ii < NUM_MEM_CS_BITS; ii++) {
        NEW_PS.SR_CS [jj++] = PS.MEM_CS [ii];
    }
    
}


/************************* AGEX_stage() *************************/
void AGEX_stage() {
    
    int ii, jj = 0;
    
    /* your code for AGEX stage goes here */
    
    /* MEM.ADDRESS */
    ADDR1MUX_OUT = Get_ADDR1MUX(PS.AGEX_CS) ? PS.AGEX_SR1 : PS.AGEX_NPC;
    switch (Get_ADDR2MUX(PS.AGEX_CS)) {
        case 0: ADDR2MUX_OUT = 0; break;
        case 1: ADDR2MUX_OUT = SEXT(bits5_0(PS.AGEX_IR), 5); break;
        case 2: ADDR2MUX_OUT = SEXT(bits8_0(PS.AGEX_IR), 8); break;
        case 3: ADDR2MUX_OUT = SEXT(bits10_0(PS.AGEX_IR), 10); break;
        default: ADDR2MUX_OUT = 0; break;
    }
    LSHF1_OUT = Get_LSHF1(PS.AGEX_CS) ? (ADDR2MUX_OUT << 1) : ADDR2MUX_OUT;
    ADDRESSMUX_OUT = Get_ADDRESSMUX(PS.AGEX_CS) ? (ADDR1MUX_OUT + LSHF1_OUT) : bits7_0(PS.AGEX_IR) << 1;
    
    /* MEM.ALU.RESULT */
    SR2MUX_OUT = Get_SR2MUX(PS.AGEX_CS) ? SEXT(bits4_0(PS.AGEX_IR), 4) : PS.AGEX_SR2;
    switch (Get_ALUK(PS.AGEX_CS)) {
        case 0: ALU_OUT = Low16bits(PS.AGEX_SR1 + SR2MUX_OUT); break;
        case 1: ALU_OUT = Low16bits(PS.AGEX_SR1 & SR2MUX_OUT); break;
        case 2: ALU_OUT = Low16bits(PS.AGEX_SR1 ^ SR2MUX_OUT); break;
        case 3: ALU_OUT = Low16bits(SR2MUX_OUT); break;
        default: ALU_OUT = Low16bits(SR2MUX_OUT); break;
    }
    switch (bits5_4(PS.AGEX_IR)) {
        case 0: SHIFTER_OUT = Low16bits(PS.AGEX_SR1 << bits3_0(PS.AGEX_IR)); break;
        case 1: SHIFTER_OUT = Low16bits(PS.AGEX_SR1 >> bits3_0(PS.AGEX_IR)); break;
        case 2: SHIFTER_OUT = Low16bits(PS.AGEX_SR1 << bits3_0(PS.AGEX_IR)); break;
        case 3: SHIFTER_OUT = Low16bits((0xFFFF0000 >> bits3_0(PS.AGEX_IR))) + (PS.AGEX_SR1 >> bits3_0(PS.AGEX_IR)); break;
        default: SHIFTER_OUT = Low16bits(PS.AGEX_SR1 << bits3_0(PS.AGEX_IR)); break;
    }
    ALU_RESULTMUX_OUT = Get_ALU_RESULTMUX(PS.AGEX_CS) ? ALU_OUT : SHIFTER_OUT;
    
    /* DEPENDENCY CHECK SIGNALS */
    V_AGEX_LD_CC = PS.AGEX_V & Get_AGEX_LD_CC(PS.AGEX_CS);
    V_AGEX_LD_REG = PS.AGEX_V & Get_AGEX_LD_REG(PS.AGEX_CS);
    v_agex_br_stall = PS.AGEX_V & Get_AGEX_BR_STALL(PS.AGEX_CS);
    
    /* AGEX.DRID, LD.MEM */
    AGEX_DRID = PS.AGEX_DRID;
    LD_MEM = !mem_stall;
    
    if (LD_MEM) {
        /* Your code for latching into MEM latches goes here */
        NEW_PS.MEM_ADDRESS = ADDRESSMUX_OUT;
        NEW_PS.MEM_NPC = PS.AGEX_NPC;
        NEW_PS.MEM_CC = PS.AGEX_CC;
        NEW_PS.MEM_ALU_RESULT = ALU_RESULTMUX_OUT;
        NEW_PS.MEM_IR = PS.AGEX_IR;
        NEW_PS.MEM_DRID = PS.AGEX_DRID;
        NEW_PS.MEM_V = PS.AGEX_V;
        
        /* The code below propagates the control signals from AGEX.CS latch
         to MEM.CS latch. */
        for (ii = COPY_MEM_CS_START; ii < NUM_AGEX_CS_BITS; ii++) {
            NEW_PS.MEM_CS [jj++] = PS.AGEX_CS [ii]; 
        }
    }
}



/************************* DE_stage() *************************/
void DE_stage() {
    
    int CONTROL_STORE_ADDRESS;  /* You need to implement the logic to
                                 set the value of this variable. Look
                                 at the figure for DE stage */ 
    int ii, jj = 0;
    int LD_AGEX; /* You need to write code to compute the value of
                  LD.AGEX signal */
    
    /* your code for DE stage goes here */
    
    /* AGEX.SR1, AGEX.SR2 */
    SR1_ID = bits8_6(PS.DE_IR);
    SR2_ID = bit13(PS.DE_IR) ? bits11_9(PS.DE_IR) : bits2_0(PS.DE_IR);
    
    /* AGEX.CC */
    CC_OUT = (N << 2) + (Z << 1) + P;
    
    /* AGEX.DRID */
    CONTROL_STORE_ADDRESS = (bits15_11(PS.DE_IR) << 1) + bit5(PS.DE_IR);
    DRMUX_OUT = Get_DRMUX(CONTROL_STORE[CONTROL_STORE_ADDRESS]) ? 7 : bits11_9(PS.DE_IR);
    
    /* V.DE.BR.STALL */
    v_de_br_stall = (PS.DE_V & Get_DE_BR_STALL(CONTROL_STORE[CONTROL_STORE_ADDRESS]));
    
    /* SR1_NEEDED, SR2_NEEDED */
    SR1_N = Get_SR1_NEEDED(CONTROL_STORE[CONTROL_STORE_ADDRESS]);
    SR2_N = Get_SR2_NEEDED(CONTROL_STORE[CONTROL_STORE_ADDRESS]);
    
    int isBR_OP = Get_DE_BR_OP(CONTROL_STORE[CONTROL_STORE_ADDRESS]);
    if (PS.DE_V) {
        dep_stall = isBR_OP ? (V_SR_LD_CC || V_MEM_LD_CC || V_AGEX_LD_CC) : (SR1_N && inputs_written(SR1_ID)) || (SR2_N && inputs_written(SR2_ID));
    } else {
        dep_stall = 0;
    }

    LD_AGEX = !mem_stall;
    
    if (LD_AGEX) {
        /* Your code for latching into AGEX latches goes here */
        NEW_PS.AGEX_NPC = PS.DE_NPC;
        NEW_PS.AGEX_IR = PS.DE_IR;
        NEW_PS.AGEX_SR1 = REGS[SR1_ID];
        NEW_PS.AGEX_SR2 = REGS[SR2_ID];
        NEW_PS.AGEX_CC = CC_OUT;
        NEW_PS.AGEX_DRID = DRMUX_OUT;
        NEW_PS.AGEX_V = (!PS.DE_V || dep_stall || v_agex_br_stall || v_mem_br_stall) ? 0 : 1;
        
        /* The code below propagates the control signals from the CONTROL
         STORE to the AGEX.CS latch. */
        for (ii = COPY_AGEX_CS_START; ii< NUM_CONTROL_STORE_BITS; ii++) {
            NEW_PS.AGEX_CS[jj++] = CONTROL_STORE[CONTROL_STORE_ADDRESS][ii];
        }
    }
    /* LD.CC */
    if (V_SR_LD_CC) {
        N = SR_N;
        Z = SR_Z;
        P = SR_P;
    }
    /* LD.REG */
    if (V_SR_LD_REG) {
        REGS[SR_REG_ID] = SR_REG_DATA;
    }
    
}



/************************* FETCH_stage() *************************/
void FETCH_stage() {
    
    /* your code for FETCH stage goes here */

    /* DE.IR */
    icache_access(PC, &READ_WORD, &icache_r);
    
    /* DE.NPC */
    PC_OUT = PC + 2;
    
    /* DE.V */
    V_OUT = (!icache_r || v_de_br_stall || v_agex_br_stall || v_mem_br_stall) ? 0 : 1;
    
    /* PCMUX */
    switch (MEM_PCMUX) {
        case 0: PCMUX_OUT = PC_OUT; break;
        case 1: PCMUX_OUT = TARGET_PC; break;
        case 2: PCMUX_OUT = TRAP_PC; break;
        default: PCMUX_OUT = PC_OUT; break;
    }
    
    /* LD.PC */
    if (!MEM_PCMUX && v_mem_br_stall) {
        LD_PC = 0;
    } else {
        if ((!mem_stall && v_mem_br_stall) || (!(dep_stall || !icache_r || mem_stall || v_de_br_stall || v_agex_br_stall))) {
            LD_PC = 1;
            PC = PCMUX_OUT;
        } else {
            LD_PC = 0;
        }
    }
    
    /* LD.DE */
    LD_DE = (dep_stall || mem_stall) ? 0 : 1;
    
    if (LD_DE) {
        NEW_PS.DE_NPC = PC_OUT;
        NEW_PS.DE_IR = READ_WORD;
        NEW_PS.DE_V = V_OUT;
    }
    
}  

