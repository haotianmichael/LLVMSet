#include "/LLVM/llvm-install/include/llvm/ADT/STLExtras.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>



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

namespace {

    // ExprAST - Base class for all expression nodes.
    class ExprAST
    {
        public:
            virtual ~ExprAST () = default;
    };


    // NumberExprAST - Expression class for numeric literals like "1.0".
    class NumberExprAST : public ExprAST
    {
        double Val;
        public:
        NumberExprAST(double Val) : Val(Val) {} 
    };



    //VariableExprAST - Expression class for referencing a variable, like "a".
    class VariableExprAST  : public ExprAST
    {
        std::string Name;
        public:
        VariableExprAST (const std::string &Name) : Name(Name){}
    };


    // BinaryExprAST - Expression class for a binnary operator.
    class BinaryExprAST: public ExprAST
    {
        char Op;
        std::unique_ptr<ExprAST> LHS, RHS;
        public:
        BinaryExprAST (char Op, std::unique_ptr<ExprAST> LHS, 
                std::unique_ptr<ExprAST> RHS)
            : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
    };


    // CallExprAST - Expression class for function calls.
    class CallExprAST : public ExprAST
    {
        std::string Callee;
        std::vector<std::unique_ptr<ExprAST>> Args;
        public:
        CallExprAST (const std::string &Callee, 
                std::vector<std::unique_copy<ExprAST>> Args)
            : Callee(Callee), Args(std::move(Args)) {}

    };


    // PrototypeAST - This class represents the "prototype" for a function,
    // which captrues its name, and its argument names(thus implicitly the number 
    // of arguments the function takes)
    class PrototypeAST
    {
        std::string name;
        std::vector<std::string> Args;
        public:
        PrototypeAST (const std::string &Name, std::vector<std::string> Args)
            : Name(Name), Args(std::move(Args)) {}

        const std::string &getName() const {return name; } 
    };     


    // FunctionAST - This class represents a function definition itself.
    class FunctionAST
    {
        std::unique_ptr<PrototypeAST> Proto;
        std::unique_ptr<ExprAST> Body;
        public:
        FunctionAST (std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
            : Proto(std::move(Proto)), Body(std::move(Body)) {}

    };

}




/*
 *Parser
 * */

static int CurTok;
static int getNextToken() { return  CurTok = gettok();}

//BinoPrecedence - This holds the precedence for each binary operator that is defined
static std::map<char, int> BinopPrecedence;

// GetTokPrecedence - Get the precedence of the pending binary operator token.
static int GetTokPrecedence() {
    if(!isascii(CurTok)) 
        return -1;


    //Make sure it's a declared binop.
    int TokPrec = BinopPrecedence[CurTok];
    if(TokPrec <= 0)
        return -1;
    return TokPrec;
}    


// LogError* - These are little helper functions for error handling.
std::unique_ptr<ExprAST> LogError(const char *str) {
    fprintf(stderr, "Error: %s\n", str); 
    return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char *str) {
    LogError(str);
    return nullptr; 
}


static std::unique_ptr<ExprAST> ParseExpressiion();

/// numberexpr ::= number
static std::unique_ptr<ExprAST> ParseNumberExpr() {
    auto Result = llvm::make_unique<NumberExprAST>(NumVal); 
    getNextToken(); //consume the number
    return std::move(Result); 
}


/// parenexpr ::= '(' expression ')'
static std::unique_ptr<ExprAST> ParseParenExpr() {
    getNextToken();
    auto V = ParseExpressiion();

    if(!V)
        return nullptr;

    if(CurTok != ')') 
        return LogError("expected ')'");
    getNextToken();
    return V;
}


/// identifierexpr  
// ::= identifier
// ::= identifier '(' expression* ')'
static std::unique_ptr<ExprAST>  ParseIdentifierExpr() {

    std::string IDName = IdentifierStr;

    getNextToken();  // eat identifier

    if(CurTok != '(')   //Simple variable ref.
        return llvm::make_unique<VariableExprAST>(IDName);


    //Call
    getNextToken();  // eat ( 
    std::vector<std::unique_ptr<ExprAST>> Args; 
    if(CurTok != ')') {
        while(true) {
            if(auto Arg = ParseExpressiion()) 
                Args.push_back(std::move(Arg));
            else 
                return nullptr;

            if(CurTok == ')')
                break;

            if(CurTok != ',')
                return LogError("Expected ')' or ',' in argument list"); 
            getNextToken(); 
        } 
    }

    // Eat the ')'
    getNextToken();
    return llvm::make_unique<CallExprAST>(IDName, std::move(Args));
}



/// primary
// ::= identifierexpr
// ::= numberexpr
// ::= parenexpr
static std::unique_ptr<CallExprAST>ParsePrimary() {
    switch (CurTok) {
        case tok_number:
            return ParseParenExpr();                
        case tok_identifier:
            return ParseNumberExpr();
        case '(':
            return ParseParenExpr();
        default:
            return LogError("unknown token when expecting an expression"); 
    } 
}   



/// binoprhs
// ::= ( '+' primary)*
static std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS) {

    // If this is a binop, find its precedence.
    while(true) {
        int TokPrec = GetTokPrecedence();

        // If this is a binop that bind at least as tightly as the current binop,
        // consume it , otherwise we are done.
        if(TokPrec < ExprPrec)
            return LHS;


        // Okay, we know this is a binop.
        int BinOp = CurTok;
        getNextToken();


        // Parse the primary expression afeter the binary operator.
        auto RHS = ParsePrimary();
        if(!RHS)
            return nullptr;

        // If Binop bind less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int NextPrec = GetTokPrecedence();
        if(TokPrec < NextPrec) {
            RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS)); 
            if(!RHS)
                return nullptr;
        }

        // Merge LHS/RHS
        LHS = llvm::make_unique<>(BinOp, std::move(LHS), std::move(RHS));
    }
}



