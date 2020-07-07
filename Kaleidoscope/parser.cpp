#include "/LLVM/llvm-install/include/llvm/ADT/STLExtras.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;


enum Token{
    tok_eof = -1,

    //commands
    tok_def = -2,
    tok_extern = -3,

    //primary
    tok_identifier = -4,
    tok_number = -5
};

static std::string IdentifierStr;   //Filled in if tok_identifier
static double NumVal;   //Filled in if tok_number


//Lexer
static int gettok() {
    static int LastChar = ' ';

    while(isspace(LastChar))   //skip whitespace
        LastChar = getchar();


    if(isalpha(LastChar)) {    // identifier: [a-zA-Z][a-ZA-Z0-9]*
        IdentifierStr = LastChar;
        while(isalnum(LastChar = getchar())) 
            IdentifierStr += LastChar;

        if(IdentifierStr == "def")
            return tok_def;
        if(IdentifierStr == "extern")
            return tok_extern;
        return tok_identifier; 
    }


    if(isdigit(LastChar) || LastChar == '.') {   // Number: [0-9,]+ 
        std::string NumStr;
        do{
            NumStr += LastChar;
            LastChar = getchar(); 
        }while(isdigit(LastChar) || LastChar == '.'); 
    
        NumStr = strtod(NumStr.c_str(), nullptr); 
        return tok_number;
    }


    if(LastChar == '#') {        //Comment until end of line
        do 
           LastChar = getchar();
        while(LastChar != EOF && LastChar != '\n' && LastChar != '\r'); 
    
        if(LastChar != EOF)
            return gettok();
    }


    //Check for end of file. Don't eat the EOF.
    if(LastChar == EOF)
        return tok_eof;

    //Otherwise, just return the character as its ascii value.
    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;

}

