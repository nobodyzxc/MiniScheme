#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "mem.h"
#include "type.h"
#include "proc.h"
#include "eval.h"
#include "util.h"
#include "token.h"
#include "parse.h"
#include "gc.h"


char cwd[1024];
bool flag_i = false;

FILE *main_str;
char lib_logs[500];
char *ctx_p = NULL;
char *contxt = NULL;

char *raw_input(char *prompt){

    free(contxt);

    rl_instream = main_str;

    FILE *prev_rl_outstream = rl_outstream;

    rl_outstream = main_str == stdin ? stdout : fopen("/dev/null", "w");

    if(!(contxt = readline(prompt))) return NULL;

    if(main_str == stdin) add_history(contxt);
    if(main_str != stdin) fclose(rl_outstream);

    rl_outstream = prev_rl_outstream;

    if(contxt == NULL) puts("buffer NULL");
    return contxt;
}

char *non_blank(char *p , char *prompt){
    while(p && *p && is_blank(*p)) p++;
    while(!p || !*p){
        p = raw_input(prompt);
        if(p == NULL) return NULL;
        while(*p && is_blank(*p)) p++;
    }
    return p;
}

void clear_contxt(void){
    free(contxt);
    contxt = NULL;
    ctx_p = NULL;
}


bool is_shebang(char *p){
    return p && *p && !strncmp(p , "#!" , 2);
}

bool repl(bool repl_p , bool auto_gc){
    bool first_line = true;
    while((ctx_p && *ctx_p) || (ctx_p = raw_input("> "))){
        if(first_line){
            first_line = false;
            if(is_shebang(ctx_p)){
                ctx_p = NULL;
                continue;
            }
        }
        Token tok = NULL;
        if(*ctx_p){
            tok_raw_input = raw_input;
            tok_non_blank = non_blank;
            ctx_p = tokenize(ctx_p , &tok);
        }
        if(!tok){
            if(ctx_p) continue;
            else return false;
        }
        Obj val = parse(tok);
        free_token(tok);
        if(val != err){
            val = eval(val , glenv);
            if(repl_p && main_str == stdin && val && val != err)
                print_obj(val) , printf("\n");
        }
        if(auto_gc) auto_try_gc();
        if(val == err) return false;
    }
    clear_contxt();
    return true;
}

void path_error(char *name){
    if(name[0] == '/')
        fprintf(stderr , "cannot load %s\n" , name);
    else
        fprintf(stderr , "cannot load %s\n" , name);
}

bool load_script(char *name , bool log){
    FILE *prev_stream = main_str;
    bool succ = main_str = fopen(name , "r");
    if(main_str)
        succ &= repl(false , false) , fclose(main_str);
    else if(log)
        path_error(name);
    main_str = prev_stream;
    if(!succ) printf("\t     ... %s\n" , name);
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
            char expr[300];
            sprintf(expr , "");
            for(++i ; i < argc ; i++)
                sprintf(expr , "%s %s" , expr , argv[i]);
            ctx_p = expr;
            while(*ctx_p){
                tok_raw_input = raw_input;
                tok_non_blank = non_blank;
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

    main_str = stdin;
    getcwd(cwd , sizeof(cwd));

    init_buildins();
    load_libraries();

    if(argc == 1)
        flag_i = true;
    else
        handle_flags(argc , argv);
    if(flag_i){
        stdin_printf("Welcome to Zekin " xstr(VERSION));
        printf("%s\n" , EQS(lib_logs , " [ ] ") ?
                " [ primary ]" : lib_logs);
        while(!repl(true , true));
        stdin_printf("\n");
    }
    return 0;
}