/// expression
//  ::= primary binoprhs
static std::unique_ptr<ExprAST> ParseExpression() {
    auto LHS = ParsePrimary();
    if(!LHS) 
        return nullptr;

    return ParseBinOpRHS(0, std::move(LHS));
}



/// prototype
// ::= id '(' id* ')'
static std::unique_ptr<PrototypeAST> ParsePrototype() {

    if(CurTok != tok_identifier)
        return LogErrorP("Expected function name in prototype");


    std::string FnName = IdentifierStr;
    getNextToken();

    if(CurTok != '(')
        return LogErrorP("Expected '(' in prototype");

    std::vector<std::string> ArgNames;
    while(getNextToken() == tok_identifier)
        ArgNames.push_back(IdentifierStr);
    if(CurTok != ')')
        return LogErrorP("Expected ')' in prototype");

    //success
    getNextToken();
    return  llvm::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}



/// definition ::= 'def' prototype expressoin
static std::unique_ptr<FunctionAST> ParseDefinition() {
    getNextToken();  // eat def.
    auto Proto = ParsePrototype();

    if(!Proto)
        return nullptr;

    if(auto E = ParseExpression())
        return llvm::make_unique<FunctionAST>(std::move(Proto), std::move(E));

    return nullptr;
}




/// toplevelexpr ::= expression
static std::unique_ptr<FunctionAST> ParseTopLevelExpr() {

    if(auto E = ParseExpression()) {
        //Make an anonymous proto.
        auto Proto = llvm::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>());
        return llvm::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}



// extern  ::= 'extern' prototype
static std::unique_ptr<PrototypeAST> ParseExtern() {
    getNextToken();  // eat extern
    return ParsePrototype();
}



/*
 * Top-Level Parsing
 * */
static void HandleDefinition() {

    if(ParseDefinition()) {
        fprintf(stderr, "Parsered a function defition.\n");
    }else {
        //Skip token for error recovery.
        getNextToken();
    }
}

static void HandleExtern() {

    if(ParseExtern()) {
        fprintf(stderr, "Parsed an extern\n"); 
    }else {
        getNextToken(); 
    }
}

static void HandleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if(ParseTopLevelExpr()) {
        fprintf(stderr, "Parsed a top-level expr\n"); 
    }else {
        //Skip token for error recovery.
        getNextToken();
    }
}


/// top ::= definition | external | expression | ';'
static void MainLoop() {
    while(true) {
        fprintf(stderr, "ready>"); 
        switch (CurTok) {
            case tok_eof:
                break;
            case tok_def:
                HandleDefinition();
                break;
            case tok_extern:
                HandleExtern();
                break;
            case ';':
                getNextToken();
                break;
            default:
                HandleTopLevelExpression();
                break; 
        }
    }
}



/// main 
int main() {

    // Install standard binary operators.
    // 1 is lowest precedence.
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 30;
    BinopPrecedence['*'] = 40;


    // Prime the first token.
    fprintf(stderr, "ready> ");
    getNextToken();

    // Run the mainn "interpreter loop" now.
    MainLoop();


    return 0;
}







