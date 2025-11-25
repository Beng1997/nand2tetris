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
//in project 8 we add: branching (label, goto, if-goto) and function commands (function, call, return)
typedef enum {
    ARITHMETIC,
    PUSH,
    POP,
    LABEL,      // label command
    GOTO,       // goto command
    IF_GOTO,    // if-goto command
    FUNCTION,   // function declaration
    CALL,       // function call
    RETURN      // return from function
} CommandType;

// Structure to represent a VM command
typedef struct {
    CommandType type;           // Type of command
    char segment[20];           // Memory segment (for push/pop)
    int index;                  // Index for memory access or function arguments
    char command[20];           // For arithmetic commands
    char label[128];            // For branching commands (label, goto, if-goto)
    char functionName[128];     // For function/call commands
    int numArgs;                // Number of arguments for call or locals for function
} VMCommand;

// Structure to hold translator state
typedef struct {
    FILE* output_file;          // File pointer for writing assembly
    int label_counter;          // Counter for generating unique labels
    // added for project 8 to support functions and branching with multiple files
    char currentFunction[128];  // Current function name for scoping labels
    char currentFile[128];      // Current file name for static variable scoping
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


/*
 now at project 8 there are some thing we need to add:
    1. function call and return commands
    2. branching commands: label, goto, if-goto
    3. initialization code (sys.init call and multiple files handling)

*/


bool isFunctionCommand(const char* command); // Check if the command is a function command

bool isBranchingCommand(const char* command); // Check if the command is a branching command

// Branching commands
void writeLabel(CodeWriter* writer, const char* label); // Write assembly code for label command

void writeGoto(CodeWriter* writer, const char* label); // Write assembly code for goto command

void writeIfGoto(CodeWriter* writer, const char* label); // Write assembly code for if-goto command

// Function commands
void writeFunction(CodeWriter* writer, const char* functionName, int numLocals); // Write assembly code for function declaration

void writeCall(CodeWriter* writer, const char* functionName, int numArgs); // Write assembly code for function call

void writeReturn(CodeWriter* writer); // Write assembly code for return command

void setFileName(CodeWriter* writer, const char* fileName); // Set current file name for static variable scoping

void writeInit(CodeWriter* writer); // Write bootstrap code (SP=256, call Sys.init)

#endif // VM_TRAN_H


