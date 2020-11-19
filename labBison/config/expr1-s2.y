/*
 * expr1.y : A simple yacc expression parser
 *          Based on the Bison manual example. 
 * The parser eliminates conflicts by introducing more nondeterminals.
 */

%{
#include <stdio.h>
#include <math.h>
int yylex();
void yyerror(const char *s);
%}

%union {
    struct Un{
   float val;
   char code[100];
   } Un ; 
   char *op;
}
%locations

%token <Un> NUMBER
%token <op> PLUS MINUS MULT DIV EXPON
%token EOL
%token LB RB

%left  MINUS PLUS
%left  MULT DIV
%right EXPON 
%type  <Un> exp term next eon fact 

%%
input   :
        | input line
        ;

line    : EOL { printf("\n");}
        | exp EOL { printf(" = %g  at line %d\n",$1.val, @$.last_line);}

exp     : exp PLUS  term         { $$.val = $1.val + $3.val;   }
        | exp MINUS term         { $$.val = $1.val - $3.val;   }
        | term                   { $$.val = $1.val ;        }
        ;
term    : term MULT next         { $$.val = $1.val * $3.val;   }
        | term DIV  next         { $$.val = $1.val / $3.val;   }
        | next                   { $$.val = $1.val;        }
        ;

next    : MINUS next             { $$.val = -$2.val;       }
        | eon                    { $$.val = $1.val;        }
        ; 
eon     : fact EXPON eon         { $$.val = pow($1.val,$3.val);}
        | fact                   { $$.val = $1.val;        }
        ;
fact    : NUMBER                 { $$.val = $1.val;        }
        | LB exp RB              { $$.val = $2.val;        }
        ;

%%
void yyerror(const char *message)
{
  printf("%s\n",message);
}

int main(int argc, char *argv[])
{
  yyparse();
  return(0);
}








