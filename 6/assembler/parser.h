#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <string.h>

// Instruction types
typedef enum {
    A_INSTRUCTION,
    C_INSTRUCTION,
    L_INSTRUCTION  // Label (xxx)
} instruction_type_t;

// Parser functions
bool is_comment(const char* line);
bool is_whitespace(const char* line);
void strip_whitespace(char* line);
void remove_comments(char* line);

instruction_type_t get_instruction_type(const char* line);
/**
 *  Get the type of instruction from a line of assembly code.
 * for A-instruction the line should start with '@'
 * for C-instruction the line should contain '=' or ';' with fields of dest, comp, jump
 * for L-instruction the line should be enclosed in parentheses (LABEL)
 */

 
// A-instruction: @value or @symbol
char* get_a_instruction_symbol(const char* line);

// C-instruction: dest=comp;jump
char* get_dest(const char* line);
char* get_comp(const char* line);
char* get_jump(const char* line);

// L-instruction: (LABEL)
char* get_label(const char* line);

#endif // PARSER_H


