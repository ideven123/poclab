/*
 * expr.y : A simple yacc expression parser
 *          Based on the Bison manual example. 
 * The parser uses precedence declarations to eliminate conflicts.
 */

%{
#include <stdio.h>
#include <math.h>
int yylex();
void yyerror(const char *s);
int lineno = 0;
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

%type  <Un> exp

%%
input   :
	  { lineno ++; }
        |  
          input
	  { printf("Line %d (%d):\t", lineno++, @1.last_line);
	  }
          line
        ;

line    : EOL			{ printf("\n");}
        | exp EOL 		{ printf(" = %g at line %d\n",$1.val, @1.last_line);}

exp     : NUMBER                 { $$.val = $1.val;        }
        | exp PLUS  exp          { $$.val = $1.val + $3.val;   }
        | exp MINUS exp          { $$.val = $1.val - $3.val;   }
        | exp MULT  exp          { $$.val = $1.val * $3.val;   }
        | exp DIV   exp          { $$.val = $1.val / $3.val;   }
        | MINUS  exp %prec MINUS { $$.val = -$2.val;       }
        | exp EXPON exp          { $$.val = pow($1.val,$3.val);}
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

