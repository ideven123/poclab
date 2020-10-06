#include <iostream>
#include <stdio.h>
#include <string.h>
#include "antlr4-runtime.h"
#include "relop.h"

using namespace antlr4;
int main(int argc, const char* argv[]) {
    
    char in[200] ;
    int length;
    std::string s;
    std::cin.get(in,200);
    ANTLRInputStream input(in);
    relop lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();
    for (auto token : tokens.getTokens()){
        
        switch (token->getType()){
            case  7 : s = token->getText(); length = s.size() ;printf("(other,%d)",length); break;
            case  2 : printf("(relop,<>)"); break;
            case  1 : printf("(relop,=)");  break;
            case  3 : printf("(relop,<)"); break;
            case  4 : printf("(relop,>)"); break;
            case  5 : printf("(relop,<=)"); break;
            case  6 : printf("(relop,>=)"); break;
            default : break;
        }
    }

    return 0;
}
