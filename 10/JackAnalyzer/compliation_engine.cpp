#include "compilation_engine.h"


CompilationEngine::CompilationEngine(string inputFilename, string outputFilename) {
    this->tokenizer = new JackTokenizer(inputFilename);
    this->outputFile.open(outputFilename);
}

CompilationEngine::~CompilationEngine() {
    delete this->tokenizer;
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
    this->outputFile << "<class>\n";
    this->outputFile << "<keyword> class </keyword>\n";
    
    // Class Name
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != IDENTIFIER) {
        throw std::runtime_error("Invalid class: expected identifier");
    }
    this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";

    // '{'
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '{') {
        throw std::runtime_error("Invalid class: expected '{'");
    }
    this->outputFile << "<symbol> { </symbol>\n";

    // Compile Class Var Decs and Subroutines
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
    this->outputFile << "<symbol> } </symbol>\n";
    this->outputFile << "</class>\n";
}

void CompilationEngine::compileClassVarDec() {
    // We enter here knowing the current token is "static" or "field"
    this->outputFile << "<classVarDec>\n";
    
    // static/field
    this->outputFile << "<keyword> " << this->tokenizer->keyword() << " </keyword>\n";
    
    // Type (int, char, boolean, className)
    this->tokenizer->advance();
    if (this->tokenizer->tokenType() == KEYWORD && (this->tokenizer->keyword() == "int" || this->tokenizer->keyword() == "char" || this->tokenizer->keyword() == "boolean")) {
        this->outputFile << "<keyword> " << this->tokenizer->keyword() << " </keyword>\n";
    } else if (this->tokenizer->tokenType() == IDENTIFIER) {
         this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
    } else {
        throw std::runtime_error("Invalid classVarDec type");
    }

    // Variable Name
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != IDENTIFIER) { 
        throw std::runtime_error("Invalid classVarDec name");
    }
    this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
    
    // Handle multiple variables: , var2, var3
    this->tokenizer->advance();
    while(this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == ',') {
        this->outputFile << "<symbol> , </symbol>\n";
        
        this->tokenizer->advance();
        if(this->tokenizer->tokenType() != IDENTIFIER ) {
             throw std::runtime_error("Invalid classVarDec definition");
        }
        this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
        
        this->tokenizer->advance();
    }

    // ';'
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ';') {
        throw std::runtime_error("Invalid classVarDec: expected ';'");
    }
    this->outputFile << "<symbol> ; </symbol>\n";
    this->outputFile << "</classVarDec>\n";
    
    // Prepare for next check in compileClass loop
    this->tokenizer->advance(); 
}

void CompilationEngine::compileSubroutineDec() {
    //we enter here knowing the current token is "constructor", "function", or "method"
    this->outputFile << "<subroutineDec>\n";
    
    //constructor, function, or method
    this->outputFile << "<keyword> " << this->tokenizer->keyword() << " </keyword>\n";

    //type void or (int, char, boolean, className)
    this->tokenizer->advance();
    if (this->tokenizer->tokenType() == KEYWORD && (this->tokenizer->keyword() == "void" || this->tokenizer->keyword() == "int" || this->tokenizer->keyword() == "char" || this->tokenizer->keyword() == "boolean")) { // we handle here void and int, char, boolean
        this->outputFile << "<keyword> " << this->tokenizer->keyword() << " </keyword>\n";
    } else if (this->tokenizer->tokenType() == IDENTIFIER) { // we handle here className
        this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
    } else {
        throw std::runtime_error("Invalid subroutine type");
    }
    //now lets handle the subroutine name
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != IDENTIFIER) { 
        throw std::runtime_error("Invalid subroutine name");
    }
    this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
    this->tokenizer->advance();
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '(') { 
        throw std::runtime_error("Invalid subroutine name");
    }
    this->outputFile << "<symbol> ( </symbol>\n";
    this->compileParameterList(); //this method will handle the parameter list internally
    // compileParameterList leaves current token at ')'
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') { 
        throw std::runtime_error("Expected ')' after parameter list");
    }
    this->outputFile << "<symbol> ) </symbol>\n";
    this->tokenizer->advance(); // Move past ')' before body
    this->compileSubroutineBody(); //this method will handle the subroutine body internally
    this->outputFile << "</subroutineDec>\n";
}

