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

exp     : NUMBER                 { $$.val = $1.val;  printf("%g ", $1.val);     }
        | exp PLUS  exp          { $$.val = $1.val + $3.val;  printf("%s ", $2); }
        | exp MINUS exp          { $$.val = $1.val - $3.val;  printf("%s ", $2); }
        | exp MULT  exp          { $$.val = $1.val * $3.val;  printf("%s ", $2); }
        | exp DIV   exp          { $$.val = $1.val / $3.val;  printf("%s ", $2); }
        | MINUS  exp %prec UMINUS { $$.val = -$2.val;  printf("%s ", $1);      }
        | exp EXPON exp          { $$.val = pow($1.val,$3.val);  printf("%s ", $2);}
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

