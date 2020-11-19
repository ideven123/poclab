/*
 * exprL-pre-s3.y : A simple yacc expression parser
 *          Based on the Bison manual example. 
 * You need add rules with actions to complete the conversion 
 * from infix expression to corresponding Polish expression.
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	    printf("Line %d:\n\t", lineno++);
	  }
          line
        ;

line    : EOL			{ printf("\n");}
        | exp EOL 		{ printf("%s = %g at line %d\n",$1.code,$1.val, @1.last_line);}
        ;
exp     : NUMBER                 { $$.val = $1.val;  sprintf($$.code,"%g ",$$.val);    }
        | exp PLUS  exp          { $$.val = $1.val + $3.val; strcpy($$.code," + ");strcat($$.code,$1.code);strcat($$.code,$3.code);   }
        | exp MINUS exp          { $$.val = $1.val - $3.val; strcpy($$.code," - ");strcat($$.code,$1.code);strcat($$.code,$3.code);    }
        | exp MULT  exp          { $$.val = $1.val * $3.val; strcpy($$.code," * ");strcat($$.code,$1.code);strcat($$.code,$3.code);    }
        | exp DIV   exp          { $$.val = $1.val / $3.val; strcpy($$.code," / ");strcat($$.code,$1.code);strcat($$.code,$3.code);   }
        | MINUS  exp %prec UMINUS { $$.val = -$2.val;  strcpy($$.code," - ");strcat($$.code,$2.code);   }
        | exp EXPON exp          { $$.val = pow($1.val,$3.val);  strcpy($$.code," ** ");strcat($$.code,$1.code);strcat($$.code,$3.code);}
        | LB exp RB              { $$.val = $2.val;  strcpy($$.code, $2.code);      }
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

