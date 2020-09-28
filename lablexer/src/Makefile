CC       = gcc -g
LEX      = flex -i -I 
INC      = -I../pl0/include 

all:  labLexer-2 labLexer-2m 

labLexer-2:   ../grammar/lex_analyse.lex
	$(LEX)  ../grammar/lex_analyse.lex
	$(CC)  $(INC) -DLEXERGEN lex.yy.c lex_analyse-2.c -o labLexer-2 -ll

labLexer-2m:   
	$(CC)  $(INC) lex_analyse-2.c -o labLexer-2m -ll

clean:
	rm -f *.BAK *.o core *~* *.a 
	rm -f *.tab.h *.tab.c
	rm -f *.out
