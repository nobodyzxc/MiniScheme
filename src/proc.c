#include <stdlib.h>
#include "util.h"
#include "proc.h"
#include "mem.h"
#include "func.h"
#include "syntax.h"
#include "opt.h"

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

Obj lookup_symenv(char *v , Obj env_obj){
    while(env_obj){
        Symtree iter = env_obj->env->symtab;
        while(iter){
            int df = strcmp(iter->sym->str , v);
            if(!df) return env_obj;
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
    glenv = new(ENV , NULL);
    BIND(FUNCTION , "cons"       , &apply_cons      , glenv);
    BIND(FUNCTION , "apply"      , &apply_apply     , glenv);
    BIND(FUNCTION , "null?"      , &apply_nullq     , glenv);
    BIND(FUNCTION , "+"          , &apply_add       , glenv);
    BIND(FUNCTION , "*"          , &apply_mul       , glenv);
    BIND(FUNCTION , "-"          , &apply_sub       , glenv);
    BIND(FUNCTION , "/"          , &apply_div       , glenv);
    BIND(FUNCTION , "%"          , &apply_mod       , glenv);
    BIND(FUNCTION , "="          , &apply_eqnum     , glenv);
    BIND(FUNCTION , "<"          , &apply_lt        , glenv);
    BIND(FUNCTION , ">"          , &apply_gt        , glenv);
    BIND(FUNCTION , ">="         , &apply_let       , glenv);
    BIND(FUNCTION , ">="         , &apply_get       , glenv);
    BIND(FUNCTION , "mod"        , &apply_mod       , glenv);
    BIND(FUNCTION , "not"        , &apply_not       , glenv);
    BIND(FUNCTION , "car"        , &apply_car       , glenv);
    BIND(FUNCTION , "cdr"        , &apply_cdr       , glenv);
    BIND(FUNCTION , "length"     , &apply_length    , glenv);
    BIND(FUNCTION , "display"    , &apply_display   , glenv);
    BIND(FUNCTION , "list?"      , &apply_listq     , glenv);
    BIND(FUNCTION , "pair?"      , &apply_pairq     , glenv);
    BIND(FUNCTION , "source"     , &apply_source    , glenv);
    BIND(FUNCTION , "void"       , &apply_void      , glenv);
    BIND(FUNCTION , "void?"      , &apply_voidq     , glenv);
    BIND(FUNCTION , "eq?"        , &apply_eqq       , glenv);
    BIND(FUNCTION , "eqv?"       , &apply_eqvq      , glenv);
    BIND(FUNCTION , "equal?"     , &apply_equalq    , glenv);
    BIND(FUNCTION , "symbol?"    , &apply_symbolq   , glenv);
    BIND(FUNCTION , "procedure?" , &apply_procedureq, glenv);
    BIND(FUNCTION , "read"       , &apply_read      , glenv);

    BIND(SYNTAX ,  "if"         , &apply_if        , glenv);
    BIND(SYNTAX ,  "quote"      , &apply_quote     , glenv);
    BIND(SYNTAX ,  "lambda"     , &apply_lambda    , glenv);
    BIND(SYNTAX ,  "define"     , &apply_define    , glenv);
    BIND(SYNTAX ,  "set!"       , &apply_set       , glenv);

    add_symbol(new(SYMBOL , strdup("global")) , glenv , glenv);
    BIND(SYNTAX   , "syntax-rules"  , &apply_syntax_rules , glenv);
    BIND(FUNCTION   , "gc"       , &apply_gc        , glenv);
    BIND(FUNCTION , "senv"       , &apply_senv      , glenv);
}
