#include "symbol_table.h"
#include <iostream>

table_entry::table_entry(string type, kind k, int index) {
    this->type = type;
    this->k = k;
    this->index = index;
}

table_entry::table_entry() {
    this->type = "";
    this->k = NONE;
    this->index = -1;
}

symbol_table::symbol_table() {
    this->staticCount = 0;
    this->fieldCount = 0;
    this->varCount = 0;
    this->argCount = 0;
}

symbol_table::~symbol_table() {
}

void symbol_table::start_subroutine() {
    this->subroutineMap.clear();
    this->varCount = 0;
    this->argCount = 0;
}

void symbol_table::define(string name, string type, kind k) {
    int index = 0;
    switch (k) {
        case STATIC: 
            index = staticCount++; 
            classMap[name] = table_entry(type, k, index);
            break;
        case FIELD:  
            index = fieldCount++; 
            classMap[name] = table_entry(type, k, index);
            break;
        case VAR:    
            index = varCount++; 
            subroutineMap[name] = table_entry(type, k, index);
            break;
        case ARG:    
            index = argCount++; 
            subroutineMap[name] = table_entry(type, k, index);
            break;
        default:     
            break;
    }
}

int symbol_table::var_count(kind k) {
    switch (k) {
        case STATIC: return staticCount;
        case FIELD:  return fieldCount;
        case VAR:    return varCount;
        case ARG:    return argCount;
        default:     return 0;
    }
}

kind symbol_table::kind_of(string name) {
    auto it = subroutineMap.find(name);
    if (it != subroutineMap.end()) {
        return it->second.k;
    }
    it = classMap.find(name);
    if (it != classMap.end()) {
        return it->second.k;
    }
    return NONE;
}

string symbol_table::type_of(string name) {
    auto it = subroutineMap.find(name);
    if (it != subroutineMap.end()) {
        return it->second.type;
    }
    it = classMap.find(name);
    if (it != classMap.end()) {
        return it->second.type;
    }
    return "";
}

int symbol_table::index_of(string name) {
    auto it = subroutineMap.find(name);
    if (it != subroutineMap.end()) {
        return it->second.index;
    }
    it = classMap.find(name);
    if (it != classMap.end()) {
        return it->second.index;
    }
    return -1;
}

