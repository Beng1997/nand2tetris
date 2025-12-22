#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

typedef std::string string;

typedef enum {
    FIELD,
    STATIC,
    VAR,
    ARG,
    NONE
} kind;

class table_entry {
    public:
        string type;
        kind k;
        int index;
        table_entry(string type, kind k, int index);
        table_entry();
};

class symbol_table {
    private:
        std::unordered_map<string, table_entry> classMap;
        std::unordered_map<string, table_entry> subroutineMap;
        int staticCount;
        int fieldCount;
        int varCount;
        int argCount;

    public:
        symbol_table();
        ~symbol_table();
        
        void start_subroutine();
        void define(string name, string type, kind k);
        int var_count(kind k);
        kind kind_of(string name);
        string type_of(string name);
        int index_of(string name);
};

#endif
