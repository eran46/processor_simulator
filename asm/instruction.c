#define _CRT_SECURE_NO_WARNINGS

#define MAX_LINES 4096

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include "instruction.h"
#include "label.h"
#include "utils.h"


/**
 * Parses a line of assembly code into its components (opcode, registers, immediate values, and labels).
 *
 * param: line The line of assembly code to be parsed.
 * param: inst A pointer to the Instruction
 * return: int Returns 1 if an instruction was successfully parsed, 0 if only a label was found.
 */
int parseInstruction(char* line, Instruction* inst) {
    // Removes leading whitespace
    while (isspace((unsigned char)*line)) line++;

    // Check for inline labels (labels ending with ':')
    char* colonPos = strchr(line, ':');
    if (colonPos != NULL) {
        *colonPos = '\0';  
        strcpy(inst->label, line);  // Store the label
        line = colonPos + 1;  // Move past the colon to the rest of the line
        while (isspace((unsigned char)*line)) line++;
    }
    else {
        inst->label[0] = '\0';  // No label found
    }

    // If there's an instruction part after the label, parse it
    if (*line != '\0') {
        sscanf(line, "%s %[^,], %[^,], %[^,], %s", inst->opcode, inst->rd, inst->rs, inst->rt, inst->imm);
        return 1;  // Indicates that an instruction was parsed
    }
    else {
        // If there's no instruction after the label, clear the instruction fields
        inst->opcode[0] = '\0';
        inst->rd[0] = '\0';
        inst->rs[0] = '\0';
        inst->rt[0] = '\0';
        inst->imm[0] = '\0';
        return 0;  // Indicates that only a label was found
    }
}

/**
 * Handles the .word directive in assembly code, writing a specific value to a memory address.
 *
 * param: outfileName The name of the output file where the memory image will be stored.
 * param: inst A pointer to the Instruction
 */
void handleWordDirective(const char* outfileName, Instruction* inst) {
    int addr = 0;
    int data = 0;

    // Parse the address and data (hexa or decimal)
    if (inst->rd[0] == '0' && (inst->rd[1] == 'x' || inst->rd[1] == 'X')) {
        sscanf(inst->rd, "0x%x", &addr);
    }
    else {
        addr = atoi(inst->rd);
    }

    if (inst->imm[0] == '0' && (inst->imm[1] == 'x' || inst->imm[1] == 'X')) {
        sscanf(inst->imm, "0x%x", &data);
    }
    else {
        data = atoi(inst->imm);
    }

    // Write the .word directive to the specific address in the output array
    writeWordDirectiveToOutput(addr, data);
}

/**
 * Converts an assembly instruction into machine code and writes it to the output file.
 *
 * param: outfileName The name of the output file where the machine code will be stored.
 * param: inst A pointer to the Instruction
 * param: labels An array of Label structures containing all the labels in the program.
 * param: labelCount The number of labels in the program.
 * param: isIFormat A flag indicating whether the instruction uses an $imm
 */
void writeMachineCode(const char* outfileName, Instruction* inst, Label* labels, int labelCount, int isIFormat) {
    static int currentAddress = 0;  // Start writing at the beginning of memory

    if (strcmp(inst->opcode, ".word") == 0) {
        // Handle .word directives separately
        sscanf(inst->rd, "%*s %s", inst->imm); //Seperate and take the data correctly
        handleWordDirective(outfileName, inst);
        return;
    }

    int opcode = getOpcode(inst->opcode);
    int rd = registerToNumber(inst->rd);
    int rs = registerToNumber(inst->rs);
    int rt = registerToNumber(inst->rt);
    int imm = 0;
    int isLabel = 0;

    // Handle immediate values and labels
    if (isalpha(inst->imm[0])) { // It's a label (starts with a character)
        isLabel = 1;
        for (int i = 0; i < labelCount; i++) {
            if (strcmp(labels[i].label, inst->imm) == 0) {
                imm = labels[i].address;
                break;
            }
        }
    }
    else if (inst->imm[0] == '0' && inst->imm[1] == 'x') { // It's a hexadecimal number
        sscanf(inst->imm, "0x%x", &imm);
    }
    else { // Decimal
        imm = atoi(inst->imm);
    }

    char machineCode[6]; // 5 characters + null terminator
    snprintf(machineCode, sizeof(machineCode), "%02X%X%X%X", opcode, rd, rs, rt);

    // Write the machine code to the current address
    writeInstructionToOutput(currentAddress, machineCode);

    // For I format ot label, write the immediate value to the next line
    if (isIFormat || isLabel) {
        currentAddress++;
        char immString[6];  // 5 digits + null terminator
        snprintf(immString, sizeof(immString), "%05X", imm & 0xFFFFF);
        writeInstructionToOutput(currentAddress, immString);
    }

    // Increment current address for the next instruction
    currentAddress++;
}

/**
 * Prints the contents of an Instruction structure for debugging purposes.
 *
 * param: inst A pointer to the Instruction
 */
void printInstruction(Instruction* inst) {
    printf("Instruction:\n");
    printf("  Opcode: %s\n", inst->opcode);
    printf("  rd: %s\n", inst->rd);
    printf("  rs: %s\n", inst->rs);
    printf("  rt: %s\n", inst->rt);
    printf("  imm: %s\n", inst->imm);
    printf("  label: %s\n", inst->label);
}
