#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "token.h"
#include "parse.h"
#include "proc.h"
#include "eval.h"
#include "mem.h"

FILE *stream;

Env gl_env;

bool check_shell(char *p){
    while(*p && is_blank(*p)) p++;
    return *p == '#'
        && (strncmp(p , "#t" , 2))
        && (strncmp(p , "#f" , 2))
        && (strncmp(p , "#\\" , 2));
}

int main(int args , char *argv[]){
    stream = stdin;
    if(args > 1)
        if(!(stream = fopen(argv[1] , "r")))
            printf("cannot open %s\n" , argv[1]) , exit(1);
    Token tok = NULL;
    bool fist_line = true;
    char *p = NULL;
    while((p && *p) || (p = input("> " , false))){
        if(fist_line){
            fist_line = false;
            if(check_shell(p)){
                *p = 0;
                continue;
            }
        }
        if(*p) p = tokenize(p , &tok);
        Obj val = parse(tok);
        Obj v = NULL;
        v = eval(val , gl_env);
        if(v) print_obj(v) , printf("\n");
        free_token(tok);
        free_obj(val);
        if(val != v && v) free_obj(v);
        tok = NULL , val = NULL , v = NULL;
    }
    stdin_printf("\n");
    fclose(memchk);
    return 0;
}

/*   Obj obj = new_obj(INTEGER);
 *   obj->integer = 1;
 *   obj = cons(obj , cons(obj , nil));
     print_obj(obj) , puts("");
     print_obj(nil) , puts(""); */
