#include "vm_tran.h"
#include <string.h>

bool isArithmeticCommand(const char* command) {
    return (strcmp(command, "add") == 0 ||
            strcmp(command, "sub") == 0 ||
            strcmp(command, "neg") == 0 ||
            strcmp(command, "eq") == 0 ||
            strcmp(command, "gt") == 0 ||
            strcmp(command, "lt") == 0 ||
            strcmp(command, "and") == 0 ||
            strcmp(command, "or") == 0 ||
            strcmp(command, "not") == 0);
}


bool isMemoryAccessCommand(const char* command) {
    return (strcmp(command, "push") == 0 ||
            strcmp(command, "pop") == 0);
}


bool isCommentOrEmpty(const char* line) {
    // Skip leading whitespace
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    // Check if the line is empty, just newline, or a comment
    return (*line == '\0' || *line == '\n' || *line == '\r' || strncmp(line, "//", 2) == 0);
}

void parseCommand(const char* line, VMCommand* vm_command) {
    char command[20];
    char segment[20];
    int index;

    sscanf(line, "%s %s %d", command, segment, &index); // scan the line and extract tokens for command, segment, and index

    if (isArithmeticCommand(command)) {
        vm_command->type = ARITHMETIC;
        strcpy(vm_command->command, command); // copy the command string
    } else if (isMemoryAccessCommand(command)) {
        if (strcmp(command, "push") == 0) {
            vm_command->type = PUSH;
        } else {
            vm_command->type = POP;
        }
        strcpy(vm_command->segment, segment); // copy the segment string
        vm_command->index = index; 
    }
    /* project 8 additions below */
    else if (isFunctionCommand(command)) {
        if (strcmp(command, "function") == 0) {
            vm_command->type = FUNCTION;
        } else if (strcmp(command, "call") == 0) {
            vm_command->type = CALL;
        } else if (strcmp(command, "return") == 0) {
            vm_command->type = RETURN;
        }
        // For function and call, we need to extract function name and number of args/locals
        if (vm_command->type == FUNCTION || vm_command->type == CALL) {
            char functionName[128];
            int numArgs;
            sscanf(line, "%*s %s %d", functionName, &numArgs);
            strcpy(vm_command->functionName, functionName);
            vm_command->numArgs = numArgs;
        }
    }
    else if (isBranchingCommand(command)) {
        if (strcmp(command, "label") == 0) {
            vm_command->type = LABEL;
        } else if (strcmp(command, "goto") == 0) {
            vm_command->type = GOTO;
        } else if (strcmp(command, "if-goto") == 0) {
            vm_command->type = IF_GOTO;
        }
        // For branching commands, extract the label name
        char labelName[128];
        sscanf(line, "%*s %s", labelName);
        strcpy(vm_command->label, labelName);
    }

}

void codewriterInit(CodeWriter* writer, FILE* output_file) { // Initialize the code writer
    writer->output_file = output_file;
    writer->label_counter = 0;
    /*project 8 additions below */
    writer->currentFunction[0] = '\0'; // Initialize current function to empty
    writer->currentFile[0] = '\0';    // Initialize current file to empty
}   
/*  Implementation for writing assembly code for arithmetic commands
    bassicly we first need to identify which arithmetic command
    then we write the corresponding assembly code to the output file
    the problem is wee first need to bring the topmost elements from the stack into the built-in registers
     we need to print the screen the pointer manipulations represented in the video
    */
