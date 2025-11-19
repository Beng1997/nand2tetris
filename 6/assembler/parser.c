#define _POSIX_C_SOURCE 200809L
#include "parser.h"
#include <stdlib.h>
#include <string.h>



bool is_comment(const char* line) {
    while (*line != '\0' && (*line == ' ' || *line == '\t')) { //skip whitespaces
        line++;
    }
    return *line == '/' && *(line + 1) == '/'; // Check if line starts with //
}



bool is_whitespace(const char* line) {
    while (*line != '\0') {
        if (*line != ' ' && *line != '\t' && *line != '\n' && *line != '\r') { // Check for any non-whitespace character
            return false;
        }
        line++;
    }
    return true;
}



void strip_whitespace(char* line) {
    char* src = line;
    char* dst = line;// Destination pointer starts at the beginning of the line
    while (*src != '\0') {
        if (*src != ' ' && *src != '\t' && *src != '\n' && *src != '\r') { // Skip spaces, tabs, and newlines
            *dst++ = *src; // Copy non-whitespace character
        }
        src++;
    }
    *dst = '\0'; // Null-terminate the modified string
}

void remove_comments(char* line) {
    char* comment_start = strstr(line, "//");//buil t-in function to find substring and return pointer to its first occurrence
    if (comment_start != NULL) {
        *comment_start = '\0'; // Truncate the line at the start of the comment
    }
}



instruction_type_t get_instruction_type(const char* line) { //pretty straightforward
    if (line[0] == '@') {
        return A_INSTRUCTION;
    } else if (line[0] == '(' && line[strlen(line) - 1] == ')') {
        return L_INSTRUCTION;
    } else {
        return C_INSTRUCTION;
    }
}


char* get_a_instruction_symbol(const char* line) {
    return strdup(line + 1); // Duplicate the string after '@'
}


char* get_dest(const char* line) { // Extract dest part from C-instruction
    const char* equal_sign = strchr(line, '='); // Find '=' character
    if (equal_sign != NULL) {
        size_t length = equal_sign - line;
        char* dest = (char*)malloc(length + 1); 
        strncpy(dest, line, length); // Copy dest part
        dest[length] = '\0';
        return dest;
    }
    return strdup(""); // No dest part
}


char* get_comp(const char* line) { // Extract comp part from C-instruction
    const char* start = line;
    const char* end = line + strlen(line);

    const char* equal_sign = strchr(line, '='); // Find '=' character
    if (equal_sign != NULL) {
        start = equal_sign + 1; // Comp starts after '='
    }

    const char* semicolon = strchr(line, ';'); // Find ';' character
    if (semicolon != NULL) {
        end = semicolon; // Comp ends before ';'
    }

    size_t length = end - start;
    char* comp = (char*)malloc(length + 1);
    strncpy(comp, start, length); // Copy comp part
    comp[length] = '\0';
    
    // Remove trailing whitespace/newline
    for (int i = length - 1; i >= 0 && (comp[i] == ' ' || comp[i] == '\t' || comp[i] == '\n' || comp[i] == '\r'); i--) {
        comp[i] = '\0';
    }
    
    return comp;
}
char* get_jump(const char* line) { // Extract jump part from C-instruction
    const char* semicolon = strchr(line, ';'); // Find ';' character
    if (semicolon != NULL) {
        return strdup(semicolon + 1); // Duplicate the string after ';'
    }
    return strdup(""); // No jump part
}

char* get_label(const char* line) { // Extract label from (LABEL)
    if (line[0] != '(' || line[strlen(line) - 1] != ')') {
        return NULL; // Not a valid label
    }
    size_t length = strlen(line) - 2; // Exclude parentheses
    char* label = (char*)malloc(length + 1);
    strncpy(label, line + 1, length);
    label[length] = '\0';
    return label;
}
