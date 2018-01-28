#include "mem.h"
#include "type.h"
#include "util.h"
#include "proc.h"
#include "token.h"

#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#ifdef RL_LIB
#include <readline/readline.h>
#include <readline/history.h>
Obj rl_pt = NULL;

char *symbol_generator(const char *text , int state){

    static int len;
    static Symtree iter;
    static char dir;

    char *name = NULL;

    if (!state) {
        dir = 'L';
        len = strlen(text);
        iter = glenv->env->symtab;
        while(iter){
            if(!strncmp(text , iter->sym->str , len))
                break;
            iter = *LoR(iter , cmp_node(text , iter));
        }
        if(!iter) return NULL;
    }
    name = strdup(iter->sym->str);
    /* move pointer */
    switch(dir){
        case 'L':
            if(iter->lt) iter = iter->lt;
            else if(iter->rt) iter = iter->lt;
            else if(iter->parent){
                /* do something recursively */
            }
            else iter = NULL;
        case 'R':
        case 'U':
        default:
            puts("completeion error") , exit(0);
            break;
    }
    return name;

    return NULL;
}

char ** symbol_completion(
        const char *text , int start , int end){
    return rl_completion_matches(text , symbol_generator);
}

char *rl_raw_input(char *prompt){

    assert(rl_pt);
    free(port_ctx(rl_pt));
    rl_instream = port_fp(rl_pt);

    FILE *prev_rl_outstream = rl_outstream;

    rl_outstream =
        rl_instream == stdin ?
        stdout : fopen("/dev/null", "w");

    rl_attempted_completion_function = symbol_completion;

    if(!(port_ctx(rl_pt) = readline(prompt))) return NULL;

    if(rl_instream != stdin) fclose(rl_outstream);
    if(port_fp(rl_pt) == stdin) add_history(port_ctx(rl_pt));

    rl_outstream = prev_rl_outstream;
    port_ptr(rl_pt) = port_ctx(rl_pt);

    return port_ptr(rl_pt);
}

char *rl_non_blank(char *p , char *prompt){
    while(p && *p && is_blank(*p)) p++;
    while(!p || !*p){
        p = rl_raw_input(prompt);
        if(p == NULL) return NULL;
        while(*p && is_blank(*p)) p++;
    }
    return p;
}
#endif

Obj read_pt = NULL;

#define RDSIZE 300
char *handled_raw_input(char *raw){
    char *p = raw;
    while(*p){
        if(*p == '\033'){
            *p = ' ';
            *(p + 1) = ' ';
            *(p + 2) = ' ';
            p += 2; /* ignore arrow key "\033[A"*/
        }
        else if(isalnum(*p));
        else if(ispunct(*p));
        else if(iscntrl(*p)){
            *p = ' ';
        }
        else if(isgraph(*p)){
            *p = ' ';
        }
        else if(isprint(*p) && *p != ' '){
            printf("isprint(%c)=>' '\n" , *p);
        }
        p++;
    }
    return raw;
}
char *read_raw_input(char *prompt){
    assert(read_pt);
    free(port_ctx(read_pt));

    FILE *out_fp =
        port_fp(read_pt) == stdin ?
        stdout : fopen("/dev/null", "w");
    fprintf(out_fp , prompt);
    if(port_fp(read_pt) != stdin)
        fclose(out_fp);

    port_ptr(read_pt) =
        port_ctx(read_pt) =
        (char*)malloc(sizeof(char) * RDSIZE);

    if(!fgets(port_ctx(read_pt) ,
                RDSIZE , port_fp(read_pt))){
        free(port_ctx(read_pt));
        return port_ptr(read_pt) = port_ctx(read_pt) = NULL;
    }

    port_ctx(read_pt)[strlen(port_ctx(read_pt)) - 1] = 0;
    return handled_raw_input(port_ctx(read_pt));
}

char *read_non_blank(char *p , char *prompt){
    while(p && *p && is_blank(*p)) p++;
    while(!p || !*p){
        p = read_raw_input(prompt);
        if(p == NULL) return NULL;
        while(*p && is_blank(*p)) p++;
    }
    return p;
}

void clear_ctx(Obj pt){
    free(port_ctx(pt));
    port_ctx(pt) = NULL;
    port_ptr(pt) = NULL;
}

void close_port(Obj pt_obj){
    if(pt_obj->port->open){
        fclose(port_fp(pt_obj));
        pt_obj->port->open = false;
    }
}

Obj open_port(char *name , char *mode){
    FILE *fp = fopen(name , mode);
    if(!fp) return (Obj)err;
    return new(PORT , fp , strdup(name) , mode);
}

