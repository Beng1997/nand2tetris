#include "vm_tran.h"
#include <string.h>


// tha main file, we take input from command line arguments and process the VM files
int main(int argc, char* argv[]) {
    if (argc != 2) { // check for correct number of arguments
        fprintf(stderr, "Usage: %s <input.vm> <output.asm>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_filename = argv[1];
    char output_filename[512]; // Buffer large enough to hold path + extension
    
    // Create output filename by replacing .vm extension with .asm
    char input_copy[512];
    strncpy(input_copy, input_filename, sizeof(input_copy) - 1);
    input_copy[sizeof(input_copy) - 1] = '\0'; // Ensure null termination
    
    char* dot = strrchr(input_copy, '.');
    if (dot && strcmp(dot, ".vm") == 0) {
        *dot = '\0'; // Remove .vm extension
    }
    snprintf(output_filename, sizeof(output_filename), "%s.asm", input_copy); // Append .asm extension

    FILE* input_file = fopen(input_filename, "r");
    if (!input_file) { // check if input file opened successfully
        fprintf(stderr, "Error: Could not open input file %s\n", input_filename);
        return EXIT_FAILURE;
    }


    FILE* output_file = fopen(output_filename, "w");
    if (!output_file) { // check if output file opened successfully
        fprintf(stderr, "Error: Could not open output file %s\n", output_filename);
        fclose(input_file);
        return EXIT_FAILURE;
    }   

    CodeWriter writer;
    codewriterInit(&writer, output_file); // initialize code writer
    char line[256];
    while (fgets(line, sizeof(line), input_file)) { // read each line from input file
        if (isCommentOrEmpty(line)) {
            continue; // skip comments and empty lines
        }
        VMCommand vm_command;
        parseCommand(line, &vm_command); // parse the line into a VMCommand structure
        if (vm_command.type == ARITHMETIC) {
            writeComment(&writer, line); // write comment for clarity
            writeArithmetic(&writer, &vm_command); // write assembly code for arithmetic command
        } else if (vm_command.type == PUSH || vm_command.type == POP) {
            writeComment(&writer, line); // write comment for clarity
            writePushPop(&writer, &vm_command); // write assembly code for push/pop commands
        }
    }

    fclose(input_file);
    fclose(output_file);
    return EXIT_SUCCESS;
}