void CompilationEngine::compileParameterList() {
    // We enter here with current token being '(' - need to advance past it
    this->outputFile << "<parameterList>\n";
    this->tokenizer->advance(); // Move past '('
    
    // Check if there are any parameters (not immediately ')' )
    while (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') {
        // Type (int, char, boolean, or className)
        if (this->tokenizer->tokenType() == KEYWORD) {
            this->outputFile << "<keyword> " << this->tokenizer->keyword() << " </keyword>\n";
        } else if (this->tokenizer->tokenType() == IDENTIFIER) {
            this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
        }
        
        // Variable name
        this->tokenizer->advance();
        if (this->tokenizer->tokenType() == IDENTIFIER) {
            this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
        }
        
        this->tokenizer->advance();
        
        // Check for comma (more parameters)
        if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == ',') {
            this->outputFile << "<symbol> , </symbol>\n";
            this->tokenizer->advance();
        }
    }
    
    this->outputFile << "</parameterList>\n";
    // Current token is now ')' - caller will handle it
}
void CompilationEngine::compileSubroutineBody() {
    // Current token should be '{'
    this->outputFile << "<subroutineBody>\n";
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '{') { 
        throw std::runtime_error("Invalid subroutine body: expected '{'");
    }
    this->outputFile << "<symbol> { </symbol>\n";
    
    this->tokenizer->advance(); // Move past '{'
    
    // Handle var declarations
    while (this->tokenizer->tokenType() == KEYWORD && this->tokenizer->keyword() == "var") {
        this->compileVarDec();
    }
    
    // Handle statements
    this->compileStatements();
    
    if(this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '}') { 
        throw std::runtime_error("Invalid subroutine body: expected '}'");
    }
    this->outputFile << "<symbol> } </symbol>\n";
    this->outputFile << "</subroutineBody>\n";
    
    this->tokenizer->advance(); // Move past '}'
}

void CompilationEngine::compileVarDec() {
    // Current token is "var"
    this->outputFile << "<varDec>\n";
    if (this->tokenizer->tokenType() != KEYWORD || this->tokenizer->keyword() != "var") { 
        throw std::runtime_error("Invalid varDec: expected 'var'");
    }
    this->outputFile << "<keyword> var </keyword>\n";
    
    // Type (int, char, boolean, or className)
    this->tokenizer->advance();
    if (this->tokenizer->tokenType() == KEYWORD && 
        (this->tokenizer->keyword() == "int" || this->tokenizer->keyword() == "char" || this->tokenizer->keyword() == "boolean")) {
        this->outputFile << "<keyword> " << this->tokenizer->keyword() << " </keyword>\n";
    } else if (this->tokenizer->tokenType() == IDENTIFIER) {
        this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
    } else {
        throw std::runtime_error("Invalid varDec: expected type");
    }
    
    // Variable Name
    this->tokenizer->advance();
    if (this->tokenizer->tokenType() != IDENTIFIER) { 
        throw std::runtime_error("Invalid varDec: expected variable name");
    }
    this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
    
    // Handle multiple variables: , var2, var3
    this->tokenizer->advance();
    while (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == ',') {
        this->outputFile << "<symbol> , </symbol>\n";
        
        this->tokenizer->advance();
        if (this->tokenizer->tokenType() != IDENTIFIER) { 
            throw std::runtime_error("Invalid varDec: expected variable name after ','");
        }
        this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
        
        this->tokenizer->advance();
    }
    
    // Semicolon
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ';') { 
        throw std::runtime_error("Invalid varDec: expected ';'");
    }
    this->outputFile << "<symbol> ; </symbol>\n";
    this->outputFile << "</varDec>\n";
    
    this->tokenizer->advance(); // Move past ';' for next var or statement
}

void CompilationEngine::compileStatements() {
    // Current token is "let" or "if" or "while" or "do" or "return"
    this->outputFile << "<statements>\n";
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
    this->outputFile << "</statements>\n";
}    


