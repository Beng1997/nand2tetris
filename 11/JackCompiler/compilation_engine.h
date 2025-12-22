#ifndef COMPILE_ENGINE_H
#define COMPILE_ENGINE_H

#include <string>
#include <fstream>
#include <iostream>
#include "jacktokenizer.h"
#include "symbol_table.h"
#include "vm_writer.h"

class CompilationEngine {
    private:
        std::ofstream outputFile;
        JackTokenizer* tokenizer;
        symbol_table* symbolTable;
        vm_writer* vmWriter;
        string className;
        int ifLabelCount;
        int whileLabelCount;
    public:


        CompilationEngine(string inputFilename, string outputFilename);
        ~CompilationEngine();
        void compileClass();
        void compileClassVarDec();
        void compileSubroutineDec();
        void compileParameterList();
        void compileSubroutineBody(string subroutineType, string subroutineName);
        void compileVarDec();

        void compileStatements();
        void compileLet();
        void compileIf();
        void compileWhile();
        void compileDo();
        void compileReturn();
        void compileExpression();
        void compileTerm();
        int compileExpressionList();
};



    
        
#endif
        
    