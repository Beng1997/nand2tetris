#include "compilation_engine.h"


CompilationEngine::CompilationEngine(string inputFilename, string outputFilename) {
    this->tokenizer = new JackTokenizer(inputFilename);
    this->outputFile.open(outputFilename);
    this->symbolTable = new symbol_table();
    this->vmWriter = new vm_writer(outputFilename);
    this->ifLabelCount = 0;
    this->whileLabelCount = 0;
}


CompilationEngine::~CompilationEngine() {
    delete this->tokenizer;
    delete this->symbolTable;
    delete this->vmWriter;
    this->outputFile.close();
}

/**
 * Compiles a complete class
 */
void CompilationEngine::compileClass() {
    this->tokenizer->advance();
    
    // Check for "class" keyword
    if(this->tokenizer->tokenType() != KEYWORD || this->tokenizer->keyword() != "class") {
         throw std::runtime_error("Invalid class: expected 'class'");
    }
    
    // Class Name
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != IDENTIFIER) {
        throw std::runtime_error("Invalid class: expected identifier");
    }
    this->className = this->tokenizer->identifier();

    // '{'
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '{') {
        throw std::runtime_error("Invalid class: expected '{'");
    }

    // Compile Class Var Decs
    this->tokenizer->advance();
    while (this->tokenizer->tokenType() == KEYWORD && 
          (this->tokenizer->keyword() == "static" || this->tokenizer->keyword() == "field")) {
        this->compileClassVarDec(); 
    }
    
    // Compile Subroutines
    while (this->tokenizer->tokenType() == KEYWORD && 
          (this->tokenizer->keyword() == "constructor" || this->tokenizer->keyword() == "function" || this->tokenizer->keyword() == "method")) {
        this->compileSubroutineDec();
    }

    // '}'
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '}') {
        throw std::runtime_error("Invalid class: expected '}'");
    }
    // No more tokens to advance to at class level end usually, but we can advance past '}'
    if (this->tokenizer->hasMoreTokens()) {
        this->tokenizer->advance();
    }
}

void CompilationEngine::compileClassVarDec() {
    // Current token is "static" or "field"
    string k_str = this->tokenizer->keyword();
    kind k = (k_str == "static") ? STATIC : FIELD;
    
    // Type (int, char, boolean, className)
    this->tokenizer->advance();
    string type;
    if (this->tokenizer->tokenType() == KEYWORD) {
        type = this->tokenizer->keyword();
    } else if (this->tokenizer->tokenType() == IDENTIFIER) {
        type = this->tokenizer->identifier();
    } else {
        throw std::runtime_error("Invalid classVarDec type");
    }

    // Variable Name
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != IDENTIFIER) { 
        throw std::runtime_error("Invalid classVarDec name");
    }
    string name = this->tokenizer->identifier();
    
    // Define in symbol table
    this->symbolTable->define(name, type, k);
    
    // Handle multiple variables: , var2, var3
    this->tokenizer->advance();
    while(this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == ',') {
        this->tokenizer->advance();
        if(this->tokenizer->tokenType() != IDENTIFIER ) {
             throw std::runtime_error("Invalid classVarDec definition");
        }
        this->symbolTable->define(this->tokenizer->identifier(), type, k);
        this->tokenizer->advance();
    }

    // ';'
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ';') {
        throw std::runtime_error("Invalid classVarDec: expected ';'");
    }
    
    this->tokenizer->advance(); 
}


void CompilationEngine::compileSubroutineDec() {
    this->symbolTable->start_subroutine();
    // Current token is "constructor", "function", or "method"
    string subroutineType = this->tokenizer->keyword();
    
    if (subroutineType == "method") {
        this->symbolTable->define("this", this->className, ARG);
    }

    // Type (void, int, char, boolean, className)
    this->tokenizer->advance();
    // (We don't need to store the return type for now, but we advance past it)

    // Subroutine Name
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != IDENTIFIER) { 
        throw std::runtime_error("Invalid subroutine name");
    }
    string subroutineName = this->tokenizer->identifier();
    
    // '('
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '(') { 
        throw std::runtime_error("Invalid subroutine: expected '('");
    }
    
    this->compileParameterList();
    
    // compileParameterList leaves current token at ')'
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') { 
        throw std::runtime_error("Expected ')' after parameter list");
    }
    
    this->tokenizer->advance(); // Move past ')' before body
    
    // The actual VM function command is written inside compileSubroutineBody 
    // because we need to know the number of local variables first.
    this->compileSubroutineBody(subroutineType, subroutineName); 
}

