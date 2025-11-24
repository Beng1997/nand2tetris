#ifndef VM_TRAN_H
#define VM_TRAN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SP 0
#define LCL 1
#define ARG 2   
#define THIS 3
#define THAT 4

//in project 7 we only deal with 2 types of commands: arithmetic and memory access
typedef enum {
    ARITHMETIC,
    PUSH,
    POP
} CommandType;

// Structure to represent a VM command
typedef struct {
    CommandType type;           // Type of command
    char segment[20];           // Memory segment (for push/pop)
    int index;                  // Index for memory access or function arguments
    char command[20];           // For arithmetic commands or label names
} VMCommand;

// Structure to hold translator state
typedef struct {
    FILE* output_file;          // File pointer for writing assembly
    int label_counter;          // Counter for generating unique labels
} CodeWriter;


bool isArithmeticCommand(const char* command); // Check if the command is an arithmetic command

bool isMemoryAccessCommand(const char* command); // Check if the command is a memory access command

bool isCommentOrEmpty(const char* line); // Check if the line is a comment or empty to be skipped


void parseCommand(const char* line, VMCommand* vm_command); // Parse a line into a VMCommand structure

// CodeWriter functions
void codewriterInit(CodeWriter* writer, FILE* output_file); // Initialize the code writer

void writeArithmetic(CodeWriter* writer, const VMCommand* vm_command); // Write assembly code for an arithmetic command

void writePushPop(CodeWriter* writer, const VMCommand* vm_command); // Write assembly code for push/pop commands

char* generateLabel(CodeWriter* writer, const char* base); // Generate a unique label for branching

void writeComment(CodeWriter* writer, const char* comment); // Write a comment in the assembly code

#endif // VM_TRAN_H


