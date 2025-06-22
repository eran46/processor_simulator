#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include "label.h"
#include "instruction.h"

// This file manages label collection and lookup.

/**
 * Collects labels from the assembly code and calculates their corresponding addresses.
 *
 * This function is used during the first pass of the assembler, It scans each line of the assembly code,
 * identifies labels (lines ending with a colon ':'), and stores their addresses in the `labels` array.
 * also updates the program counter (`address`) for each instruction.
 *
 * param: line The current line of assembly code being processed.
 * param: labels An array of `Label` structures where the labels and their addresses are stored.
 * param: labelCount A pointer to the number of labels found so far.
 * param: address A pointer to the current program counter (address) being processed.
 */
void collectLabels(char* line, Label* labels, int* labelCount, int* address) {
    // Remove leading whitespace
    while (isspace((unsigned char)*line)) line++;

    // Check if the line contains a label (ending with a colon ':')
    char* labelPtr = strchr(line, ':');
    if (labelPtr) {
        *labelPtr = '\0';  
        strcpy(labels[*labelCount].label, line);  // Store the label
        labels[*labelCount].address = *address;  // Store the address of the label
        (*labelCount)++;  // Increment the label count
        line = labelPtr + 1;
        while (isspace((unsigned char)*line)) line++;
    }

    // If the remaining line is not empty, we have an instruction to process
    if (*line != '\0') {
        // Check if the line contains a .word directive and return if it does
        if (strstr(line, ".word")) {
            return;  // Don't increment address for .word directives
        }
        // Check if the line contains an immediate value ($imm)
        if (strstr(line, "$imm")) {
            (*address) += 2;  // Adjust address for instructions with immediate values
        }
        else {
            (*address)++;  // Increment address for a normal instruction
        }
    }
}
