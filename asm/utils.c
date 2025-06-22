#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

char outputFile[MAX_LINES][LINE_LENGTH];  // Define the output array

/**
 * Initializes the output file array.
 *
 * This function fills the `outputFile` array with default values ("00000\n").
 * 
 */
void initializeOutputFile() {
    for (int i = 0; i < MAX_LINES; i++) {
        snprintf(outputFile[i], LINE_LENGTH, "00000\n");
    }
}

/**
 * Writes an instruction's machine code to a specific address in the output array.
 *
 * param: address The memory address where the machine code should be written.
 * param: machineCode The machine code to be written to the memory address.
 */
void writeInstructionToOutput(int address, const char* machineCode) {
    if (address < MAX_LINES) {
        snprintf(outputFile[address], LINE_LENGTH, "%s\n", machineCode);
    }
    else {
        printf("Address out of bounds: %d\n", address);
    }
}

/**
 * Writes the result of a .word directive to a specific address in the output array.
 *
 * param: address The memory address where the data should be written.
 * param: data The data to be written to the memory address.
 * 
 */
void writeWordDirectiveToOutput(int address, int data) {
    if (address < MAX_LINES) {
        snprintf(outputFile[address], LINE_LENGTH, "%05X\n", data & 0xFFFFF);
    }
    else {
        printf("Address out of bounds: %d\n", address);
    }
}

/**
 *
 * This function writes the memory image from the `outputFile` array to memin
 *
 * param: outfileName The name of the output file where the memory image should be saved.
 */
void flushOutputToFile(const char* outfileName) {
    FILE* outfile = fopen(outfileName, "w");
    if (!outfile) {
        perror("Error opening output file");
        return;
    }
    for (int i = 0; i < MAX_LINES; i++) {
        fputs(outputFile[i], outfile);
        fputs("\n", outfile);// =New line after each line
    }

    fclose(outfile);
}
/**
 * Maps register names to their corresponding register numbers.
 *
 * param: reg A string representing the name of the register (e.g., "$t0").
 * return: int The register number corresponding to the register name. Returns -1 for an invalid register name.
 */
int registerToNumber(char* reg) {
    if (strcmp(reg, "$zero") == 0) return 0;
    if (strcmp(reg, "$imm") == 0) return 1;
    if (strcmp(reg, "$v0") == 0) return 2;
    if (strcmp(reg, "$a0") == 0) return 3;
    if (strcmp(reg, "$a1") == 0) return 4;
    if (strcmp(reg, "$a2") == 0) return 5;
    if (strcmp(reg, "$a3") == 0) return 6;
    if (strcmp(reg, "$t0") == 0) return 7;
    if (strcmp(reg, "$t1") == 0) return 8;
    if (strcmp(reg, "$t2") == 0) return 9;
    if (strcmp(reg, "$s0") == 0) return 10;
    if (strcmp(reg, "$s1") == 0) return 11;
    if (strcmp(reg, "$s2") == 0) return 12;
    if (strcmp(reg, "$gp") == 0) return 13;
    if (strcmp(reg, "$sp") == 0) return 14;
    if (strcmp(reg, "$ra") == 0) return 15;
    return -1; // Invalid register name
}

/**
*Maps instruction opcode to their corresponding opcode values.
*
* params: opcode A string representing the mnemonic of the instruction(e.g., "add").
* return : int The opcode corresponding to the instruction mnemonic.Returns - 1 for an invalid opcode.
*/
int getOpcode(char* opcode) {
    if (strcmp(opcode, "add") == 0) return 0;
    if (strcmp(opcode, "sub") == 0) return 1;
    if (strcmp(opcode, "mul") == 0) return 2;
    if (strcmp(opcode, "and") == 0) return 3;
    if (strcmp(opcode, "or") == 0) return 4;
    if (strcmp(opcode, "xor") == 0) return 5;
    if (strcmp(opcode, "sll") == 0) return 6;
    if (strcmp(opcode, "sra") == 0) return 7;
    if (strcmp(opcode, "srl") == 0) return 8;
    if (strcmp(opcode, "beq") == 0) return 9;
    if (strcmp(opcode, "bne") == 0) return 10;
    if (strcmp(opcode, "blt") == 0) return 11;
    if (strcmp(opcode, "bgt") == 0) return 12;
    if (strcmp(opcode, "ble") == 0) return 13;
    if (strcmp(opcode, "bge") == 0) return 14;
    if (strcmp(opcode, "jal") == 0) return 15;
    if (strcmp(opcode, "lw") == 0) return 16;
    if (strcmp(opcode, "sw") == 0) return 17;
    if (strcmp(opcode, "reti") == 0) return 18;
    if (strcmp(opcode, "in") == 0) return 19;
    if (strcmp(opcode, "out") == 0) return 20;
    if (strcmp(opcode, "halt") == 0) return 21;
    return -1; // Invalid opcode
}