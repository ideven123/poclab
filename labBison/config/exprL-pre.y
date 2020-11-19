/*
 * exprL-pre.y : A simple yacc expression parser
 *          Based on the Bison manual example. 
 * This file fails in the conversion 
 * from infix expression to corresponding Polish expression.
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
%right UMINUS

%type  <Un> exp

%%
input   :
	  { lineno ++; 
	  }
        |  
          input
	  { 
	    printf("Line %d (%d):\t", lineno++, @1.last_line);
	  }
          line
        ;

line    : EOL			{ printf("\n");}
        | exp EOL 		{ printf(" = %g at line %d\n",$1.val, @1.last_line);}

exp     : NUMBER                 { $$.val = $1.val ;        }
        | {printf(" + ");} exp PLUS  exp          { $$.val = $2.val + $4.val;   }
        | {printf(" - ");} exp MINUS exp          { $$.val = $2.val - $4.val;   }
        | {printf(" * ");} exp MULT  exp          { $$.val = $2.val * $4.val;   }
        | {printf(" / ");} exp DIV   exp          { $$.val = $2.val / $4.val;   }
        | {printf(" - ");} MINUS  exp %prec UMINUS { $$.val = -$3.val;       }
        | {printf(" ** ");}exp EXPON exp          { $$.val = pow($2.val,$4.val);}
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

