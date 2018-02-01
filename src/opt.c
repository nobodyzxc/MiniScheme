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

        args = map_eval(args , env ,
                clos_tr(clos) ? clos_args(clos) :
                NULL); /* todo : space opt ? */
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

        alert("" , clos_args(tail));
        fflush(stderr);

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

typedef struct objls_tag *Objls;
typedef struct arg_elt  *Argelt;

struct objls_tag{
    Obj val;
    Objls next;
};

struct arg_elt{
    Obj argpr;
    Objls objs;
    Argelt next;
}

struct Argelt argdb[10] = { [0 ... 9] = NULL };

static int args_rest = 0;
static Obj args_head = NULL;

Obj aloc_arg_cell(){
    Obj aloc = (Obj)MALLOC(sizeof(obj_t));
    aloc->type = PAIR;
    aloc->pair = new_cons(NULL , nil);
    return aloc;
}

Obj extend_cell(Obj head , int len){
//    if(head){
//        alert("continue : " , head);
//    }
//    else{
//        printf("ori ") , fflush(stdout);
//    }
    if(head && !is_pair(head))
        alert("extend non-pair " , head) , exit(0);
    Obj rtn = NULL;
    if(head){
        rtn = head;
        len--;
        while(iterable(cdr(head)))
            len-- , head = cdr(head);
    }
    while(len){
        Obj aloc = aloc_arg_cell();
        if(!head)
            rtn = head = aloc;
        else{
            cdr(head) = aloc;
            head = cdr(head);
        }
        len--;
    }
    cdr(head) = (Obj)nil;
//    alert("--> " , rtn); fflush(stderr);
    return rtn;
}

int clear_list(Obj head){
    int len = 0;
    while(iterable(head))
        len++ , car(head) = NULL , head = cdr(head);
    return len;
}

Obj append_list(Obj pre , Obj rest){
    if(!iterable(pre)) return rest;
    Obj iter = pre;
    while(iterable(cdr(iter)))
        iter = cdr(iter);
    cdr(iter) = rest;
    return pre;
}

//Obj cut_rest(int len){
//    /* assert len > 0 */
//    Obj rtn = args_head;
//    Obj iter = args_head;
//    args_rest -= len;
//    while(--len)
//        iter = cdr(iter);
//    args_head = cdr(iter);
//    if(args_head == nil)
//        args_head = NULL;
//    cdr(iter) = (Obj)nil;
//
////    alert("cut " , rtn);
////    alert("rest " , args_head);
//    return rtn;
//}

Obj args_aloc(int len){
    if(!len) return (Obj)nil;

    if(!args_head)
        return extend_cell(NULL , len);
    else if(len <= args_rest)
        return cut_rest(len);
    else{
        Obj head = extend_cell(args_head , len);
        args_rest = 0 , args_head = NULL;
        return head;
    }
}

void args_rles(Obj head){
    int len = clear_list(head);
    if(len){
        args_rest += len;
        args_head = append_list(args_head , head);
    }
//    alert("release cur head : " , args_head);
//    fflush(stderr);
}
