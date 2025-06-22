#pragma once
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdio.h> // For FILE*
#include "label.h" // For Label struct

#define MAX_OPCODE_LENGTH 10
#define MAX_REG_LENGTH 10
#define MAX_IMM_LENGTH 10

typedef struct {
    char opcode[MAX_OPCODE_LENGTH];
    char rd[MAX_REG_LENGTH];
    char rs[MAX_REG_LENGTH];
    char rt[MAX_REG_LENGTH];
    char imm[MAX_IMM_LENGTH];
    char label[MAX_LABEL_LENGTH];
} Instruction;

int parseInstruction(char* line, Instruction* inst);
void writeMachineCode(const char* outfileName, Instruction* inst, Label* labels, int labelCount, int isIFormat);
void handleWordDirective(const char* outfileName, Instruction* inst);
void printInstruction(Instruction* inst);



#endif // INSTRUCTION_H
