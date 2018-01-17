#include "opt.h"
#include "util.h"
#include "eval.h"
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
            || IS_EXPR_OF(expr , "define")
            || IS_EXPR_OF(expr , "quote")
            || IS_EXPR_OF(expr , "set!")
            || IS_EXPR_OF(expr , "and")
            || IS_EXPR_OF(expr , "or")
            || IS_EXPR_OF(expr , "let")
            || IS_EXPR_OF(expr , "lambda")){
        return new(CLOSURE ,
                new(EXPR ,
                    NULL , /* name */
                    eval(expr , env) , /* args */
                    NULL) , /* body */
                NULL);
    }
    else if(IS_EXPR_OF(expr , "begin")){
        return find_tail(clos , cdr(expr) , env);
    }
    else if(IS_EXPR_OF(expr , "if")){
        return IS_TRUE(eval(cadr(expr) , env)) ?
            build_tail(clos , caddr(expr) , env) :
            (length(expr) == 3 ? NULL :
             build_tail(clos , cadddr(expr) , env));
    }
    else if(IS_PAIR(expr)){
        Obj nbody = car(expr)->type == SYMBOL ?
                lookup_symbol(car(expr)->str , env) : eval(car(expr) , env);
        Obj nargs;
        if(nbody->type == MACRO || nbody->type == SYNTAX)
            //clos_args(clos) = cdr(expr);
            nargs = cdr(expr);
        else
            //clos_args(clos) = map_eval(cdr(expr) , env);
            nargs = map_eval(cdr(expr) , env);
        alert("build call : " , nbody);puts("");
        alert("build call : " , nargs);puts("");

        clos->clos->exp->expr->args =
            map_eval(cdr(expr) , env);
        clos->clos->exp->expr->body =
            eval(car(expr) , env);
        return clos;

        return new(CLOSURE ,
                new(EXPR ,
                    NULL , /* name */
                    nargs , /* args */
                    nbody) , /* body */
                NULL);

        //Obj tail_args =
        //if(clos_body(clos)->type == CLOSURE)
        //else puts("fuck") , exit(0);
        return clos;
    }
    printf("cannot do tail eval : ");
    print_obj(expr); puts("");
    return NULL;
}

Obj find_tail(Obj clos , Obj body , Obj env){
    Obj iter = body , last_expr = NULL;
    while(!IS_NIL(iter)){
        if(last_expr) eval(last_expr , env);
        last_expr = car(iter);
        iter = cdr(iter);
    }
    return build_tail(clos , last_expr , env);
}