void writeArithmetic(CodeWriter* writer, const VMCommand* vm_command) { 
    // Identify which arithmetic command and write the corresponding assembly code
    // Binary operations: pop two values, operate, push result
    // Unary operations: pop one value, operate, push result
    
    if (strcmp(vm_command->command, "add") == 0) {
        fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\nA=A-1\nM=D+M\n"); // SP--; D= *SP; A=SP-1; *A = *A + D
    } else if (strcmp(vm_command->command, "sub") == 0) {
        fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\nA=A-1\nM=M-D\n"); // SP--; D= *SP; A=SP-1; *A = *A - D
    } else if (strcmp(vm_command->command, "neg") == 0) {
        fprintf(writer->output_file, "@SP\nA=M-1\nM=-M\n"); // A=SP-1; *A = -(*A)
    } 
    // For eq, gt, lt we need to generate unique labels for branching and then we compare the top two stack values
    // Based on the comparison we set the top of the stack to true (-1) or false (0)
    // All 3 cases are similar in structure but differ in the jump condition
    else if (strcmp(vm_command->command, "eq") == 0) {
        int label_true = writer->label_counter++; // generate unique label for true case
        int label_end = writer->label_counter++; // generate unique label for end case
        fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n"); // SP--; D= *SP; A=SP-1; D = *A - D
        fprintf(writer->output_file, "@TRUE%d\nD;JEQ\n", label_true);
        fprintf(writer->output_file, "@SP\nA=M-1\nM=0\n"); // false case
        fprintf(writer->output_file, "@END%d\n0;JMP\n", label_end);
        fprintf(writer->output_file, "(TRUE%d)\n@SP\nA=M-1\nM=-1\n", label_true); // true case
        fprintf(writer->output_file, "(END%d)\n", label_end);
    } else if (strcmp(vm_command->command, "gt") == 0) {
        int label_true = writer->label_counter++; // generate unique label for true case
        int label_end = writer->label_counter++; // generate unique label for end case
        fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n"); // SP--; D= *SP; A=SP-1; D = *A - D
        fprintf(writer->output_file, "@TRUE%d\nD;JGT\n", label_true);
        fprintf(writer->output_file, "@SP\nA=M-1\nM=0\n"); // false case
        fprintf(writer->output_file, "@END%d\n0;JMP\n", label_end);
        fprintf(writer->output_file, "(TRUE%d)\n@SP\nA=M-1\nM=-1\n", label_true); // true case
        fprintf(writer->output_file, "(END%d)\n", label_end);
    } else if (strcmp(vm_command->command, "lt") == 0) {
        int label_true = writer->label_counter++; // generate unique label for true case
        int label_end = writer->label_counter++; // generate unique label for end case
        fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n"); // SP--; D= *SP; A=SP-1; D = *A - D
        fprintf(writer->output_file, "@TRUE%d\nD;JLT\n", label_true);
        fprintf(writer->output_file, "@SP\nA=M-1\nM=0\n"); // false case
        fprintf(writer->output_file, "@END%d\n0;JMP\n", label_end);
        fprintf(writer->output_file, "(TRUE%d)\n@SP\nA=M-1\nM=-1\n", label_true); // true case
        fprintf(writer->output_file, "(END%d)\n", label_end);
    } 
    // Bitwise operations are straightforward
    else if (strcmp(vm_command->command, "and") == 0) {
        fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\nA=A-1\nM=D&M\n"); // SP--; D= *SP; A=SP-1; *A = *A & D
    } else if (strcmp(vm_command->command, "or") == 0) {
        fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\nA=A-1\nM=D|M\n"); // SP--; D= *SP; A=SP-1; *A = *A | D
    } else if (strcmp(vm_command->command, "not") == 0) {
        fprintf(writer->output_file, "@SP\nA=M-1\nM=!M\n"); // A=SP-1; *A = !(*A)
    }
}

