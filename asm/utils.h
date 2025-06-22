#pragma once
#ifndef UTILS_H
#define UTILS_H

#define MAX_LINES 4096
#define LINE_LENGTH 6

extern char outputFile[MAX_LINES][LINE_LENGTH];  // Declare the output array

int getOpcode(char* opcode);
int registerToNumber(char* reg);
void initializeOutputFile();
void writeInstructionToOutput(int address, const char* machineCode);
void writeWordDirectiveToOutput(int address, int data);
void flushOutputToFile(const char* outfileName);

#endif // UTILS_H