void CompilationEngine::compileParameterList() {
    this->tokenizer->advance(); // Move past '('
    
    while (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') {
        // Type
        string type;
        if (this->tokenizer->tokenType() == KEYWORD) {
            type = this->tokenizer->keyword();
        } else if (this->tokenizer->tokenType() == IDENTIFIER) {
            type = this->tokenizer->identifier();
        }
        
        // Variable name
        this->tokenizer->advance();
        string name = this->tokenizer->identifier();
        
        this->symbolTable->define(name, type, ARG);
        
        this->tokenizer->advance();
        
        // Check for comma
        if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == ',') {
            this->tokenizer->advance();
        }
    }
}

void CompilationEngine::compileSubroutineBody(string subroutineType, string subroutineName) {
    // Current token should be '{'
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '{') { 
        throw std::runtime_error("Invalid subroutine body: expected '{'");
    }
    
    this->tokenizer->advance(); // Move past '{'
    
    // Handle var declarations
    while (this->tokenizer->tokenType() == KEYWORD && this->tokenizer->keyword() == "var") {
        this->compileVarDec();
    }
    
    // Now write the VM function command
    int nLocals = this->symbolTable->var_count(VAR);
    this->vmWriter->write_function(this->className + "." + subroutineName, nLocals);
    
    // Initialization code
    if (subroutineType == "constructor") {
        // Allocate memory for fields
        int nFields = this->symbolTable->var_count(FIELD);
        this->vmWriter->write_push(SEG_CONSTANT, nFields);
        this->vmWriter->write_call("Memory.alloc", 1);
        this->vmWriter->write_pop(SEG_POINTER, 0);
    } else if (subroutineType == "method") {
        // Set 'this' pointer
        this->vmWriter->write_push(SEG_ARG, 0);
        this->vmWriter->write_pop(SEG_POINTER, 0);
    }

    // Handle statements
    this->compileStatements();
    
    // Current token should be '}'
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '}') { 
        throw std::runtime_error("Invalid subroutine body: expected '}'");
    }
    
    this->tokenizer->advance(); // Move past '}'
}

void CompilationEngine::compileVarDec() {
    // Current token is "var"
    this->tokenizer->advance();
    
    // Type
    string type;
    if (this->tokenizer->tokenType() == KEYWORD) {
        type = this->tokenizer->keyword();
    } else if (this->tokenizer->tokenType() == IDENTIFIER) {
        type = this->tokenizer->identifier();
    }
    
    // Variable Name
    this->tokenizer->advance();
    string name = this->tokenizer->identifier();
    this->symbolTable->define(name, type, VAR);
    
    // Handle multiple variables
    this->tokenizer->advance();
    while (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == ',') {
        this->tokenizer->advance();
        this->symbolTable->define(this->tokenizer->identifier(), type, VAR);
        this->tokenizer->advance();
    }
    
    // Semicolon
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ';') { 
        throw std::runtime_error("Invalid varDec: expected ';'");
    }
    
    this->tokenizer->advance(); 
}


void CompilationEngine::compileStatements() {
    while (this->tokenizer->tokenType() == KEYWORD && 
           (this->tokenizer->keyword() == "let" || this->tokenizer->keyword() == "if" || 
            this->tokenizer->keyword() == "while" || this->tokenizer->keyword() == "do" || 
            this->tokenizer->keyword() == "return")) {
        if (this->tokenizer->keyword() == "let") {
            this->compileLet();
        } else if (this->tokenizer->keyword() == "if") {
            this->compileIf();
        } else if (this->tokenizer->keyword() == "while") {
            this->compileWhile();
        } else if (this->tokenizer->keyword() == "do") {
            this->compileDo();
        } else if (this->tokenizer->keyword() == "return") {
            this->compileReturn();
        }
    }
}