void CompilationEngine::compileLet() {
    // Current token is "let"
    this->outputFile << "<letStatement>\n";
    if (this->tokenizer->tokenType() != KEYWORD || this->tokenizer->keyword() != "let") { 
        throw std::runtime_error("Invalid letStatement: expected 'let'");
    }
    this->outputFile << "<keyword> let </keyword>\n";
    
    // Variable name
    this->tokenizer->advance();
    if (this->tokenizer->tokenType() != IDENTIFIER) { 
        throw std::runtime_error("Invalid letStatement: expected identifier");
    }
    this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
    
    this->tokenizer->advance();
    
    // Optional array index [expression]
    if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == '[') { 
        this->outputFile << "<symbol> [ </symbol>\n";
        this->tokenizer->advance();
        this->compileExpression();
        // compileExpression leaves token at ']'
        if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ']') { 
            throw std::runtime_error("Invalid letStatement: expected ']'");
        }
        this->outputFile << "<symbol> ] </symbol>\n";
        this->tokenizer->advance();
    }
    
    // '='
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '=') { 
        throw std::runtime_error("Invalid letStatement: expected '='");
    }
    this->outputFile << "<symbol> = </symbol>\n";
    
    this->tokenizer->advance();
    this->compileExpression();
    
    // ';'
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ';') { 
        throw std::runtime_error("Invalid letStatement: expected ';'");
    }
    this->outputFile << "<symbol> ; </symbol>\n";
    this->outputFile << "</letStatement>\n";
    
    this->tokenizer->advance(); // Move past ';'
}

void CompilationEngine::compileIf() {
    this->outputFile << "<ifStatement>\n";
    if (this->tokenizer->tokenType() != KEYWORD || this->tokenizer->keyword() != "if") { 
        throw std::runtime_error("Invalid ifStatement");
    }
    this->outputFile << "<keyword> if </keyword>\n";
    this->tokenizer->advance();
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '(') { 
        throw std::runtime_error("Invalid ifStatement");
    }
    this->outputFile << "<symbol> ( </symbol>\n";
    this->tokenizer->advance();
    this->compileExpression();
    // compileExpression leaves token at ')'
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') { 
        throw std::runtime_error("Invalid ifStatement");
    }
    this->outputFile << "<symbol> ) </symbol>\n";
    this->tokenizer->advance();
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '{') { 
        throw std::runtime_error("Invalid ifStatement");
    }
    this->outputFile << "<symbol> { </symbol>\n";
    this->tokenizer->advance();
    this->compileStatements();
    // compileStatements leaves token at '}'
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '}') { 
        throw std::runtime_error("Invalid ifStatement");
    }
    this->outputFile << "<symbol> } </symbol>\n";
    this->tokenizer->advance();
    // now we can have an else statement or not
    if (this->tokenizer->tokenType() == KEYWORD && this->tokenizer->keyword() == "else") { 
        this->outputFile << "<keyword> else </keyword>\n";
        this->tokenizer->advance();
        if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '{') { 
            throw std::runtime_error("Invalid ifStatement");
        }
        this->outputFile << "<symbol> { </symbol>\n";
        this->tokenizer->advance();
        this->compileStatements();
        // compileStatements leaves token at '}'
        if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '}') { 
            throw std::runtime_error("Invalid ifStatement");
        }
        this->outputFile << "<symbol> } </symbol>\n";
        this->tokenizer->advance();
    }
    this->outputFile << "</ifStatement>\n";
}


void CompilationEngine::compileWhile() {
    this->outputFile << "<whileStatement>\n";
    if (this->tokenizer->tokenType() != KEYWORD || this->tokenizer->keyword() != "while") { 
        throw std::runtime_error("Invalid whileStatement");
    }
    this->outputFile << "<keyword> while </keyword>\n";
    this->tokenizer->advance();
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '(') { 
        throw std::runtime_error("Invalid whileStatement");
    }
    this->outputFile << "<symbol> ( </symbol>\n";
    this->tokenizer->advance();
    this->compileExpression();
    // compileExpression leaves token at ')'
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') { 
        throw std::runtime_error("Invalid whileStatement");
    }
    this->outputFile << "<symbol> ) </symbol>\n";
    this->tokenizer->advance();
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '{') { 
        throw std::runtime_error("Invalid whileStatement");
    }
    this->outputFile << "<symbol> { </symbol>\n";
    this->tokenizer->advance();
    this->compileStatements();
    // compileStatements leaves token at '}'
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '}') { 
        throw std::runtime_error("Invalid whileStatement");
    }
    this->outputFile << "<symbol> } </symbol>\n";
    this->tokenizer->advance();
    this->outputFile << "</whileStatement>\n";
}
    
