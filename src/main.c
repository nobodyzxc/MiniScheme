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
bool interpret = false;


bool is_shebang(char *p){
    return p && *p && !strncmp(p , "#!" , 2);
}

void repl(bool repl_p , bool auto_gc){
    Token tok = NULL;
    bool first_line = true;
    char *p = "";
    while((p && *p) || (p = input(glo_buffer , "> " , false))){
        if(first_line){
            first_line = false;
            if(is_shebang(p)){
                p = NULL;
                continue;
            }
        }
        if(*p) p = tokenize(glo_buffer , p , &tok);
        if(!tok) continue;
        //print_token(tok);
        Obj val = parse(tok);
        val = eval(val , glenv);
        if(repl_p && stream == stdin && val && val != err)
            print_obj(val) , printf("\n");
        //if(val == err) puts("  _(:з」∠)_  ");
        free_token(tok);
        tok = NULL , val = NULL;
        if(auto_gc) auto_try_gc();
    }
    clear_buf();
}

bool load_script(char *name){
    FILE *prev_stream = stream;
    bool succ = stream = fopen(name , "r");
    if(stream)
        repl(false , false) , fclose(stream);
    stream = prev_stream;
    return succ;
}

int handle_flags(int argc , char *argv[]){
    char *p;
    Token tok;
    for(int i = 1 ; i < argc ; i++){
        if(EQS(argv[i] , "-i"))
            interpret = true;
        else if(EQS(argv[i] , "-e")){
            if(i + 1 >= argc) puts("null expr") , exit(1);
            for(int i = 0 ; i < argc ; i++){
                for(int j = 0 ; j < strlen(argv[i]) ; j++)
                    printf("%c -> %p\n" , argv[i][j] , &argv[i][j]);
                puts("");
            }

            p = argv[i + 1];
            while(*p){
                p = tokenize(glo_buffer , p , &tok);
                print_obj(eval(parse(tok) , glenv));
            }
            exit(0);
        }
        else load_script(argv[i]);
    }
}

int main(int argc , char *argv[]){
    stream = stdin;
    init_buildins();
    bool succ = load_script(EXTENSION);
    if(argc == 1)
        interpret = true;
    else
        handle_flags(argc , argv);
    if(interpret){
        stdin_printf("Welcome to Zekin v1.0");
        if(succ) stdin_printf(" (\"" EXTENSION "\" loaded)");
        stdin_printf("\n");
        repl(true , true); stdin_printf("\n");
    }
    return 0;
}
