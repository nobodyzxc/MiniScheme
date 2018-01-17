#include "opt.h"
#include "util.h"
#include "eval.h"
#include "syntax.h"
#include "mem.h"
#define TAB_SIZE 1000

/* symbol aloc opt */

int hash_cnt = 0;

MapObj sym_hashtab[TAB_SIZE] = {
    [0 ... TAB_SIZE - 1].key = NULL ,
    [0 ... TAB_SIZE - 1].val = NULL ,
};

/* djb2 ref from
 * http://www.cse.yorku.ca/~oz/hash.html */
unsigned long
hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c;
    /* hash * 33 + c */
    return hash;
}


Obj lookup_sym_pool(char *s){
    int hc = hash(s) % TAB_SIZE;
    while(sym_hashtab[hc].key &&
            !EQS(sym_hashtab[hc].key , s))
        hc++;
    return sym_hashtab[hc].key ?
        sym_hashtab[hc].val : NULL;
}

void push_sym_pool(Obj sym){
    if(hash_cnt > TAB_SIZE){
        puts("cannot opt : tab size is not enough");
        return;
    }
    int hc = hash(sym->str) % TAB_SIZE;
    while(sym_hashtab[hc].key) hc++;
    sym_hashtab[hc].key = sym->str;
    sym_hashtab[hc].val = sym;
    hash_cnt++;
}

Obj find_tail(Obj , Obj , Obj);
/* tail call opt */
Obj build_tail(Obj clos , Obj expr , Obj env){
    if(IS_SELFEVAL(expr)
            || IS_EXPR_OF(expr , define)
            || IS_EXPR_OF(expr , quote)
            || IS_EXPR_OF(expr , set)
            ){
        return new(CLOSURE ,
                new(EXPR ,
                    NULL , /* name */
                    eval(expr , env) , /* args */
                    NULL) , /* body */
                NULL);
    }
    else if(IS_PAIR(expr)){
        Obj app = car(expr);
        app = app->type == SYMBOL ?
                lookup_symbol(app->str , env) : eval(app , env);
        /* speed up beg , why? */
        if(app->type == SYNTAX) /* consider quote? */
            return build_tail(clos , app->proc->apply(cdr(expr) , env) , env);
        if(app->type == MACRO)
            return build_tail(clos , apply_macro(app , cdr(expr) , env) , env);
        /* speed up end */
        clos_args(clos) = map_eval(cdr(expr) , env);
        clos_body(clos) = app;
        return clos;
    }
    printf("cannot do tail eval : ");
    print_obj(expr); puts("");
    return NULL;
}

Obj find_last_expr(Obj exprs , Obj env){
    Obj last_expr = NULL;
    while(!IS_NIL(exprs)){
        if(last_expr) eval(last_expr , env);
        last_expr = car(exprs);
        exprs = cdr(exprs);
    }
    return last_expr;
}

Obj find_tail(Obj clos , Obj body , Obj env){
    return build_tail(clos , find_last_expr(body , env) , env);
}
