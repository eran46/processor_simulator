#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "label.h"
#include "utils.h"

#define MAX_LINE_LENGTH 500
#define MEMORY_SIZE 4096 

/**
 * Main function for the assembler.
 *
 * This function is resposible for the process of reading an assembly program,
 * collecting labels, translating the instructions into machine code, and
 * writing the output to a memory file.
 *
 */
int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return 1;
    }

    // input file
    FILE* infile = fopen(argv[1], "r");
    if (!infile) {
        perror("Error opening input file");
        return 1;
    }

    initializeOutputFile();  //Initializing array from which we will build the oputput after

    char line[MAX_LINE_LENGTH];  
    Instruction inst;  // Structure to store the parsed instruction
    Label labels[MAX_LINES];  // Array to store labels found in the program
    int labelCount = 0;  
    int address = 0;  // Program counter (PC = address)

    // First pass: collect labels and calculate their addresses
    while (fgets(line, sizeof(line), infile)) {
        collectLabels(line, labels, &labelCount, &address);
    }

    // Rewind the file pointer to the beginning for the second pass
    rewind(infile);
    address = 0;  

    // Second pass: translate instructions into machine code
    while (fgets(line, sizeof(line), infile)) {
        if (parseInstruction(line, &inst)) { // Parse the instruction from each line
            // Only proceed if an actual instruction was parsed - not a label
            int isIFormat = (strcmp(inst.rt, "$imm") == 0 || strcmp(inst.rs, "$imm") == 0);

            // Write the machine code to the output array
            writeMachineCode(argv[2], &inst, labels, labelCount, isIFormat);
        }
    }

    // write the output array to the output file
    flushOutputToFile(argv[2]);

    fclose(infile);
    return 0;
}
