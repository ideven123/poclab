#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pl0.h"
#ifndef LEXERGEN

void getsym(){
    int i=0 ,num=0 ;
    char ch ;
    char src[200];

	fgets(src,200,infile);

    int state = 0;
    int next_state = 0 ;
    while( !(src[i] == 13 && src[i+1]==10 )&& src[i]!= 10 ){
         ch = src[i] ;
         state = next_state ;
        switch (state)
        {
        case 0 :
            if(ch == '<') {
                next_state = 1;
                if(num != 0) printf("(other,%d)",num) ;
                num = 0;
            }     
            else if(ch == '='){
                next_state = 0;
                if(num != 0) printf("(other,%d)",num) ;
                printf("(relop,=)");
                num = 0;
            }else if(ch == '>')
            {
                next_state = 6;
                if(num != 0) printf("(other,%d)",num) ;
                num = 0;
            }else{
                num++;
                next_state = 0;
            }          
            break;
        case 1 :
             if(ch == '=')
               printf("(relop,<=)");
             else if (ch == '>')
             {
                 printf("(relop,<>)"); 
             }             
            else
               {
                printf("(relop,<)"); 
                i = i-1 ;
                num = 0;
               }              
              next_state = 0 ;
            break;

        case 6 :
            if(ch == '=')
            printf("(relop,>=)");
            else
            {   
                i = i-1 ; 
                printf("(relop,>)"); 
                num = 0;
            }
            next_state = 0;
            break;                  
        
        default:
            break;
        }
        i++ ;
    }
    if(num != 0){
        printf("(other,%d)",num);
    }
    if(next_state == 1 )  printf("(relop,<)"); 
    if(next_state == 6 )  printf("(relop,>)"); 
    return ;
 }

#endif

int main(int argc, char *argv[])
{
	

	printf("please input source program file name: ");
	scanf("%s", infilename);
	if ( (infile=fopen(infilename, "r"))==NULL ) {
		printf("File %s can't be opened.\n", infilename);
		exit(1);
	}
#ifdef LEXERGEN
	extern FILE * yyin;
	yyin=infile;
#endif
  
    getsym();	


	printf("File %s has already been scanned.\n", infilename);
	fclose(infile);
}
