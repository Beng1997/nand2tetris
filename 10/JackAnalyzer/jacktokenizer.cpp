#include "jacktokenizer.h"
#include <fstream>


JackTokenizer::JackTokenizer(string filename) {
    this->inputFile.open(filename);
    this->charindex = 0;   
    this->line = "";
    this->currentToken = "";
}
JackTokenizer::~JackTokenizer() {
    this->inputFile.close();
}
bool JackTokenizer::hasMoreTokens() {
    // Basic check - advance() handles the actual finding of the next token
    return !this->inputFile.eof() || this->charindex < this->line.length();
}

void JackTokenizer::advance() {
    this->currentToken = "";
    
    // Loop until we find a token or run out of input
    while (true) {
        if (this->charindex >= this->line.length()) {
            if (!std::getline(this->inputFile, this->line)) {
                return; // End of file
            }
            this->charindex = 0;
            continue; // Re-check the length condition in case of empty line
        }

        char c = this->line[this->charindex]; //get the current character
        
        // Skip whitespace
        if (isspace(c)) {
            this->charindex++;
            continue;
        }

        // Handle Single-line Comments (//) - Skip rest of line
        if (c == '/' && this->charindex + 1 < this->line.length() && this->line[this->charindex+1] == '/') {
            this->charindex = this->line.length(); // Skip to end of line
            continue;
        }

        // Handle Block Comments (/* ... */ and /** ... */)
        if (c == '/' && this->charindex + 1 < this->line.length() && this->line[this->charindex+1] == '*') {
            this->charindex += 2; // Skip /*
            // Find closing */
            while (true) {
                // Check if we need to read more lines
                while (this->charindex >= this->line.length()) {
                    if (!std::getline(this->inputFile, this->line)) {
                        return; // End of file inside comment
                    }
                    this->charindex = 0;
                }
                // Look for */
                if (this->line[this->charindex] == '*' && 
                    this->charindex + 1 < this->line.length() && 
                    this->line[this->charindex + 1] == '/') {
                    this->charindex += 2; // Skip */
                    break;
                }
                this->charindex++;
            }
            continue;
        }

        // Handle Symbols
        if (is_symbol(c) != '\0') {
            this->currentToken = string(1, c);
            this->charindex++;
            return;
        }

        // Handle String Constants
        if (c == '"') {
             // Find closing quote
             size_t endQuote = this->line.find('"', this->charindex + 1);
             if (endQuote != string::npos) {//if we find a closing quote
                 this->currentToken = this->line.substr(this->charindex, endQuote - this->charindex + 1);//get the string constant
                 this->charindex = endQuote + 1;
                 return;
             }
        }

        // Handle Integer Constants, Keywords, Identifiers
        // Read until next delimiter (space or symbol)
        int start = this->charindex;
        while (this->charindex < this->line.length()) {
            char nextC = this->line[this->charindex];
            if (isspace(nextC) || is_symbol(nextC) != '\0') {
                break;
            }
            this->charindex++;
        }
        this->currentToken = this->line.substr(start, this->charindex - start);//get the identifier
        return;
    }
}

TokenType JackTokenizer::tokenType() {
    if (this->currentToken.empty()) {
        // Should not happen if advance is valid and checked hasMoreTokens
        return SYMBOL; 
    }

    if (is_symbol(this->currentToken[0]) != '\0' && this->currentToken.length() == 1) {
        return SYMBOL;
    }
    if (!is_keyword(this->currentToken).empty()) {
        return KEYWORD;
    }
    if (is_int(this->currentToken) != -1) {
        return INT_CONST;
    }
    if (!is_string(this->currentToken).empty()) {
        return STRING_CONST;
    }
    return IDENTIFIER;
}

string JackTokenizer::keyword() {
    return this->currentToken;
}

char JackTokenizer::symbol() {
    return this->currentToken[0];
}

string JackTokenizer::identifier() {
    return this->currentToken;
}

int JackTokenizer::intVal() {
    return std::stoi(this->currentToken);
}

string JackTokenizer::stringVal() {
    // Remove quotes
    return this->currentToken.substr(1, this->currentToken.length() - 2);
}


string is_keyword(string token) { //note: its only called when tokenType() returns KEYWORD
    if (token == "class") return "class";
    if (token == "method") return "method";
    if (token == "function") return "function";
    if (token == "constructor") return "constructor";
    if (token == "int") return "int";
    if (token == "char") return "char";
    if (token == "boolean") return "boolean";
    if (token == "void") return "void";
    if (token == "var") return "var";
    if (token == "static") return "static";
    if (token == "field") return "field";
    if (token == "let") return "let";
    if (token == "do") return "do";
    if (token == "if") return "if";
    if (token == "else") return "else";
    if (token == "while") return "while";
    if (token == "return") return "return";
    if (token == "true") return "true";
    if (token == "false") return "false";
    if (token == "null") return "null";
    if (token == "this") return "this";
    return "";
}

char is_symbol(char c) { //note: its only called when tokenType() returns SYMBOL
    switch (c) {
        case '{':
            return '{';
        case '}':
            return '}';
        case '(':
            return '(';
        case ')':
            return ')';
        case '[':
            return '[';
        case ']':
            return ']';
        case '.':
            return '.';
        case ',':
            return ',';
        case ';':
            return ';';
        case '+':
            return '+';
        case '-':
            return '-';
        case '*':
            return '*';
        case '/':
            return '/';
        case '&':
            return '&';
        case '|':
            return '|';
        case '<':
            return '<';
        case '>':
            return '>';
        case '=':
            return '=';
        case '~':
            return '~';
        default:
            return '\0';
    }   
}
            
string is_identifier(string token) { //note: its only called when tokenType() returns IDENTIFIER
    if (token[0] >= 'a' && token[0] <= 'z') {
        return token;
    } else {
        return "";
    }
}

int is_int(string token) { //note: its only called when tokenType() returns INT_CONST
    if (token[0] >= '0' && token[0] <= '9') {
        return std::stoi(token); //built-in function to convert string to integer
    } else {
        return -1;
    }
}

string is_string(string token) { //note: its only called when tokenType() returns STRING_CONST
    if (token[0] == '"' && token[token.length() - 1] == '"') { 
        return token;
    } else {
        return "";
    }
}


    