void CompilationEngine::compileLet() {
    this->tokenizer->advance(); // Move past 'let'
    
    // Variable name
    string name = this->tokenizer->identifier();
    this->tokenizer->advance();
    
    // std::cout << "Let lookup: " << name << std::endl;
    kind k = this->symbolTable->kind_of(name);
    int index = this->symbolTable->index_of(name);
    segment seg;
    switch (k) {
        case STATIC: seg = SEG_STATIC; break;
        case FIELD:  seg = SEG_THIS; break;
        case VAR:    seg = SEG_LOCAL; break;
        case ARG:    seg = SEG_ARG; break;
        default:     throw std::runtime_error("Variable not found: " + name);
    }

    bool isArray = false;
    // Optional array index [expression]
    if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == '[') { 
        isArray = true;
        this->vmWriter->write_push(seg, index);
        this->tokenizer->advance();
        this->compileExpression();
        this->vmWriter->write_arithmetic("add");
        // compileExpression leaves token at ']'
        this->tokenizer->advance(); // Move past ']'
    }
    
    // '='
    this->tokenizer->advance(); // Move past '='
    this->compileExpression();
    
    if (isArray) {
        this->vmWriter->write_pop(SEG_TEMP, 0); // RHS result
        this->vmWriter->write_pop(SEG_POINTER, 1); // Destination address
        this->vmWriter->write_push(SEG_TEMP, 0);
        this->vmWriter->write_pop(SEG_THAT, 0);
    } else {
        this->vmWriter->write_pop(seg, index);
    }
    
    this->tokenizer->advance(); // Move past ';'
}


void CompilationEngine::compileIf() {
    int labelIndex = this->ifLabelCount++;
    string falseLabel = "IF_FALSE" + std::to_string(labelIndex);
    string endLabel = "IF_END" + std::to_string(labelIndex);
    
    this->tokenizer->advance(); // Move past 'if'
    this->tokenizer->advance(); // Move past '('
    this->compileExpression();
    this->tokenizer->advance(); // Move past ')'
    
    this->vmWriter->write_arithmetic("not");
    this->vmWriter->write_if(falseLabel);
    
    this->tokenizer->advance(); // Move past '{'
    this->compileStatements();
    this->tokenizer->advance(); // Move past '}'
    
    this->vmWriter->write_goto(endLabel);
    this->vmWriter->write_label(falseLabel);
    
    if (this->tokenizer->tokenType() == KEYWORD && this->tokenizer->keyword() == "else") { 
        this->tokenizer->advance(); // Move past 'else'
        this->tokenizer->advance(); // Move past '{'
        this->compileStatements();
        this->tokenizer->advance(); // Move past '}'
    }
    
    this->vmWriter->write_label(endLabel);
}

void CompilationEngine::compileWhile() {
    int labelIndex = this->whileLabelCount++;
    string startLabel = "WHILE_EXP" + std::to_string(labelIndex);
    string endLabel = "WHILE_END" + std::to_string(labelIndex);
    
    this->vmWriter->write_label(startLabel);
    
    this->tokenizer->advance(); // Move past 'while'
    this->tokenizer->advance(); // Move past '('
    this->compileExpression();
    this->tokenizer->advance(); // Move past ')'
    
    this->vmWriter->write_arithmetic("not");
    this->vmWriter->write_if(endLabel);
    
    this->tokenizer->advance(); // Move past '{'
    this->compileStatements();
    this->tokenizer->advance(); // Move past '}'
    
    this->vmWriter->write_goto(startLabel);
    this->vmWriter->write_label(endLabel);
}
    
