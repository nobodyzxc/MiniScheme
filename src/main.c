#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "mem.h"
#include "type.h"
#include "proc.h"
#include "eval.h"
#include "util.h"
#include "token.h"
#include "parse.h"
#include "gc.h"

#define EXTENSION "lib.ss"
FILE *stream;

bool check_shell(char *p){
    while(*p && is_blank(*p)) p++;
    return *p == '#'
        && (strncmp(p , "#t" , 2))
        && (strncmp(p , "#f" , 2))
        && (strncmp(p , "#\\" , 2));
}

void repl(bool _p , bool auto_gc){
    Token tok = NULL;
    bool fist_line = true;
    char *p = "";
    while((p && *p) || (p = input("> " , false))){
        if(fist_line){
            fist_line = false;
            if(check_shell(p)){
                *p = 0;
                continue;
            }
        }
        if(*p) p = tokenize(p , &tok);
        if(!tok) continue;
        Obj val = parse(tok);
        val = eval(val , glenv);
        if(_p && stream == stdin)
            print_obj(val) , printf("\n");
        free_token(tok);
        tok = NULL , val = NULL;
        if(auto_gc) auto_try_gc();
    }
    clear_buf();
}

int main(int args , char *argv[]){

    init_buildins();
    stream = fopen(EXTENSION , "r");
    if(stream) repl(false , false) , fclose(stream) , auto_try_gc();
    stream = stdin;
    if(args > 1)
        if(!(stream = fopen(argv[1] , "r")))
            printf("cannot open %s\n" , argv[1]) , exit(1);
    stdin_printf("Welcome to Zekin v1.0\n");
    repl(true , true); stdin_printf("\n");
    return 0;
}