void writePushPop(CodeWriter* writer, const VMCommand* vm_command) { // Write assembly code for push/pop commands
    //now push is pretty straightforward for constant segment
    if (vm_command->type == PUSH && strcmp(vm_command->segment, "constant") == 0) {
        fprintf(writer->output_file, "@%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", vm_command->index); // Push constant onto stack
    }
    //the problem with both push and pop is in the other segments
    //we need to first calculate the address based on the segment and index
    //then for push we read from that address and write to the stack
    //for pop we read from the stack and write to that address
    else if (vm_command->type == PUSH) {
        // Handle other segments (local, argument, this, that, temp, pointer, static
        const char* segment_pointer;
        if (strcmp(vm_command->segment, "local") == 0) {
            segment_pointer = "LCL";
        } else if (strcmp(vm_command->segment, "argument") == 0) {
            segment_pointer = "ARG";
        } else if (strcmp(vm_command->segment, "this") == 0) {
            segment_pointer = "THIS";
        } else if (strcmp(vm_command->segment, "that") == 0) {
            segment_pointer = "THAT";
        } else if (strcmp(vm_command->segment, "temp") == 0) {
            segment_pointer = "5"; // Temp segment starts at RAM[5]
        } else if (strcmp(vm_command->segment, "pointer") == 0) {
            // Pointer segment: push THIS (pointer 0) or THAT (pointer 1) register value
            if (vm_command->index == 0) {
                fprintf(writer->output_file, "@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n"); // Push THIS value
            } else {
                fprintf(writer->output_file, "@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n"); // Push THAT value
            }
            return;
        } else if (strcmp(vm_command->segment, "static") == 0) {
            fprintf(writer->output_file, "@%s.%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", writer->currentFile, vm_command->index); // Static variable
            return;
        } else {
            return; // Unknown segment  
        }
        if (strcmp(vm_command->segment, "temp") == 0) {
            fprintf(writer->output_file, "@%s\nD=A\n@%d\nA=D+A\nD=M\n", segment_pointer, vm_command->index); // Calculate address for temp
        } else {
            fprintf(writer->output_file, "@%s\nD=M\n@%d\nA=D+A\nD=M\n", segment_pointer, vm_command->index); // Calculate address for other segments
        }
        fprintf(writer->output_file, "@SP\nA=M\nM=D\n@SP\nM=M+1\n"); // Push value onto stack
    } else if (vm_command->type == POP) {
        // Handle pop for other segments
        const char* segment_pointer;
        if (strcmp(vm_command->segment, "local") == 0) {
            segment_pointer = "LCL";
        } else if (strcmp(vm_command->segment, "argument") == 0) {
            segment_pointer = "ARG";
        } else if (strcmp(vm_command->segment, "this") == 0) {
            segment_pointer = "THIS";
        } else if (strcmp(vm_command->segment, "that") == 0) {
            segment_pointer = "THAT";
        } else if (strcmp(vm_command->segment, "temp") == 0) {
            segment_pointer = "5"; // Temp segment starts at RAM[5]
        } else if (strcmp(vm_command->segment, "pointer") == 0) {
            // Pointer segment: pop to THIS (pointer 0) or THAT (pointer 1) register
            if (vm_command->index == 0) {
                fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\n@THIS\nM=D\n"); // Pop to THIS
            } else {
                fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\n@THAT\nM=D\n"); // Pop to THAT
            }
            return;
        } else if (strcmp(vm_command->segment, "static") == 0) {
            fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\n@%s.%d\nM=D\n", writer->currentFile, vm_command->index); // Static variable
            return;
        } else {
            return; // Unknown segment
        }
        // Calculate target address and store in R13
        if (strcmp(vm_command->segment, "temp") == 0) {
            fprintf(writer->output_file, "@%s\nD=A\n@%d\nD=D+A\n@R13\nM=D\n", segment_pointer, vm_command->index); // Calculate address for temp
        } else {
            fprintf(writer->output_file, "@%s\nD=M\n@%d\nD=D+A\n@R13\nM=D\n", segment_pointer, vm_command->index); // Calculate address for other segments
        }
        // Pop value from stack and store at target address
        fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n");
    }
}

char* generateLabel(CodeWriter* writer, const char* base) { // Generate a unique label for branching
    static char label[50];
    sprintf(label, "%s_%d", base, writer->label_counter++);
    return label;
}
void writeComment(CodeWriter* writer, const char* comment) { // Write a comment in the assembly code
    fprintf(writer->output_file, "// %s\n", comment);
}
    
/* project 8 additions below */



bool isFunctionCommand(const char* command) {
    return (strcmp(command, "function") == 0 ||
            strcmp(command, "call") == 0 ||
            strcmp(command, "return") == 0);
}

bool isBranchingCommand(const char* command) {
    return (strcmp(command, "label") == 0 ||
            strcmp(command, "goto") == 0 ||
            strcmp(command, "if-goto") == 0);
}

void writeLabel(CodeWriter* writer, const char* label) {
    fprintf(writer->output_file, "(%s$%s)\n", writer->currentFunction, label); // Scoped label
}