void CompilationEngine::compileDo() {
    this->tokenizer->advance(); // Move past 'do'
    
    // Subroutine Call
    string name = this->tokenizer->identifier();
    this->tokenizer->advance();
    
    int nArgs = 0;
    if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == '.') {
        this->tokenizer->advance(); // Move past '.'
        string subName = this->tokenizer->identifier();
        this->tokenizer->advance();
        
        // Is it a variable (method call) or a class (static call)?
        kind k = this->symbolTable->kind_of(name);
        if (k != NONE) {
            // Method call: push variable as first argument
            segment seg;
            switch (k) {
                case STATIC: seg = SEG_STATIC; break;
                case FIELD:  seg = SEG_THIS; break;
                case VAR:    seg = SEG_LOCAL; break;
                case ARG:    seg = SEG_ARG; break;
                default: seg = SEG_TEMP; break; // Should not happen for methods
            }
            this->vmWriter->write_push(seg, this->symbolTable->index_of(name));
            name = this->symbolTable->type_of(name) + "." + subName;
            nArgs = 1;
        } else {
            // Static call
            name = name + "." + subName;
        }
    } else {
        // Direct call (method of current class)
        this->vmWriter->write_push(SEG_POINTER, 0);
        name = this->className + "." + name;
        nArgs = 1;
    }
    
    this->tokenizer->advance(); // Move past '('
    nArgs += this->compileExpressionList();
    this->tokenizer->advance(); // Move past ')'
    
    this->vmWriter->write_call(name, nArgs);
    this->vmWriter->write_pop(SEG_TEMP, 0); // discard return value
    
    this->tokenizer->advance(); // Move past ';'
}

void CompilationEngine::compileReturn() {
    this->tokenizer->advance(); // Move past 'return'
    
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ';') { 
        this->compileExpression();
    } else {
        this->vmWriter->write_push(SEG_CONSTANT, 0);
    }
    
    this->vmWriter->write_return();
    this->tokenizer->advance(); // Move past ';'
}


void CompilationEngine::compileExpression() {
    this->compileTerm();
    
    while (this->tokenizer->tokenType() == SYMBOL && 
    (this->tokenizer->symbol() == '+' || this->tokenizer->symbol() == '-' 
    || this->tokenizer->symbol() == '*' || this->tokenizer->symbol() == '/' 
    || this->tokenizer->symbol() == '&' || this->tokenizer->symbol() == '|' 
    || this->tokenizer->symbol() == '<' || this->tokenizer->symbol() == '>' 
    || this->tokenizer->symbol() == '=')) { 
        char sym = this->tokenizer->symbol();
        this->tokenizer->advance();
        this->compileTerm();
        
        switch (sym) {
            case '+': this->vmWriter->write_arithmetic("add"); break;
            case '-': this->vmWriter->write_arithmetic("sub"); break;
            case '*': this->vmWriter->write_call("Math.multiply", 2); break;
            case '/': this->vmWriter->write_call("Math.divide", 2); break;
            case '&': this->vmWriter->write_arithmetic("and"); break;
            case '|': this->vmWriter->write_arithmetic("or"); break;
            case '<': this->vmWriter->write_arithmetic("lt"); break;
            case '>': this->vmWriter->write_arithmetic("gt"); break;
            case '=': this->vmWriter->write_arithmetic("eq"); break;
        }
    }
}

int CompilationEngine::compileExpressionList() {
    int count = 0;
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') {
        this->compileExpression();
        count++;
        while (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == ',') {
            this->tokenizer->advance();
            this->compileExpression();
            count++;
        }
    }
    return count;
}

