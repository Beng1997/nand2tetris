#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "coder.h"
#include "symboltable.h"




int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <inputfile.asm>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //check if the file is .asm
    const char* input_filename = argv[1];
    size_t len = strlen(input_filename);
    if (len < 4 || strcmp(input_filename + len - 4, ".asm") != 0) {
        fprintf(stderr, "Error: Input file must have a .asm extension\n");
        return EXIT_FAILURE;
    }

    // Open input file
    FILE* input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }
    // Create output file name by replacing .asm with .hack 
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%.*shack", (int)(len - 4), input_filename);// copy all but last 4 chars and append .hack
    FILE* output_file = fopen(output_filename, "w"); // give write permission
    if (output_file == NULL) {
        perror("Error creating output file");
        fclose(input_file);
        return EXIT_FAILURE;
    }
    // Initialize symbol table with predefined symbols
    symbol_table_t symbol_table;
    init_symbol_table(&symbol_table, 64); // initial capacity of 64 entries, can be adjusted if needed, we addressed that in symboltable.c
    init_predefined_symbols(&symbol_table); // add predefined symbols like R0-R15, SP, LCL, etc.


    //now well use the workflow suggested in the unit: first pass to handle labels
    char line[256];
    int rom_address = 0; // Address for instructions
    while (fgets(line, sizeof(line), input_file)) { // Read line by line
        strip_whitespace(line);
        remove_comments(line);
        // we need to rembmer empty lines doent count as instructions
        if (is_whitespace(line) || is_comment(line)) {
            continue; // Skip empty lines and comments
        }
        instruction_type_t type = get_instruction_type(line);
        if (type == L_INSTRUCTION) {
            char* label = get_label(line);
            if (!contains_symbol(&symbol_table, label)) {
                add_symbol(&symbol_table, label, rom_address); // Add label with current ROM address
            }
            free(label);
        } else {
            rom_address++; // Only increment for A and C instructions
        }
    }

    // now we do the second pass to handle variables and generate machine code
    rewind(input_file); // Reset file pointer to beginning
    int ram_address = 16; // Starting address for variables
    while (fgets(line, sizeof(line), input_file)) {
        strip_whitespace(line);
        remove_comments(line);
        if (is_whitespace(line) || is_comment(line)) {
            continue; // Skip empty lines and comments
        }
        instruction_type_t type = get_instruction_type(line);
        if (type == L_INSTRUCTION) {
            continue; // Skip labels in second pass
        } else if (type == A_INSTRUCTION) {
            char* symbol = get_a_instruction_symbol(line);
            int value;
            if (isdigit(symbol[0])) { 
                value = atoi(symbol); // Numeric value
            } else {
                if (!contains_symbol(&symbol_table, symbol)) { // New variable
                    add_symbol(&symbol_table, symbol, ram_address++);
                }
                value = lookup_symbol(&symbol_table, symbol);
            }
            char* binary = encode_a_instruction(value);
            fprintf(output_file, "%s\n", binary);
            free(binary);
            free(symbol);
        } else if (type == C_INSTRUCTION) {
            char* dest = get_dest(line);
            char* comp = get_comp(line);
            char* jump = get_jump(line);
            char* binary = encode_c_instruction(dest, comp, jump);
            if (binary != NULL) {
                fprintf(output_file, "%s\n", binary); // Write machine code to output file
                free(binary);
            } else { // Encoding failed
                fprintf(stderr, "Error encoding C-instruction: %s (dest='%s', comp='%s', jump='%s')\n", line, dest, comp, jump);
            }
            free(dest);
            free(comp);
            free(jump);
        }
        // L-instructions are ignored in the second pass
    }



    return EXIT_SUCCESS;
}