void CompilationEngine::compileDo() {
    this->outputFile << "<doStatement>\n";
    if (this->tokenizer->tokenType() != KEYWORD || this->tokenizer->keyword() != "do") { 
        throw std::runtime_error("Invalid doStatement");
    }
    this->outputFile << "<keyword> do </keyword>\n";
    this->tokenizer->advance();
    
    // Subroutine Call
    // First identifier (className, varName, or subroutineName)
    if (this->tokenizer->tokenType() != IDENTIFIER) {
         throw std::runtime_error("Invalid doStatement: expected identifier");
    }
    this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
    this->tokenizer->advance();
    
    // Check for '.' (method call) or '(' (direct call)
    if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == '.') {
        this->outputFile << "<symbol> . </symbol>\n";
        this->tokenizer->advance();
        if (this->tokenizer->tokenType() != IDENTIFIER) {
             throw std::runtime_error("Expected method name");
        }
        this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
        this->tokenizer->advance();
    }
    
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '(') {
         throw std::runtime_error("Expected '(' in subroutine call");
    }
    this->outputFile << "<symbol> ( </symbol>\n";
    this->tokenizer->advance();
    
    this->compileExpressionList();
    
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') {
         throw std::runtime_error("Expected ')' in subroutine call");
    }
    this->outputFile << "<symbol> ) </symbol>\n";
    this->tokenizer->advance();
    
    // ';'
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ';') {
         throw std::runtime_error("Invalid doStatement: expected ';'");
    }
    this->outputFile << "<symbol> ; </symbol>\n";
    this->tokenizer->advance(); // Next from ';'
    this->outputFile << "</doStatement>\n";
}

void CompilationEngine::compileReturn() {
    this->outputFile << "<returnStatement>\n";
    if (this->tokenizer->tokenType() != KEYWORD || this->tokenizer->keyword() != "return") { 
        throw std::runtime_error("Invalid returnStatement");
    }
    this->outputFile << "<keyword> return </keyword>\n";
    this->tokenizer->advance();
    //we must check if the next token is a semicolon or not for expression
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ';') { 
        this->compileExpression();
    }
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ';') { 
        throw std::runtime_error("Invalid returnStatement");
    }
    this->outputFile << "<symbol> ; </symbol>\n";
    this->tokenizer->advance();
    this->outputFile << "</returnStatement>\n";
}

void CompilationEngine::compileExpression() {
    this->outputFile << "<expression>\n";
    this->compileTerm();
    
    while (this->tokenizer->tokenType() == SYMBOL && 
    (this->tokenizer->symbol() == '+' || this->tokenizer->symbol() == '-' 
    || this->tokenizer->symbol() == '*' || this->tokenizer->symbol() == '/' 
    || this->tokenizer->symbol() == '&' || this->tokenizer->symbol() == '|' 
    || this->tokenizer->symbol() == '<' || this->tokenizer->symbol() == '>' 
    || this->tokenizer->symbol() == '=')) { 
        char sym = this->tokenizer->symbol();
        this->outputFile << "<symbol> ";
        if (sym == '<') this->outputFile << "&lt;";
        else if (sym == '>') this->outputFile << "&gt;";
        else if (sym == '&') this->outputFile << "&amp;";
        else this->outputFile << sym;
        this->outputFile << " </symbol>\n";
        
        this->tokenizer->advance();
        this->compileTerm();
    }
    this->outputFile << "</expression>\n";
    // Do NOT advance here. Leave the delimiter (e.g. ';', ')', ']') for the caller.
}

void CompilationEngine::compileExpressionList() {
    this->outputFile << "<expressionList>\n";
    
    // Check if we have an expression. 
    // Expressions start with CONSTANTS, IDENTIFIERS, '(', '-', '~'.
    // Faster check: if next is ')', list is empty.
    if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') {
        this->compileExpression();
        
        while (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == ',') {
            this->outputFile << "<symbol> , </symbol>\n";
            this->tokenizer->advance();
            this->compileExpression();
        }
    }
    
    this->outputFile << "</expressionList>\n";
}

