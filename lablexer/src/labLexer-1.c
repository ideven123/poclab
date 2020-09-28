#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
    int i=0 ,num=0 ;
    char ch ;
    char src[200];
    fflush(stdin);
    fgets(src,200,stdin);
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
                printf("(other,%d)",num) ;
                printf("(relop,=)");
                num = 0;
            }else if(ch == '>')
            {
                next_state = 6;
                 printf("(other,%d)",num) ;
                 num = 0;
            }else{
                num++;
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
                num = 1;
               }              
              next_state = 0 ;
            break;

        case 6 :
            if(ch == '=')
            printf("(relop,>=)");
            else
            {
                printf("(relop,>)"); 
                num = 1;
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
    return 0;
 }
