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

Obj find_tail(Obj , Obj);
/* tail call opt */
Obj build_tail(Obj expr , Obj env){
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
                env);
    }
    else if(IS_EXPR_OF(expr , "begin")){
        return find_tail(cdr(expr) , env);
    }
    else if(IS_EXPR_OF(expr , "if")){
        return IS_TRUE(eval(cadr(expr) , env)) ?
            build_tail(caddr(expr) , env) :
            (length(expr) == 3 ? NULL :
             build_tail(cadddr(expr) , env));
    }
    //else if(IS_EXPR_OF(expr , "cond")){}
    //else if(IS_EXPR_OF(expr , "let")){}
    else if(IS_PAIR(expr)){
        return new(CLOSURE ,
                new(EXPR ,
                    NULL , /* name */
                    map_eval(cdr(expr) , env) , /* args */
                    eval(car(expr) , env)) , /* body */
                env);
    }
    printf("cannot tail eval : ");
    print_obj(expr); puts("");
    return NULL;
}

Obj find_tail(Obj body , Obj env){
    Obj iter = body , last = NULL;
    while(!IS_NIL(iter)){
        if(last) eval(last , env);
        last = car(iter);
        iter = cdr(iter);
    }
    return build_tail(last , env);
}