void CompilationEngine::compileTerm() {
    this->outputFile << "<term>\n";
    
    // Integer Constant
    if (this->tokenizer->tokenType() == INT_CONST) { 
        this->outputFile << "<integerConstant> " << this->tokenizer->intVal() << " </integerConstant>\n";
        this->tokenizer->advance();
    }
    // String Constant
    else if (this->tokenizer->tokenType() == STRING_CONST) { 
        this->outputFile << "<stringConstant> " << this->tokenizer->stringVal() << " </stringConstant>\n";
        this->tokenizer->advance();
    }
    // Keyword Constant (true, false, null, this)
    else if (this->tokenizer->tokenType() == KEYWORD) {
        this->outputFile << "<keyword> " << this->tokenizer->keyword() << " </keyword>\n";
        this->tokenizer->advance();
    }
    // Parenthesized Expression: (expression)
    else if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == '(') {
         this->outputFile << "<symbol> ( </symbol>\n";
         this->tokenizer->advance();
         this->compileExpression();
         if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') {
             throw std::runtime_error("Expected ')' in term");
         }
         this->outputFile << "<symbol> ) </symbol>\n";
         this->tokenizer->advance();
    }
    // Unary Op: - term OR ~ term
    else if (this->tokenizer->tokenType() == SYMBOL && (this->tokenizer->symbol() == '-' || this->tokenizer->symbol() == '~')) {
        this->outputFile << "<symbol> " << this->tokenizer->symbol() << " </symbol>\n";
        this->tokenizer->advance();
        this->compileTerm();
    }
    // Identifiers: varName, varName[expr], subroutineCall
    else if (this->tokenizer->tokenType() == IDENTIFIER) { 
        // We have an identifier. We need to peek ahead to decide what it is.
        // Since we don't have peek(), we read identifier, advance, and check.
        string prevIdentifier = this->tokenizer->identifier();
        
        this->tokenizer->advance();
        
        // Case 1: Array Access [
        if (this->tokenizer->tokenType() == SYMBOL && this->tokenizer->symbol() == '[') {
            this->outputFile << "<identifier> " << prevIdentifier << " </identifier>\n";
            this->outputFile << "<symbol> [ </symbol>\n";
            this->tokenizer->advance();
            this->compileExpression();
            if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ']') {
                 throw std::runtime_error("Expected ']' in array term");
            }
            this->outputFile << "<symbol> ] </symbol>\n";
            this->tokenizer->advance();
        }
        // Case 2: Subroutine Call . (method) or ( (function)
        else if (this->tokenizer->tokenType() == SYMBOL && (this->tokenizer->symbol() == '.' || this->tokenizer->symbol() == '(')) {
             this->outputFile << "<identifier> " << prevIdentifier << " </identifier>\n";
             if (this->tokenizer->symbol() == '.') {
                 this->outputFile << "<symbol> . </symbol>\n";
                 this->tokenizer->advance();
                 if (this->tokenizer->tokenType() != IDENTIFIER) {
                     throw std::runtime_error("Expected method name");
                 }
                 this->outputFile << "<identifier> " << this->tokenizer->identifier() << " </identifier>\n";
                 this->tokenizer->advance();
             }
             
             if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != '(') {
                  throw std::runtime_error("Expected '(' in subroutine call");
             }
             this->outputFile << "<symbol> ( </symbol>\n";
             this->tokenizer->advance();
             this->compileExpressionList(); // Ends at ')'
             if (this->tokenizer->tokenType() != SYMBOL || this->tokenizer->symbol() != ')') {
                  throw std::runtime_error("Expected ')' in subroutine call");
             }
             this->outputFile << "<symbol> ) </symbol>\n";
             this->tokenizer->advance();
        }
        // Case 3: Just a variable name
        else {
            this->outputFile << "<identifier> " << prevIdentifier << " </identifier>\n";
            // Do NOT advance here, we already advanced to check the symbol. 
            // The current token (which wasn't [ or . or () belongs to the parse flow *after* the term.
        }
    } else {
        throw std::runtime_error("Invalid term start");
    }
    
    this->outputFile << "</term>\n";
}
    
    
