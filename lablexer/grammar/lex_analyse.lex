
%{
#include <stdio.h>
#include <string.h>
#define relop 1
#define other 2
%}


other  [^><=/n/r]

%%
{other}+  { 
           printf("(other,%zu)",yyleng);
          }
"<"        { printf("(relop,%s)",yytext );
          }

">"        { printf("(relop,%s)",yytext );
          }
"="        { printf("(relop,%s)",yytext );
          }
"<>"      { printf("(relop,%s)",yytext );
        }
"<="      { printf("(relop,%s)",yytext );
        }

%%
void getsym(){
        yylex();
        return ;
}
int yywarap(){
        return 1;
}
