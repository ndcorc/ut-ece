/*
 Name 1: Nolan Corcoran
 UTEID 1: ndc466
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255


typedef struct {
    int address;
    char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];


enum
{
    DONE, OK, EMPTY_LINE
};


FILE* infile = NULL;
FILE* outfile = NULL;
int currentAddress;
int startAddress;
int labelAddress;
int labelCount;
int instruction;
int dr;
int sr1;
int sr2;
int pcOffset;


void toLower(char *opcode)
{
    int i;
    for (i = 0; opcode[i] != '\0'; i++)
    {
        opcode[i] = (char)tolower(opcode[i]);
    }
}


int isOpcode(char *opcode) /* Returns 1 if valid opcode, -1 if not valid opcode */
{
    char validOpcodes[28][6] =
    {
        "add", "and", "br", "brn", "brz", "brp", "brnz", "brzp", "brnp", "brnzp", "halt", "jmp",
        "jsr", "jsrr", "ldb", "ldw", "lea", "not", "nop", "ret", "rti", "lshf", "rshfl", "rshfa",
        "stb", "stw", "trap", "xor"
    };
    
    int i;
    for (i = 0; i < 28; i++)
    {
        if (strcmp(opcode, validOpcodes[i]) == 0) { return (1); }
    }
    return (-1);
}


int isPseudoOp(char *pseudoOp) /* Returns 1 if valid pseudo op, -1 if not valid psuedo op */
{
    char validPseudoOps[3][6] = { ".orig", ".end", ".fill" };
    int i;
    for (i = 0; i < 3; i++)
    {
        if (strcmp(pseudoOp, validPseudoOps[i]) == 0) { return (1); }
    }
    return (-1);
}


