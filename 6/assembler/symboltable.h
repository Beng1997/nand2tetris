#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdbool.h>
#include <string.h>

typedef struct hash_table_entry { 
    char* key;
    int value;
    struct hash_table_entry* next;
} hash_table_entry_t;

typedef struct symbol_table {
    size_t count;
    hash_table_entry_t** symbols;
    size_t capacity;
} symbol_table_t;



void init_symbol_table(symbol_table_t* table, size_t initial_capacity);

void init_predefined_symbols(symbol_table_t* table);

int get_predefined_value(const char* symbol);

void free_symbol_table(symbol_table_t* table);

bool is_predefined_symbol(const char* symbol);

bool is_label_symbol(const char* symbol);

bool is_variable_symbol(const char* symbol);

void add_symbol(symbol_table_t* table, const char* symbol, int value);

int lookup_symbol(symbol_table_t* table, const char* symbol);

bool contains_symbol(symbol_table_t* table, const char* symbol);
#endif // SYMBOLTABLE_H