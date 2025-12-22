#include "vm_writer.h"
#include <fstream>

vm_writer::vm_writer(string output_filename) {
    this->output_file = output_filename;
    this->outputFile.open(output_filename);
}

vm_writer::~vm_writer() {
    if (this->outputFile.is_open()) {
        this->outputFile.close();
    }
}


void vm_writer::write_push(segment seg, int index) {
    string s;
    switch (seg) {
        case SEG_CONSTANT: s = "constant"; break;
        case SEG_ARG:      s = "argument"; break;
        case SEG_LOCAL:    s = "local"; break;
        case SEG_STATIC:   s = "static"; break;
        case SEG_THIS:     s = "this"; break;
        case SEG_THAT:     s = "that"; break;
        case SEG_POINTER:  s = "pointer"; break;
        case SEG_TEMP:     s = "temp"; break;
    }
    this->outputFile << "push " << s << " " << index << "\n";
}

void vm_writer::write_pop(segment seg, int index) {
    string s;
    switch (seg) {
        case SEG_CONSTANT: s = "constant"; break;
        case SEG_ARG:      s = "argument"; break;
        case SEG_LOCAL:    s = "local"; break;
        case SEG_STATIC:   s = "static"; break;
        case SEG_THIS:     s = "this"; break;
        case SEG_THAT:     s = "that"; break;
        case SEG_POINTER:  s = "pointer"; break;
        case SEG_TEMP:     s = "temp"; break;
    }
    this->outputFile << "pop " << s << " " << index << "\n";
}

void vm_writer::write_arithmetic(string command) {
    this->outputFile << command << "\n";
}

void vm_writer::write_label(string label) {
    this->outputFile << "label " << label << "\n";
}

void vm_writer::write_goto(string label) {
    this->outputFile << "goto " << label << "\n";
}

void vm_writer::write_if(string label) {
    this->outputFile << "if-goto " << label << "\n";
}

void vm_writer::write_call(string name, int n_args) {
    this->outputFile << "call " << name << " " << n_args << "\n";
}

void vm_writer::write_function(string name, int n_locals) {
    this->outputFile << "function " << name << " " << n_locals << "\n";
}

void vm_writer::write_return() {
    this->outputFile << "return\n";
}

void vm_writer::close() {
    this->outputFile.close();
}

void vm_writer::write_string(string s) {
    this->outputFile << s << "\n";
}