void CompilationEngine::compileTerm() {
    // Integer Constant
    if (this->tokenizer->tokenType() == INT_CONST) { 
        this->vmWriter->write_push(SEG_CONSTANT, this->tokenizer->intVal());
        this->tokenizer->advance();
    }
    // String Constant
    else if (this->tokenizer->tokenType() == STRING_CONST) { 
        string s = this->tokenizer->stringVal();
        this->vmWriter->write_push(SEG_CONSTANT, s.length());
        this->vmWriter->write_call("String.new", 1);
        for (char c : s) {
            this->vmWriter->write_push(SEG_CONSTANT, (int)c);
            this->vmWriter->write_call("String.appendChar", 2);
        }
        this->tokenizer->advance();
    }
    // Keyword Constant (true, false, null, this)
    else if (this->tokenizer->tokenType() == KEYWORD) {
        string k = this->tokenizer->keyword();
        if (k == "true") {
            this->vmWriter->write_push(SEG_CONSTANT, 0);
            this->vmWriter->write_arithmetic("not");
        } else if (k == "false" || k == "null") {
            this->vmWriter->write_push(SEG_CONSTANT, 0);
        } else if (k == "this") {
            this->vmWriter->write_push(SEG_POINTER, 0);
        }
        this->tokenizer->advance();
    }
    // Parenthesized Expression: (expression)
    else if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == '(') {
         this->tokenizer->advance();
         this->compileExpression();
         this->tokenizer->advance(); // Move past ')'
    }
    // Unary Op: - term OR ~ term
    else if (this->tokenizer->tokenType() == SYMBOL && (this->tokenizer->symbol() == '-' || this->tokenizer->symbol() == '~')) {
        char op = this->tokenizer->symbol();
        this->tokenizer->advance();
        this->compileTerm();
        if (op == '-') this->vmWriter->write_arithmetic("neg");
        else this->vmWriter->write_arithmetic("not");
    }
    // Identifiers: varName, varName[expr], subroutineCall
    else if (this->tokenizer->tokenType() == IDENTIFIER) { 
        string name = this->tokenizer->identifier();
        this->tokenizer->advance();
        
        // Case 1: Array Access [
        if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == '[') {
            kind k = this->symbolTable->kind_of(name);
            int index = this->symbolTable->index_of(name);
            segment seg;
            switch (k) {
                case STATIC: seg = SEG_STATIC; break;
                case FIELD:  seg = SEG_THIS; break;
                case VAR:    seg = SEG_LOCAL; break;
                case ARG:    seg = SEG_ARG; break;
                default: seg = SEG_TEMP; break; // Should not happen for methods
            }
            this->vmWriter->write_push(seg, index);
            this->tokenizer->advance();
            this->compileExpression();
            this->vmWriter->write_arithmetic("add");
            this->tokenizer->advance(); // Move past ']'
            this->vmWriter->write_pop(SEG_POINTER, 1);
            this->vmWriter->write_push(SEG_THAT, 0);
        }
        // Case 2: Subroutine Call . (method) or ( (function)
        else if (this->tokenizer->tokenType() == SYMBOL && (this->tokenizer->symbol() == '.' || this->tokenizer->symbol() == '(')) {
             int nArgs = 0;
             if (this->tokenizer->symbol() == '.') {
                 this->tokenizer->advance(); // Move past '.'
                 string subName = this->tokenizer->identifier();
                 this->tokenizer->advance();
                 
                 kind k = this->symbolTable->kind_of(name);
                 if (k != NONE) {
                     segment seg;
                     switch (k) {
                         case STATIC: seg = SEG_STATIC; break;
                         case FIELD:  seg = SEG_THIS; break;
                         case VAR:    seg = SEG_LOCAL; break;
                         case ARG:    seg = SEG_ARG; break;
                         default: seg = SEG_TEMP; break;
                     }
                     this->vmWriter->write_push(seg, this->symbolTable->index_of(name));
                     name = this->symbolTable->type_of(name) + "." + subName;
                     nArgs = 1;
                 } else {
                     name = name + "." + subName;
                 }
             } else {
                 // Direct call
                 this->vmWriter->write_push(SEG_POINTER, 0);
                 name = this->className + "." + name;
                 nArgs = 1;
             }
             
             this->tokenizer->advance(); // Move past '('
             nArgs += this->compileExpressionList();
             this->tokenizer->advance(); // Move past ')'
             this->vmWriter->write_call(name, nArgs);
        }
        // Case 3: Just a variable name
        else {
             kind k = this->symbolTable->kind_of(name);
             int index = this->symbolTable->index_of(name);
             segment seg;
             switch (k) {
                 case STATIC: seg = SEG_STATIC; break;
                 case FIELD:  seg = SEG_THIS; break;
                 case VAR:    seg = SEG_LOCAL; break;
                 case ARG:    seg = SEG_ARG; break;
                 default: seg = SEG_TEMP; break;
             }
             this->vmWriter->write_push(seg, index);
        }
    }
}
