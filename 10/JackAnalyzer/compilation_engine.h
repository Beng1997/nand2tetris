#ifndef COMPILE_ENGINE_H
#define COMPILE_ENGINE_H

#include <string>
#include <fstream>
#include <iostream>
#include "jacktokenizer.h"

class CompilationEngine {
    private:
        std::ofstream outputFile;
        JackTokenizer* tokenizer;
    public:
        CompilationEngine(string inputFilename, string outputFilename);
        ~CompilationEngine();
        void compileClass();
        void compileClassVarDec();
        void compileSubroutineDec();
        void compileParameterList();
        void compileSubroutineBody();
        void compileVarDec();
        void compileStatements();
        void compileLet();
        void compileIf();
        void compileWhile();
        void compileDo();
        void compileReturn();
        void compileExpression();
        void compileTerm();
        void compileExpressionList();
};
        
#endif
        
    