void writeGoto(CodeWriter* writer, const char* label) {
    fprintf(writer->output_file, "@%s$%s\n0;JMP\n", writer->currentFunction, label); // Scoped goto
}

void writeIfGoto(CodeWriter* writer, const char* label) {
    fprintf(writer->output_file, "@SP\nAM=M-1\nD=M\n@%s$%s\nD;JNE\n", writer->currentFunction, label); // Scoped if-goto
}


void writeFunction(CodeWriter* writer, const char* functionName, int numLocals) {
    strcpy(writer->currentFunction, functionName); // Update current function name
    fprintf(writer->output_file, "(%s)\n", functionName); // Function label
    // Initialize local variables to 0
    for (int i = 0; i < numLocals; i++) {
        fprintf(writer->output_file, "@0\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n"); // Push 0 onto stack
    }
}

// now this function follows the workflow explained in the unit
//did a whole procedure for the push and pop because they are used in the call function
void writeCall(CodeWriter* writer, const char* functionName, int numArgs) {
    char returnLabel[50];
    sprintf(returnLabel, "%s$ret.%d", functionName, writer->label_counter++); // Unique return label

    // Push return address
    fprintf(writer->output_file, "@%s\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", returnLabel);
    // Push LCL, ARG, THIS, THAT
    fprintf(writer->output_file,
            "@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n"
            "@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n"
            "@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n"
            "@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
    // Reposition ARG
    fprintf(writer->output_file,
            "@SP\nD=M\n@5\nD=D-A\n@%d\nD=D-A\n@ARG\nM=D\n", numArgs);
    // Reposition LCL
    fprintf(writer->output_file,
            "@SP\nD=M\n@LCL\nM=D\n");
    // Transfer control
    fprintf(writer->output_file,
            "@%s\n0;JMP\n", functionName);
    // Declare return label
    fprintf(writer->output_file,
            "(%s)\n", returnLabel);
}

void writeReturn(CodeWriter* writer) {
    // FRAME = LCL
    fprintf(writer->output_file,
            "@LCL\nD=M\n@R13\nM=D\n"); // R13 = FRAME
    // RET = *(FRAME-5)
    fprintf(writer->output_file,
            "@5\nA=D-A\nD=M\n@R14\nM=D\n"); // R14 = RET
    // *ARG = pop()
    fprintf(writer->output_file,
            "@SP\nAM=M-1\nD=M\n@ARG\nA=M\nM=D\n"); // *ARG = pop()
    // SP = ARG + 1
    fprintf(writer->output_file,
            "@ARG\nD=M+1\n@SP\nM=D\n"); // SP = ARG + 1
    // THAT = *(FRAME-1)
    fprintf(writer->output_file,
            "@R13\nAM=M-1\nD=M\n@THAT\nM=D\n"); // THAT = *(FRAME-1)
    // THIS = *(FRAME-2)
    fprintf(writer->output_file,
            "@R13\nAM=M-1\nD=M\n@THIS\nM=D\n"); // THIS = *(FRAME-2)
    // ARG = *(FRAME-3)
    fprintf(writer->output_file,
            "@R13\nAM=M-1\nD=M\n@ARG\nM=D\n"); // ARG = *(FRAME-3)
    // LCL = *(FRAME-4)
    fprintf(writer->output_file,
            "@R13\nAM=M-1\nD=M\n@LCL\nM=D\n"); // LCL = *(FRAME-4)
    // goto RET
    fprintf(writer->output_file,
            "@R14\nA=M\n0;JMP\n"); // goto RET
}


void setFileName(CodeWriter* writer, const char* fileName) { // Set current file name for static variable scoping
    strcpy(writer->currentFile, fileName);
}

void writeInit(CodeWriter* writer) { //set the convention for the sys.init call
    // Write bootstrap code (SP=256, call Sys.init)
    fprintf(writer->output_file,
            "@256\nD=A\n@SP\nM=D\n"); // Initialize SP to 256
    // Call Sys.init
    writeCall(writer, "Sys.init", 0);
}




    