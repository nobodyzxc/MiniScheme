#include "opt.h"
#include "util.h"
#include "eval.h"
#include "syntax.h"
#include "mem.h"

#define TAB_SIZE 1000

typedef struct map_obj_tag MapObj;

struct map_obj_tag{
    char *key;
    Obj val;
};

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
        return set_clos(clos , eval(expr , env , NULL) , NULL);
    else if(is_pair(expr)){
        Obj app = car(expr) , args = cdr(expr);
        app = app->type == SYMBOL ?
            lookup_symbol(app->str , env) : eval(app , env , NULL);

        if(app == NULL)
            return set_clos(clos , alert("cannot not apply " , app) , NULL);
        else if(app->type == SYNTAX) /* consider quote */{
            return app->proc->apply == apply_quote ?
                set_clos(clos , cadr(expr) , NULL) :
                build_tail(clos , app->proc->apply(args , env , NULL) , env);
        }

        else if(app->type == MACRO)
            return build_tail(clos , eval_macro(app , args , env) , env);

        if(clos_tr(clos)){
            int len = length(args);
            if(len){
                Argelt ae = args_aloc(len);
                map_eval(args , env , ae->args);
                args_copy(clos_args(clos) , ae->args);
                args = clos_args(clos);
                args_rles(ae);
            }
            else
                args = map_eval(args , env , NULL);
            /* todo : space opt ? */
        }
        else{
            args = map_eval(args , env , NULL);
            /* todo : space opt ? */
        }

        return set_clos(clos ,
                app->type == FUNCTION ? /* consider it */
                app->proc->apply(args , env , NULL) : args ,
                app->type == FUNCTION ?  NULL : app);
    }
    return set_clos(clos , alert("cannot do tail eval : " , expr) , NULL);
}

Obj find_last_expr(Obj exprs , Obj env){
    Obj last_expr = NULL;
    while(not_nil(exprs)){
        if(last_expr) eval(last_expr , env , NULL);
        last_expr = car(exprs);
        exprs = cdr(exprs);
    }
    return last_expr;
}

Obj find_tail(Obj clos , Obj body , Obj env){
    if(env == err) return env;
    return build_tail(clos , find_last_expr(body , env) , env);
}

Obj tco(Obj clos , Obj args , Obj env){

    Obj tail = new(CLOSURE ,
            new(EXPR , NULL ,
                args , /* return args as tail call's Ans */
                clos), /* contain tail call's pars & def */
            env);
    bool reuse_env = false;
    Obj eval_env = (Obj)err;
    while(clos && clos != err
            && clos_args(tail) != err
            && clos->type == CLOSURE){

        eval_env =
            reuse_env ?
            eval_env :
            /* clos_env(clos) : */
            new(ENV , clos_env(clos));

        tail = find_tail(tail , clos_body(clos) ,
                zip_env(
                    clos_args(clos) ,
                    clos_args(tail) ,
                    eval_env));

        Obj new_clos =
            tail == err ?
            tail : clos_body(tail);

        if(is_clos(new_clos)){
            if(env_ref(eval_env)){
                reuse_env = false;
                alert("be ref after : " , clos_body(clos));
            }
            else{
                reuse_env = clos == new_clos;
            }
            if(clos != new_clos)
                puts("not equal clos");
            clos_tr(tail) = reuse_env;
        }
        clos = new_clos;
    }
    if(clos && clos != err && !is_clos(clos))
        alert("not a procedure : " , clos);
    return clos || args == err ? (Obj)err : clos_args(tail);
}

/* space opt , args list */
#define OPTLEN 10

Argelt argdb[OPTLEN] = {
    [0 ... (OPTLEN - 1)] = NULL };

int arrange_arg(Argelt arg){
    int i = 0;
    Obj iter = arg->args;
    Objls oter = arg->objs;
    while(iterable(iter)){
        car(iter) = oter->val;
        oter = oter->next;
        iter = cdr(iter);
        i++;
    }
    return i;
}

Obj new_pr(){
    Obj pr = (Obj)MALLOC(sizeof(obj_t));
    pr->type = PAIR;
    pr->pair = new_cons(NULL , NULL);
    return pr;
}

Obj new_args(int len){
    Obj head = new_pr();
    Obj iter = head;
    while(--len){
        cdr(iter) = new_pr();
        iter = cdr(iter);
    }
    cdr(iter) = (Obj)nil;
    return head;
}

Objls new_objls_elt(){
    Objls rtn = (Objls)MALLOC(sizeof(objls_t));
    rtn->val = (Obj)MALLOC(sizeof(obj_t));
    rtn->val->type = INTEGER;
    rtn->val->integer = 0;
    rtn->next = NULL;
    return rtn;
}

Objls new_objs(int len){
    Objls head = new_objls_elt();
    Objls iter = head;
    while(--len){
        iter->next = new_objls_elt();
        iter = iter->next;
    }
    return head;
}

Argelt new_arg_elt(int len){
    Argelt aloc = (Argelt)MALLOC(sizeof(arg_elt_t));
    aloc->args = new_args(len);
    aloc->objs = new_objs(len);
    aloc->next = NULL;
    arrange_arg(aloc);
    return aloc;
}

Argelt args_aloc(int len){
    if(!len) puts("error args_aloc") , exit(0);
    if(len < OPTLEN)
        if(argdb[len]){
            Argelt rtn = argdb[len];
            argdb[len] = argdb[len]->next;
            return rtn;
        }
    return new_arg_elt(len);
}

void free_arg(Argelt arg){
    while(iterable(arg->args)){
        Obj dis = arg->args;
        arg->args = cdr(arg->args);
        FREE(dis->pair);
        FREE(dis);
    }
    while(arg->objs){
        Objls dis = arg->objs;
        arg->objs = arg->objs->next;
        FREE(dis->val);
        FREE(dis);
    }
    FREE(arg);
    puts(" too long") , exit(1);
}

void args_rles(Argelt arg){
    int len = arrange_arg(arg);
    if(len < OPTLEN){
        arg->next = argdb[len];
        argdb[len] = arg;
    }
    else free_arg(arg);
}

void args_copy(Obj tar , Obj par){
    while(iterable(tar)){
        *car(tar) = *car(par);
        tar = cdr(tar);
        par = cdr(par);
    }
}
