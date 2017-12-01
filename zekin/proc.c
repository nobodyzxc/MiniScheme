#include "proc.h"
#include "mem.h"
#include "func.h"
#include "syntax.h"
#include <stdlib.h>

Obj lookup_symbol(char *v , Obj env_obj){
    while(env_obj){
        Symtree iter = env_obj->env->symtab;
        while(iter){
            int df = strcmp(iter->sym->str , v);
            if(!df) return iter->val;
            if(df < 0) iter = iter->lt;
            else iter = iter->rt;
        }
        env_obj = env_obj->env->parent;
    }
    printf("cannot find symbol %s\n" , v);
    return NULL;
}

#define BIND(TYPE , SYM , PROC , ENV) \
    add_symbol(\
            new(SYMBOL , strdup(SYM)) , \
            new(TYPE , strdup(SYM) , PROC) , ENV)

void init_buildins(){
    glenv = new(ENV , NULL);
    BIND(FUNCTION , "car"     , &apply_car    , glenv);
    BIND(FUNCTION , "cdr"     , &apply_cdr    , glenv);
    BIND(FUNCTION , "cons"    , &apply_cons   , glenv);
    BIND(FUNCTION , "+"       , &apply_add    , glenv);
    BIND(FUNCTION , "*"       , &apply_mul    , glenv);
    BIND(FUNCTION , "-"       , &apply_sub    , glenv);
    BIND(FUNCTION , "/"       , &apply_div    , glenv);
    BIND(FUNCTION , "="       , &apply_eqnum  , glenv);
    BIND(FUNCTION , "length"  , &apply_length , glenv);
    BIND(FUNCTION , "display" , &apply_print  , glenv);
    BIND(FUNCTION , "list?"   , &apply_listq  , glenv);
    BIND(FUNCTION , "pair?"   , &apply_pairq  , glenv);
    BIND(SYNTAX   , "if"      , &apply_if     , glenv);
    BIND(SYNTAX   , "quote"   , &apply_quote  , glenv);

}
