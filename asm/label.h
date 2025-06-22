#pragma once
#ifndef LABEL_H
#define LABEL_H



#define MAX_LABEL_LENGTH 20
#define MEMORY_SIZE 4096

typedef struct {
    char label[MAX_LABEL_LENGTH];
    int address;
} Label;

void collectLabels(char* line, Label* labels, int* labelCount, int* address);


#endif // LABEL_H
