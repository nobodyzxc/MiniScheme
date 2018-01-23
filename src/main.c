#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

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
    while((glo_bufptr && *glo_bufptr) || (glo_bufptr = input(glo_buffer , "> " , false))){
        if(first_line){
            first_line = false;
            if(is_shebang(glo_bufptr)){
                glo_bufptr = NULL;
                continue;
            }
        }
        if(*glo_bufptr) glo_bufptr = tokenize(glo_buffer , glo_bufptr , &tok);
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

void path_error(char *name){
    char cwd[1024];
    getcwd(cwd , sizeof(cwd));
    if(name[0] == '/')
        fprintf(stderr , "cannot load %s\n" , name);
    else if(name[0] == '.' && name[1] == '/')
        fprintf(stderr , "cannot load %s/%s\n" , cwd , name + 2);
    else
        fprintf(stderr , "cannot load %s/%s\n" , cwd , name);
}

bool load_script(char *name , bool log){
    FILE *prev_stream = stream;
    bool succ = stream = fopen(name , "r");
    if(stream)
        repl(false , false) , fclose(stream);
    else if(log)
        path_error(name);
    stream = prev_stream;
    return succ;
}

int handle_flags(int argc , char *argv[]){
    Token tok;
    for(int i = 1 ; i < argc ; i++){
        if(EQS(argv[i] , "-i"))
            interpret = true;
        else if(EQS(argv[i] , "-h")){
            printf("%s [ -e exprs | [ -i ] scripts | -h ] \n\n"
                    "COMMAND LINE OPTIONS\n"
                    "       -e     eval expressions after the flag\n\n"
                    "       -i     enter interactive mode after load scripts\n\n"
                    "       -h     display the help information\n" ,
                    argv[0]);
            exit(0);
        }
        else if(EQS(argv[i] , "-e")){
            if(i + 1 >= argc) puts("null expr") , exit(1);
            sprintf(glo_buffer , "");
            for(++i ; i < argc ; i++){
                sprintf(glo_buffer , "%s %s" ,
                        glo_buffer , argv[i]);
            }
            glo_bufptr = glo_buffer;
            while(*glo_bufptr){
                glo_bufptr = tokenize(glo_buffer , glo_bufptr , &tok);
                Obj val = eval(parse(tok) , glenv);
                if(val != err) alert("" , val);
            }
            exit(0);
        }
        else load_script(argv[i] , true);
    }
}

char *get_extname(char *path){
    static char name[300];
    sprintf(name ,
            "%s/" EXTENSION ,
            dirname(path));
    return name;
}

int main(int argc , char *argv[]){
    stream = stdin;
    init_buildins();
    bool succ = load_script(get_extname(argv[0]) , false);
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
