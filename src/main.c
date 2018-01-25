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

FILE *stream;
bool interpret = false;

char cwd[1024];
char lib_logs[500];

bool is_shebang(char *p){
    return p && *p && !strncmp(p , "#!" , 2);
}

bool repl(bool repl_p , bool auto_gc){
    Token tok = NULL;
    bool first_line = true;
    while((ctx_p && *ctx_p) || (ctx_p = input("> " , false))){
        if(first_line){
            first_line = false;
            if(is_shebang(ctx_p)){
                ctx_p = NULL;
                continue;
            }
        }
        if(*ctx_p) ctx_p = tokenize(ctx_p , &tok);
        if(!tok) continue;
        Obj val = parse(tok);
        val = eval(val , glenv);
        if(repl_p && stream == stdin && val && val != err)
            print_obj(val) , printf("\n");
        //if(val == err) puts("  _(:з」∠)_  ");
        free_token(tok);
        if(auto_gc) auto_try_gc();
        if(val == err) return false;
        tok = NULL , val = NULL;
    }
    clear_buffer();
    return true;
}

void path_error(char *name){
    if(name[0] == '/')
        fprintf(stderr , "cannot load %s\n" , name);
    else
        fprintf(stderr , "cannot load %s\n" , name);
}

bool load_script(char *name , bool log){
    FILE *prev_stream = stream;
    bool succ = stream = fopen(name , "r");
    if(stream)
        succ &= repl(false , false) , fclose(stream);
    else if(log)
        path_error(name);
    stream = prev_stream;
    if(!succ) printf("\t     ... %s\n" , name);
    return succ;
}

int handle_flags(int argc , char *argv[]){
    Token tok;
    for(int i = 1 ; i < argc ; i++){
        if(EQS(argv[i] , "-i"))
            interpret = true;
        else if(EQS(argv[i] , "-h")){
            printf("usage: zekin [-h] [-v] [-l] [-e exprs] [-i] [file ...]\n\n"
                    "Yet another scheme interpreter\n\n"
                    "positional arguments:\n"
                    "   file   scheme script to run\n\n"
                    "optional arguments:\n"
                    "   -i     run file interactively\n\n"
                    "   -e     eval expressions and exit\n\n"
                    "   -v     information about version\n\n"
                    "   -l     show library path defined\n\n"
                    "   -h     show this help message and exit\n" ,
                    basename(argv[0]));
            exit(0);
        }
        else if(EQS(argv[i] , "-v")){
            printf("version : " xstr(VERSION) "\n");
            exit(0);
        }
        else if(EQS(argv[i] , "-l")){
            printf("library path  : " xstr(LIBPATH) "\n");
            printf("configuration : " xstr(LIBPATH) xstr(LIBCONFIG) "\n");
            exit(0);
        }
        else if(EQS(argv[i] , "-e")){
            if(i + 1 >= argc) exit(0); /* no expr to eval */
            char expr[300];
            sprintf(expr , "");
            for(++i ; i < argc ; i++){
                sprintf(expr , "%s %s" , expr , argv[i]);
            }
            ctx_p = expr;
            while(*ctx_p){
                ctx_p = tokenize(ctx_p , &tok);
                Obj val = eval(parse(tok) , glenv);
                if(val != err) alert("" , val);
            }
            exit(0);
        }
        else load_script(argv[i] , true);
    }
}

void load_libraries(){
    chdir(xstr(LIBPATH));
    char lib_path[300];
    char lib_name[100];
    sprintf(lib_logs , "");
    FILE *config = fopen(xstr(LIBPATH) xstr(LIBCONFIG) , "r");
    if(config){
        while(~fscanf(config , "%s" , lib_name)){
            if(lib_name[0] == '#'){
                fgets(lib_name , sizeof(lib_name) , config);
                continue;
            }
            sprintf(lib_path ,
                    (lib_name[0] == '/' ?
                     "%s" : xstr(LIBPATH) "%s")
                    , lib_name);
            sprintf(lib_logs , "%s (%s %s)" , lib_logs , lib_name ,
                    load_script(lib_path , false) ? "loaded" : "failed");
        }
        fclose(config);
    }
    else puts("cannot load config file : " xstr(LIBPATH) xstr(LIBCONFIG));
    chdir(cwd);
}

int main(int argc , char *argv[]){

    getcwd(cwd , sizeof(cwd));

    stream = stdin;
    init_buildins();
    load_libraries();

    if(argc == 1)
        interpret = true;
    else
        handle_flags(argc , argv);
    if(interpret){
        stdin_printf("Welcome to Zekin");
        printf(lib_logs) , puts("");
        repl(true , true); stdin_printf("\n");
    }
    return 0;
}
