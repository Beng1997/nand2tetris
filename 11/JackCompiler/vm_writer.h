#ifndef VM_WRITER_H
#define VM_WRITER_H

#include <string>
#include <fstream>
#include <iostream>

typedef std::string string;

typedef enum {
    SEG_CONSTANT,
    SEG_ARG,
    SEG_LOCAL,
    SEG_STATIC,
    SEG_THIS,
    SEG_THAT,
    SEG_POINTER,
    SEG_TEMP
} segment;

class vm_writer {
public:
    vm_writer(string output_file);
    ~vm_writer();
    void write_push(segment segment, int index);
    void write_pop(segment segment, int index);
    void write_arithmetic(string command);
    void write_label(string label);
    void write_goto(string label);
    void write_if(string label);
    void write_call(string name, int n_args);
    void write_function(string name, int n_locals);
    void write_return();
    void close();
private:
    string output_file;
    std::ofstream outputFile;
    void write_string(string s);
};

#endif
