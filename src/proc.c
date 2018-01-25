#include <stdlib.h>
#include "util.h"
#include "proc.h"
#include "mem.h"
#include "func.h"
#include "syntax.h"
#include "opt.h"

Obj lambda_symbol = NULL;

#define LoR(node , v) \
    (v > 0 ? &((node)->lt) : &((node)->rt))
#define cmp_node(s , n) \
    (strcmp((s) , (n)->sym->str))

Obj  add_symbol(Obj sym , Obj val , Obj env_obj){

    if(sym != err && !is_symbol(sym))
        alert("define only accepts symbol , got " , sym);
    else if(sym != err && val != err){
        Symtree iter = env_obj->env->symtab;
        if(!iter)
            env_obj->env->symtab =
                new_symtree(sym , val , NULL , NULL);
        else{
            while(1){
                int v = cmp_node(sym->str , iter);
                Symtree *s = LoR(iter , v);
                if(v && *s)
                    iter = *s;
                else{
                    if(v == 0)
                        iter->val = val;
                    else
                        *s = new_symtree(sym , val , NULL , NULL);
                    return sym;
                }
            }
        }
    }
    return (Obj)err;
}

Obj search_symbol(char *symstr , Obj env_obj){
    while(env_obj){
        Symtree iter = env_obj->env->symtab;
        while(iter){
            int v = cmp_node(symstr , iter);
            if(!v) return iter->val;
            else iter = *LoR(iter , v);
        }
        env_obj = env_obj->env->parent;
    }
    return (Obj)err;
}

Obj lookup_symbol(char *symstr , Obj env_obj){
    Obj rtn = search_symbol(symstr , env_obj);
    if(rtn == err)
        printf("lookup_symbol : cannot find symbol \"%s\"\n" , symstr);
    return rtn;
}

Obj lookup_symenv(char *symstr , Obj env_obj){
    while(env_obj){
        Symtree iter = env_obj->env->symtab;
        while(iter){
            int v = cmp_node(symstr , iter);
            if(!v) return env_obj;
            else iter = *LoR(iter , v);
        }
        env_obj = env_obj->env->parent;
    }
    printf("lookup_symenv : cannot find symbol \"%s\"\n" , symstr);
    return (Obj)err;
}

#define BIND(TYPE , SYM , PROC , ENV) \
    add_symbol(\
            new(SYMBOL , strdup(SYM)) , \
            new(TYPE , strdup(SYM) , PROC) , ENV)

void init_buildins(){
    glenv = new(ENV , NULL);
    BIND(FUNCTION , "+"            , &apply_add          , glenv);
    BIND(FUNCTION , "*"            , &apply_mul          , glenv);
    BIND(FUNCTION , "-"            , &apply_sub          , glenv);
    BIND(FUNCTION , "/"            , &apply_div          , glenv);
    BIND(FUNCTION , "%"            , &apply_mod          , glenv);
    BIND(FUNCTION , "="            , &apply_eqnum        , glenv);
    BIND(FUNCTION , "<"            , &apply_lt           , glenv);
    BIND(FUNCTION , ">"            , &apply_gt           , glenv);
    BIND(FUNCTION , "<="           , &apply_let          , glenv);
    BIND(FUNCTION , ">="           , &apply_get          , glenv);
    BIND(FUNCTION , "mod"          , &apply_mod          , glenv);
    BIND(FUNCTION , "not"          , &apply_not          , glenv);
    BIND(FUNCTION , "cons"         , &apply_cons         , glenv);
    BIND(FUNCTION , "car"          , &apply_car          , glenv);
    BIND(FUNCTION , "cdr"          , &apply_cdr          , glenv);
    BIND(FUNCTION , "length"       , &apply_length       , glenv);
    BIND(FUNCTION , "eval"         , &apply_eval         , glenv);
    BIND(FUNCTION , "apply"        , &apply_apply        , glenv);

    BIND(FUNCTION , "display"      , &apply_display      , glenv);
    BIND(FUNCTION , "source"       , &apply_source       , glenv);
    BIND(FUNCTION , "system"       , &apply_system       , glenv);
    BIND(FUNCTION , "void"         , &apply_void         , glenv);
    BIND(FUNCTION , "void?"        , &apply_voidq        , glenv);
    BIND(FUNCTION , "symbol?"      , &apply_symbolq      , glenv);
    BIND(FUNCTION , "boolean?"     , &apply_booleanq     , glenv);
    BIND(FUNCTION , "number?"      , &apply_numberq      , glenv);
    BIND(FUNCTION , "string?"      , &apply_stringq      , glenv);
    BIND(FUNCTION , "exact?"       , &apply_exactq       , glenv);
    BIND(FUNCTION , "integer?"     , &apply_integerq     , glenv);
    BIND(FUNCTION , "pair?"        , &apply_pairq        , glenv);
    BIND(FUNCTION , "null?"        , &apply_nullq        , glenv);
    BIND(FUNCTION , "list?"        , &apply_listq        , glenv);
    BIND(FUNCTION , "procedure?"   , &apply_procedureq   , glenv);
    BIND(FUNCTION , "closure?"     , &apply_closureq     , glenv);
    BIND(FUNCTION , "env?"         , &apply_envq         , glenv);
    BIND(FUNCTION , "eq?"          , &apply_eqq          , glenv);
    BIND(FUNCTION , "eqv?"         , &apply_eqvq         , glenv);
    BIND(FUNCTION , "equal?"       , &apply_equalq       , glenv);

    BIND(FUNCTION , "read"         , &apply_read         , glenv);

    lambda_symbol =
        BIND(SYNTAX   , "lambda"       , &apply_lambda       , glenv);
    BIND(SYNTAX   , "if"           , &apply_if           , glenv);
    BIND(SYNTAX   , "quote"        , &apply_quote        , glenv);
    BIND(SYNTAX   , "define"       , &apply_define       , glenv);
    BIND(SYNTAX   , "set!"         , &apply_set          , glenv);
    BIND(SYNTAX   , "set-cdr!"     , &apply_set_cdr      , glenv);
    BIND(SYNTAX   , "set-car!"     , &apply_set_car      , glenv);
    BIND(SYNTAX   , "syntax-rules" , &apply_syntax_rules , glenv);

    BIND(FUNCTION , "gc"           , &apply_gc           , glenv);
    BIND(FUNCTION , "exit"         , &apply_exit         , glenv);
    BIND(FUNCTION , "flush-output" , &apply_flush_output , glenv);
    BIND(FUNCTION , "get-env"      , &apply_get_env      , glenv);
    BIND(FUNCTION , "get-curenv"   , &apply_get_curenv   , glenv);
    BIND(FUNCTION , "lookup-symbol", &apply_lookup_symbol, glenv);

    add_symbol(new(SYMBOL , strdup("global")) , glenv    , glenv);
}