int toNum(char *pStr)
{
    char *t_ptr;
    char *orig_pStr;
    int t_length, k;
    int lNum, lNeg = 0;
    long int lNumLong;
    
    orig_pStr = pStr;
    if (*pStr == '#')                /* decimal */
    {
        pStr++;
        if (*pStr == '-')                /* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for (k = 0; k < t_length; k++)
        {
            if (!isdigit(*t_ptr))
            {
                printf("Error: invalid decimal operand, %s\n", orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg) { lNum = -lNum; }
        return lNum;
    }
    else if (*pStr == 'x')    /* hex */
    {
        pStr++;
        if (*pStr == '-')                 /* hex is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for (k = 0; k < t_length; k++)
        {
            if (!isxdigit(*t_ptr))
            {
                printf("Error: invalid hex operand, %s\n", orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX) ? INT_MAX : lNumLong;
        if (lNeg) { lNum = -lNum; }
        return lNum;
    }
    else
    {
        printf("Error: invalid operand, %s\n", orig_pStr);
        exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
    }
}


/* Take a line of the input file and parse it into corresponding fields */
int readAndParse(FILE *pInfile, char *pLine, char **pLabel, char
                 **pOpcode, char **pArg1, char **pArg2, char **pArg3, char **pArg4
                 )
{
    char *lPtr;
    int i;
    if (!fgets(pLine, MAX_LINE_LENGTH, pInfile)) { return(DONE); }
    for (i = 0; i < strlen(pLine); i++)
    {
        pLine[i] = tolower(pLine[i]);
    }
    
    /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);
    
    /* ignore the comments */
    lPtr = pLine;
    
    while (*lPtr != ';' && *lPtr != '\0' && *lPtr != '\n')
    {
        lPtr++;
    }
    
    *lPtr = '\0';
    if (!(lPtr = strtok(pLine, "\t\n ,")))
    {
        return(EMPTY_LINE);
    }
    
    if (isOpcode(lPtr) == -1 && lPtr[0] != '.') /* found a label */
    {
        *pLabel = lPtr;
        if (!(lPtr = strtok(NULL, "\t\n ,"))) { return(OK); }
    }
    
    *pOpcode = lPtr;
    if (!(lPtr = strtok(NULL, "\t\n ,"))) { return(OK); }
    
    *pArg1 = lPtr;
    if (!(lPtr = strtok(NULL, "\t\n ,"))) { return(OK); }
    
    *pArg2 = lPtr;
    if (!(lPtr = strtok(NULL, "\t\n ,"))) { return(OK); }
    
    *pArg3 = lPtr;
    if (!(lPtr = strtok(NULL, "\t\n ,"))) { return(OK); }
    
    *pArg4 = lPtr;
    
    return(OK);
}
/* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */

int validLabel(char *label) /* returns 1 if label is valid, -1 if label is invalid */
{
    char *ptr = label;
    if (ptr[0] == 'x')
    {
        /* Error: label can't start with 'x' */
        return (-1);
    }
    
    /* !alphanumeric || tooLong */
    int i;
    for (i = 0; ptr[i] != '\0'; i++)
    {
        if (i == MAX_LABEL_LEN)
        {
            /* Error: too long of label */
            return (-1);
        }
        if (isalnum(ptr[i]) == 0)
        {
            /* Error: non-alphanumeric */
            return (-1);
        }
    }
    
    /* opcode || pseudoOp */
    if (isOpcode(label) == 1 || isPseudoOp(label) == 1)
    {
        /* Error: label can't be opcode */
        return (-1);
    }
    
    /* label already exists */
    for (i = 0; i < labelCount; i++)
    {
        if (strcmp(label, symbolTable[i].label) == 0)
        {
            /* Error: label already exists */
            return (-1);
        }
    }
    
    /* check for illegal label */
    if (strcmp(label, "in") == 0 || strcmp(label, "out") == 0 || strcmp(label, "getc") == 0 || strcmp(label, "puts") == 0)
    {
        /* Error: illegal label */
        return (-1);
    }
    
    return (1);
    
}

/* Returns the integer of the register if it is valid, -1 if register is invalid */
int getRegister(char *reg)
{
    if (strlen(reg) != 2)
    {
        return (-1);
    }
    if (reg[0] != 'r')
    {
        return (-1);
    }
    int regNum = reg[1] - '0';
    if (regNum < 0 || regNum > 9)
    {
        return (-1);
    }
    return (regNum);
}

/* Returns the address of the label if it is defined, -1 if it is not */
int getAddress(char *label)
{
    int i;
    for (i = 0; i < labelCount; i++) {
        if (strcmp(label, symbolTable[i].label) == 0)
            return symbolTable[i].address;
    }
    return (-1);
}

void ADD(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x1000;
    
    if (*op4 != '\0')
    {
        /* Error: too many args */
        exit(4);
    }
    
    dr = getRegister(op1);
    sr1 = getRegister(op2);
    sr2 = getRegister(op3);
    
    if (dr == -1)
    {
        /* Error: invalid DR */
        exit(4);
    }
    if (sr1 == -1)
    {
        /* Error: invalid SR1 */
        exit(4);
    }
    if (sr2 == -1) /* Handle case for immediate values */
    {
        sr2 = toNum(op3);
        if (sr2 < -16 || sr2 > 15)
        {
            /* Error: Invalid immediate value */
            exit(3);
        }
        else
        {
            instruction += (dr << 9);
            instruction += (sr1 << 6);
            instruction += (1 << 5);
            instruction += (sr2 & 0x1F);
        }
    }
    else
    {
        instruction += (dr << 9);
        instruction += (sr1 << 6);
        instruction += (sr2 & 0x001F);
    }
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void AND(char *op1, char *op2, char *op3, char *op4)
{
    int instruction = 0x5000;
    
    if (*op4 != '\0')
    {
        /* Error: too many args */
        exit(4);
    }
    
    int dr = getRegister(op1);
    int sr1 = getRegister(op2);
    int sr2 = getRegister(op3);
    
    if (dr == -1)
    {
        /* Error: invalid DR */
        exit(4);
    }
    if (sr1 == -1)
    {
        /* Error: invalid SR1 */
        exit(4);
    }
    if (sr2 == -1) /* Handle case for immediate values */
    {
        sr2 = toNum(op3);
        if (sr2 < -16 || sr2 > 15)
        {
            /* Error: Invalid immediate value */
            exit(3);
        }
        else
        {
            instruction += (dr << 9) + (sr1 << 6) + (1 << 5) + (sr2 & 0x001F);
        }
    }
    else
    {
        instruction += (dr << 9) + (sr1 << 6) + sr2;
    }
    fprintf(outfile, "0x%0.4X\n", instruction);
}


void BR(char *op1, char *op2, char *op3, char *op4)
{
    int instruction = 0x0E00;
    int pcOffset;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    labelAddress = getAddress(op1);
    if (labelAddress == -1)
    {
        /* Error: undefined label */
        exit(1);
    }
    pcOffset = (labelAddress - (currentAddress + 2)) / 2;;
    if (pcOffset < -256 || pcOffset > 255)
    {
        /* Offset out of bounds */
        exit(4);
    }
    instruction += (pcOffset & 0x01FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void BRN(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x0800;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    labelAddress = getAddress(op1);
    if (labelAddress == -1)
    {
        /* Error: undefined label */
        exit(1);
    }
    pcOffset = (labelAddress - (currentAddress + 2)) / 2;
    if (pcOffset < -256 || pcOffset > 255)
    {
        /* Offset out of bounds */
        exit(4);
    }
    instruction += (pcOffset & 0x01FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void BRZ(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x0400;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    labelAddress = getAddress(op1);
    if (labelAddress == -1)
    {
        /* Error: undefined label */
        exit(1);
    }
    pcOffset = (labelAddress - (currentAddress + 2)) / 2;
    if (pcOffset < -256 || pcOffset > 255)
    {
        /* Offset out of bounds */
        exit(4);
    }
    instruction += (pcOffset & 0x01FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void BRP(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x0200;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    labelAddress = getAddress(op1);
    if (labelAddress == -1)
    {
        /* Error: undefined label */
        exit(1);
    }
    pcOffset = (labelAddress - (currentAddress + 2)) / 2;
    if (pcOffset < -256 || pcOffset > 255)
    {
        /* Offset out of bounds */
        exit(4);
    }
    instruction += (pcOffset & 0x01FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void BRNZ(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x0C00;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    labelAddress = getAddress(op1);
    if (labelAddress == -1)
    {
        /* Error: undefined label */
        exit(1);
    }
    pcOffset = (labelAddress - (currentAddress + 2)) / 2;
    if (pcOffset < -256 || pcOffset > 255)
    {
        /* Offset out of bounds */
        exit(4);
    }
    instruction += (pcOffset & 0x01FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void BRZP(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x0600;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    labelAddress = getAddress(op1);
    if (labelAddress == -1)
    {
        /* Error: undefined label */
        exit(1);
    }
    pcOffset = (labelAddress - (currentAddress + 2)) / 2;
    if (pcOffset < -256 || pcOffset > 255)
    {
        /* Offset out of bounds */
        exit(4);
    }
    instruction += (pcOffset & 0x01FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void BRNP(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x0A00;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    labelAddress = getAddress(op1);
    if (labelAddress == -1)
    {
        /* Error: undefined label */
        exit(1);
    }
    pcOffset = (labelAddress - (currentAddress + 2)) / 2;
    if (pcOffset < -256 || pcOffset > 255)
    {
        /* Offset out of bounds */
        exit(4);
    }
    instruction += (pcOffset & 0x01FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void HALT(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0xF025;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void JMP(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0xC000;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    sr1 = getRegister(op1);
    if (sr1 == -1)
    {
        exit(4);
    }
    instruction += (sr1 << 6);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void JSR(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x4000;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    labelAddress = getAddress(op1);
    if (labelAddress == -1)
    {
        /* Error: undefined label */
        exit(1);
    }
    pcOffset = (labelAddress - (currentAddress + 2)) / 2;
    if (pcOffset < -1024 || pcOffset > 1023)
    {
        exit(4);
    }
    instruction += (1 << 11) + (pcOffset & 0x07FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void JSRR(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x4000;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        /* Error: too many arguments */
        exit(4);
    }
    sr1 = getRegister(op1);
    if (sr1 == -1)
    {
        exit(4);
    }
    instruction += (sr1 << 6);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void LDB(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x2000;
    if (*op4 != '\0')
    {
        exit(4);
    }
    dr = getRegister(op1);
    sr1 = getRegister(op2);
    pcOffset = toNum(op3);
    if (pcOffset < -32 || pcOffset > 31)
    {
        exit(4);
    }
    instruction += (dr << 9) + (sr1 << 6) + (pcOffset & 0x003F);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void LDW(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x6000;
    if (*op4 != '\0')
    {
        exit(4);
    }
    dr = getRegister(op1);
    sr1 = getRegister(op2);
    pcOffset = toNum(op3) << 1;
    if (pcOffset < -32 || pcOffset > 31)
    {
        exit(4);
    }
    instruction += (dr << 9) + (sr1 << 6) + (pcOffset & 0x003F);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void LEA(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0xE000;
    if (*op3 != '\0' || *op4 != '\0')
    {
        exit(4);
    }
    dr = getRegister(op1);
    labelAddress = getAddress(op2);
    if (labelAddress == -1) { exit(1); }
    pcOffset = (labelAddress - (currentAddress + 2)) / 2;
    if (pcOffset < -256 || pcOffset > 255)
    {
        exit(4);
    }
    instruction += (dr << 9) + (pcOffset & 0x01FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void NOT(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x9000;
    if (*op3 != '\0' || *op4 != '\0')
    {
        exit(4);
    }
    dr = getRegister(op1);
    sr1 = getRegister(op2);
    instruction += (dr << 9) + (sr1 << 6) + 0x003F;
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void LSHF(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0xD000;
    if (*op4 != '\0')
    {
        exit(4);
    }
    dr = getRegister(op1);
    sr1 = getRegister(op2);
    sr2 = toNum(op3);
    if (sr2 < 0 || sr2 > 15)
    {
        exit(3);
    }
    instruction += (dr << 9) + (sr1 << 6) + (sr2 & 0x000F);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void RSHFL(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0xD000;
    if (*op4 != '\0')
    {
        exit(4);
    }
    dr = getRegister(op1);
    sr1 = getRegister(op2);
    sr2 = toNum(op3);
    if (sr2 < 0 || sr2 > 15)
    {
        exit(3);
    }
    instruction += (dr << 9) + (sr1 << 6) + (1 << 4) + (sr2 & 0x000F);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void RSHFA(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0xD000;
    if (*op4 != '\0')
    {
        exit(4);
    }
    dr = getRegister(op1);
    sr1 = getRegister(op2);
    sr2 = toNum(op3);
    if (sr2 < 0 || sr2 > 15)
    {
        exit(3);
    }
    instruction += (dr << 9) + (sr1 << 6) + (3 << 4) + (sr2 & 0x000F);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void STB(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x3000;
    if (*op4 != '\0')
    {
        exit(4);
    }
    sr1 = getRegister(op1);
    sr2 = getRegister(op2);
    pcOffset = toNum(op3);
    if (pcOffset < -32 || pcOffset > 31)
    {
        exit(4);
    }
    instruction += (sr1 << 9) + (sr2 << 6) + (pcOffset & 0x003F);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void STW(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x7000;
    if (*op4 != '\0')
    {
        exit(4);
    }
    sr1 = getRegister(op1);
    sr2 = getRegister(op2);
    pcOffset = toNum(op3) << 1;
    if (pcOffset < -32 || pcOffset > 31)
    {
        exit(4);
    }
    instruction += (sr1 << 9) + (sr2 << 6) + (pcOffset & 0x003F);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void TRAP(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0xF000;
    if (*op2 != '\0' || *op3 != '\0' || *op4 != '\0')
    {
        exit(4);
    }
    int vector = toNum(op1);
    if (vector != 0x0020 && vector != 0x0021 && vector != 0x0022 && vector != 0x0023 && vector != 0x0025)
    {
        exit(4);
    }
    instruction += (vector & 0x00FF);
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void XOR(char *op1, char *op2, char *op3, char *op4)
{
    instruction = 0x9000;
    if (*op4 != '\0')
    {
        exit(4);
    }
    dr = getRegister(op1);
    sr1 = getRegister(op2);
    sr2 = getRegister(op3);
    if (sr2 == -1) /* Check if immediate value */
    {
        sr2 = toNum(op3);
        if (sr2 < -16 || sr2 > 15)
        {
            /* Error: Invalid immediate value */
            exit(3);
        }
        else
        {
            instruction += (dr << 9) + (sr1 << 6) + (1 << 5) + (sr2 & 0x001F);
        }
    }
    else
    {
        instruction += (dr << 9) + (sr1 << 6) + sr2;
    }
    fprintf(outfile, "0x%0.4X\n", instruction);
}

void FILL(char *op1, char *op2, char *op3, char *op4)
{
    if (*op2 != '\0' || *op3 != '\0' || op4[0] != '\0')
    {
        exit(4);
    }
    sr1 = toNum(op1);
    if (sr1 < -32768 || sr1 > 32767)
    {
        exit(3);
    }
    sr1 &= 0xFFFF;
    fprintf(outfile, "0x%0.4X\n", sr1);
}


int main(int argc, char *argv[])
{
    /* open the source file */

     infile = fopen(argv[1], "r");
     outfile = fopen(argv[2], "w");
     
     if (!infile)
     {
     printf("Error: Cannot open file %s\n", argv[1]);
     exit(4);
     }
     
     if (!outfile)
     {
     printf("Error: Cannot open file %s\n", argv[2]);
     exit(4);
     }
     
/*
    infile = fopen("test.s", "r");
    outfile = fopen("out", "w");
*/
    
    /* pass 1 */
    labelCount = 0;
    int startFlag = 0;
    int stopFlag = 0;
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
    int lRet;
    
    do
    {
        lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
        if (lRet != DONE && lRet != EMPTY_LINE)
        {
            if (strcmp(lOpcode, ".orig") == 0)
            {
                startAddress = toNum(lArg1);
                if (startAddress < 0 || startAddress > 65535)
                {
                    /* Error: Origin address out of bounds */
                    fclose(infile);
                    exit(3);
                }
                if (startAddress % 2 != 0)
                {
                    /* Error: Origin address must be even */
                    fclose(infile);
                    exit(3);
                }
                if (*lArg2 != '\0')
                {
                    /* Error: Too many arguments */
                    fclose(infile);
                    exit(4);
                }
                if (startFlag == 1)
                {
                    /* Error: Already initialized .ORIG */
                    fclose(infile);
                    exit(4);
                }
                startFlag = 1;
                currentAddress = startAddress;
                continue;
            }
            if (strcmp(lOpcode, ".end") == 0)
            {
                if (startFlag == 0)
                {
                    /* Error: File does not begin with .ORIG */
                    fclose(infile);
                    exit(4);
                }
                stopFlag = 1;
                break;
            }
            if (*lLabel != '\0')
            {
                if (startFlag == 0)
                {
                    /* Error: File does not begin with .ORIG */
                    fclose(infile);
                    exit(4);
                }
                if (validLabel(lLabel) == -1)
                {
                    /* Error: Invalid Label */
                    fclose(infile);
                    exit(4);
                }
                /* Add Label to Symbol Table */
                strcpy(symbolTable[labelCount].label, lLabel);
                symbolTable[labelCount].address = currentAddress;
                /* printf(Label %s initalized to address %s); */
                labelCount += 1;
            }
            currentAddress += 2;
        }
    } while (lRet != DONE);
    
    if (stopFlag == 0)
    {
        /* Error: No .end in file */
        exit(4);
    }
    /* End of Pass 1 */
    
    rewind(infile);
    
    /* Pass 2 */
    currentAddress = startAddress;
    startFlag = 0;
    stopFlag = 0;
    do
    {
        lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
        if (lRet != DONE && lRet != EMPTY_LINE)
        {
            if (strcmp(lOpcode, ".orig") == 0)
            {
                startFlag += 1;
                fprintf(outfile, "0x%0.4X\n", startAddress);
            }
            else if (isOpcode(lOpcode) == -1 && isPseudoOp(lOpcode) == -1)
            {
                /* Error: Invalid opcode */
                exit(2);
            }
            else /* Handle Opcodes PseudoOps */
            {
                if (strcmp(lOpcode, "add") == 0) { ADD(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "and") == 0) { AND(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "br") == 0 || strcmp(lOpcode, "brnzp") == 0) { BR(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "brn") == 0) { BRN(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "brz") == 0) { BRZ(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "brp") == 0) { BRP(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "brnz") == 0) { BRNZ(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "brzp") == 0) { BRZP(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "brnp") == 0) { BRNP(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "halt") == 0) { HALT(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "jmp") == 0) { JMP(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "jsr") == 0) { JSR(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "jsrr") == 0) { JSRR(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "ldb") == 0) { LDB(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "ldw") == 0) { LDW(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "lea") == 0) { LEA(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "nop") == 0) { fprintf(outfile, "0x0000\n"); }
                else if (strcmp(lOpcode, "not") == 0) { NOT(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "ret") == 0) { fprintf(outfile, "0xC1C0\n"); }
                else if (strcmp(lOpcode, "lshf") == 0) { LSHF(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "rshfl") == 0) { RSHFL(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "rshfa") == 0) { RSHFA(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "rti") == 0) { fprintf(outfile, "0x8000\n"); }
                else if (strcmp(lOpcode, "stb") == 0) { STB(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "stw") == 0) { STW(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "trap") == 0) { TRAP(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, "xor") == 0) { XOR(lArg1, lArg2, lArg3, lArg4); }
                else if (strcmp(lOpcode, ".fill") == 0) { FILL(lArg1, lArg2, lArg3, lArg4); }
                currentAddress += 2;
            }
        }
    } while (lRet != DONE);
    
    fclose(infile);
    fclose(outfile);
    exit(0);
}
