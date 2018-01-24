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

Obj set_clos(Obj clos , Obj args , Obj body){
    clos_args(clos) = args , clos_body(clos) = body;
    return clos;
}

Obj find_tail(Obj , Obj , Obj);

/* tail call opt ,
 * build_tail rely on gcc's TCO XD ,
 * or I need rewrite it to loop */
Obj build_tail(Obj clos , Obj expr , Obj env){
    if(is_selfeval(expr))
        return set_clos(clos , expr , NULL);
    else if(is_symbol(expr))
        return set_clos(clos , eval(expr , env) , NULL);
    else if(is_pair(expr)){
        Obj app = car(expr) , args = cdr(expr);
        app = app->type == SYMBOL ?
            lookup_symbol(app->str , env) : eval(app , env);

        if(app == NULL)
            return set_clos(clos , alert("cannot not apply " , app) , NULL);
        else if(app->type == SYNTAX) /* consider quote */{
            return app->proc->apply == apply_quote ?
                set_clos(clos , cadr(expr) , NULL) :
                build_tail(clos , app->proc->apply(args , env) , env);
        }

        else if(app->type == MACRO)
            return build_tail(clos , apply_macro(app , args , env) , env);

        args = map_eval(args , env);

        return set_clos(clos ,
                app->type == FUNCTION ?
                app->proc->apply(args , env) : args ,
                app->type == FUNCTION ?  NULL : app);
    }
    return set_clos(clos , alert("cannot do tail eval : " , expr) , NULL);
}

Obj find_last_expr(Obj exprs , Obj env){
    Obj last_expr = NULL;
    while(not_nil(exprs)){
        if(last_expr) eval(last_expr , env);
        last_expr = car(exprs);
        exprs = cdr(exprs);
    }
    return last_expr;
}

Obj find_tail(Obj clos , Obj body , Obj env){
    return build_tail(clos , find_last_expr(body , env) , env);
}
