#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include <string>
#include <stdbool.h>
#include <iostream>
#include <fstream>

typedef std::string string;

enum TokenType {
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST
};




class JackTokenizer {
    private:
        string line;
        string currentToken;
        int charindex;
        std::ifstream inputFile;
        
    public:
        JackTokenizer(string filename);
        bool hasMoreTokens();
        void advance();
        TokenType tokenType();
        string keyword();
        char symbol();
        string identifier();
        int intVal();
        string stringVal();
        ~JackTokenizer();
};


string is_keyword(string token);
char is_symbol(char c);
string is_identifier(string token);
int is_int(string token);
string is_string(string token);


#endif






