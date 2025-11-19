#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symboltable.h"

// Simple hash function
static size_t hash(const char* str, size_t capacity) {
    size_t hash = 5381; //prime number seed
    int c;
    while ((c = *str++)) { 
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % capacity;
}

void init_symbol_table(symbol_table_t* table, size_t initial_capacity) {
    table->symbols = (hash_table_entry_t**)malloc(initial_capacity * sizeof(hash_table_entry_t*)); // Allocate array of pointers
    if (table->symbols == NULL) {
        fprintf(stderr, "Failed to allocate memory for symbol table\n");
        exit(EXIT_FAILURE);
    }
    table->count = 0;
    table->capacity = initial_capacity;
    for (size_t i = 0; i < initial_capacity; i++) {
        table->symbols[i] = NULL;
    }
}

void free_symbol_table(symbol_table_t* table) {
    for (size_t i = 0; i < table->capacity; i++) {
        hash_table_entry_t* entry = table->symbols[i];
        while (entry != NULL) {
            hash_table_entry_t* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(table->symbols);
    table->symbols = NULL;
    table->count = 0;
    table->capacity = 0;
}

bool is_predefined_symbol(const char* symbol) {
    const char* predefined_symbols[] = {
        "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
        "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
        "SCREEN", "KBD", "SP", "LCL", "ARG", "THIS", "THAT"
    };
    size_t num_predefined = sizeof(predefined_symbols) / sizeof(predefined_symbols[0]); // is 23 but this generic calculation is better optional for changes
    for (size_t i = 0; i < num_predefined; i++) {
        if (strcmp(symbol, predefined_symbols[i]) == 0) {
            return true;
        }
    }
    return false;
}

int get_predefined_value(const char* symbol) {
    // R0-R15 registers
    if (symbol[0] == 'R' && symbol[1] >= '0' && symbol[1] <= '9') { // R0-R9 or R10-R15
        if (symbol[2] == '\0') {
            return symbol[1] - '0'; // R0-R9
        } else if (symbol[2] >= '0' && symbol[2] <= '5' && symbol[3] == '\0') {
            return 10 + (symbol[2] - '0'); // R10-R15
        }
    }
    // Special symbols
    if (strcmp(symbol, "SP") == 0) return 0;
    if (strcmp(symbol, "LCL") == 0) return 1;
    if (strcmp(symbol, "ARG") == 0) return 2;
    if (strcmp(symbol, "THIS") == 0) return 3;
    if (strcmp(symbol, "THAT") == 0) return 4;
    if (strcmp(symbol, "SCREEN") == 0) return 16384;
    if (strcmp(symbol, "KBD") == 0) return 24576;
    
    return -1; // Not a predefined symbol
}

void init_predefined_symbols(symbol_table_t* table) { // Initialize the symbol table with predefined symbols using add_symbol
    // Add R0-R15
    for (int i = 0; i <= 15; i++) {
        char reg_name[4];
        snprintf(reg_name, sizeof(reg_name), "R%d", i);
        add_symbol(table, reg_name, i);
    }
    // Add special symbols
    add_symbol(table, "SP", 0);
    add_symbol(table, "LCL", 1);
    add_symbol(table, "ARG", 2);
    add_symbol(table, "THIS", 3);
    add_symbol(table, "THAT", 4);
    add_symbol(table, "SCREEN", 16384);
    add_symbol(table, "KBD", 24576);
}

bool is_label_symbol(const char* symbol) {
    return symbol[0] == '(' && symbol[strlen(symbol) - 1] == ')'; //checks if symbol is enclosed in parentheses
}

bool is_variable_symbol(const char* symbol) {
    return !is_predefined_symbol(symbol) && !is_label_symbol(symbol); // as defined in the video of the unit
}


void add_symbol(symbol_table_t* table, const char* symbol, int value) {
    size_t index = hash(symbol, table->capacity); // Get hash index
    
    // Create new entry
    hash_table_entry_t* new_entry = (hash_table_entry_t*)malloc(sizeof(hash_table_entry_t));
    if (new_entry == NULL) {
        fprintf(stderr, "Failed to allocate memory for hash table entry\n");
        exit(EXIT_FAILURE);
    }
    
    new_entry->key = strdup(symbol);
    if (new_entry->key == NULL) {
        fprintf(stderr, "Failed to duplicate symbol string\n");
        free(new_entry);
        exit(EXIT_FAILURE);
    }
    new_entry->value = value;
    new_entry->next = table->symbols[index]; // Chain to existing entry if collision
    
    table->symbols[index] = new_entry;
    table->count++;
}

int lookup_symbol(symbol_table_t* table, const char* symbol) { // Lookup symbol and return its value or -1 if not found
    size_t index = hash(symbol, table->capacity);
    hash_table_entry_t* entry = table->symbols[index]; // 
    
    while (entry != NULL) { 
        if (strcmp(entry->key, symbol) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return -1; // Symbol not found
}

bool contains_symbol(symbol_table_t* table, const char* symbol) { // Check if symbol exists in the table
    return lookup_symbol(table, symbol) != -1; // If lookup_symbol returns -1, symbol does not exist
}
        

