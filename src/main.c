#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <assert.h>

#include "mem.h"
#include "type.h"
#include "util.h"
#include "proc.h"
#include "eval.h"
#include "util.h"
#include "token.h"
#include "parse.h"
#include "gc.h"
#include "io.h"

Obj repl_pt = NULL;

char cwd[1024];
bool flag_i = false;

char lib_logs[500];

bool is_shebang(char *p){
    return p && *p && !strncmp(p , "#!" , 2);
}

bool repl(bool repl_p , bool auto_gc){
    assert(repl_pt);

    bool first_line = true;
    while(1){
        if(!port_ptr(repl_pt) || !*port_ptr(repl_pt)){
            Obj prev_pt = I_MODE_PORT;
            I_MODE_PORT = repl_pt;
            port_ptr(repl_pt) = I_MODE_RAW_INPUT("> ");
            I_MODE_PORT = prev_pt;
            if(!port_ptr(repl_pt)) break;
        }
        if(first_line){
            first_line = false;
            if(is_shebang(port_ptr(repl_pt))){
                port_ptr(repl_pt) = NULL;
                continue;
            }
        }
        Token tok = NULL;
        if(*port_ptr(repl_pt)){
            Obj prev_pt = I_MODE_PORT;
            I_MODE_PORT = repl_pt;
            tok_raw_input = I_MODE_RAW_INPUT;
            tok_non_blank = I_MODE_NON_BLANK;
            port_ptr(repl_pt) =
                tokenize(port_ptr(repl_pt) , &tok);
            I_MODE_PORT = prev_pt;
        }
        if(!tok){
            if(port_ptr(repl_pt)) continue;
            else return false;
        }
        Obj val = parse(tok);
        free_token(tok);
        if(val != err){
            val = eval(val , glenv);
            if(repl_p &&
                    port_fp(repl_pt) == stdin
                    && val && val != err)
                print_obj(val) , printf("\n");
        }
        if(auto_gc) auto_try_gc();
        if(val == err) return false;
    }
    clear_ctx(repl_pt);
    return true;
}

void path_error(char *name){
    if(name[0] == '/')
        fprintf(stderr , "cannot load %s\n" , name);
    else
        fprintf(stderr , "cannot load %s\n" , name);
}

bool load_script(char *name , bool log){
    Obj prev_pt = repl_pt;
    repl_pt = open_port(name , "r");
    bool succ = false;
    if(repl_pt != err){
        succ = true;
        succ &= repl(false , false);
        close_port(repl_pt);
    }
    else if(log)
        path_error(name);
    repl_pt = prev_pt;
    if(!succ)
        printf("\t     ... %s\n" , name);
    return succ;
}

int handle_flags(int argc , char *argv[]){
    Token tok;
    for(int i = 1 ; i < argc ; i++){
        if(EQS(argv[i] , "-i"))
            flag_i = true;
        else if(EQS(argv[i] , "-h")){
            printf("usage: zekin [-h] [-v] [-l] [-e exprs] [-i] [file ...]\n\n"
                    "Yet another scheme flag_ier\n\n"
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
            I_MODE_PORT = stdin_pt;
            port_ptr(I_MODE_PORT) =
                port_ctx(I_MODE_PORT) =
                (char*)MALLOC(sizeof(char) * 300);
            sprintf(port_ctx(I_MODE_PORT) , "");
            for(++i ; i < argc ; i++)
                sprintf(port_ctx(I_MODE_PORT) , "%s %s" ,
                        port_ctx(I_MODE_PORT) , argv[i]);
            while(*port_ptr(I_MODE_PORT)){
                tok_raw_input = I_MODE_RAW_INPUT;
                tok_non_blank = I_MODE_NON_BLANK;
                port_ptr(I_MODE_PORT) =
                    tokenize(port_ptr(I_MODE_PORT) , &tok);
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
    sprintf(lib_logs , " [ ");
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
            sprintf(lib_logs , "%s%s " , lib_logs , lib_name ,
                    load_script(lib_path , false) ? "loaded" : "failed");
        }
        fclose(config);
    }
    else puts("cannot load config file : " xstr(LIBPATH) xstr(LIBCONFIG));
    sprintf(lib_logs , "%s] " , lib_logs);
    chdir(cwd);
}

int main(int argc , char *argv[]){

    port_fp(stdin_pt) = stdin;
    getcwd(cwd , sizeof(cwd));

    init_buildins();
    load_libraries();

    if(argc == 1)
        flag_i = true;
    else
        handle_flags(argc , argv);
    if(flag_i){
        repl_pt = stdin_pt;
        imode_msg("Welcome to Zekin " xstr(VERSION));
        printf("%s\n" , EQS(lib_logs , " [ ] ") ?
                " [ primary ]" : lib_logs);
        while(!repl(true , true));
        imode_msg("\n");
    }
    return 0;
}
