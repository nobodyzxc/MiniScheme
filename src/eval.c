#include "memory.h"
#include "eval.h"
#include "util.h"
#include "func.h"
#include "syntax.h"
#include "util.h"
#include "opt.h"
#include "parse.h"
#include "macro.h"

#include <assert.h>

Obj eval_macro(Obj macro , Obj args , Obj env){
    for(Obj rules = macro->mac->rules ;
            rules && rules != nil ; rules = cdr(rules)){
        Obj rule = car(rules);
        Obj patn = car(rule);
        /* cdr(patn) to discard _ */
        if(match(macro->mac->keyws , cdr(patn) , args)){
            return expand(rule , args);
        }
    }
    msgobj("cannot match any rule\n" , macro);
    return alert("\n--> " , args);
}

Obj eval_clos(Obj pcr , Obj args , Obj env){
    puts("apply clos");
    Obj iter = clos_body(pcr) , val = NULL;
    env = zipped_env(clos_args(pcr) , args , clos_env(pcr));
    while(not_nil(iter))
        val = eval(car(iter) , env , NULL) , iter = cdr(iter);
    /* consider it */
    return val;
}

Obj map_eval(Obj ls , Obj env , Obj set){
    if(set){
        Obj st = set;
        for( ; not_nil(ls) ; ls = cdr(ls) , st = cdr(st)){
            if((car(st) = eval(car(ls) , env , car(st))) == err)
                return (Obj)err;
        }
        return set;
    }
    else{
#ifdef LISTLEN_OPT
        int list_len = 0;
#endif
        cons_t head;
        Cons last = &head;
        for( ; not_nil(ls) ; ls = cdr(ls)){
            Obj obj = eval(car(ls) , env , NULL);
            if(obj == err) return (Obj)err;
            last->cdr = new(PAIR , new_cons(obj , NULL));
            last = last->cdr->pair;
#ifdef LISTLEN_OPT
            list_len++;
#endif
        }
        last->cdr = (Obj)nil;
#ifdef LISTLEN_OPT
        remark_len(head.cdr , list_len);
#endif
        return head.cdr;
    }
}

Obj eval_symbol(Obj val , Obj env){
    Obj elt = lookup_symbol(val->str , env);
    if(elt && (elt->type == SYNTAX
                || elt->type == MACRO)){
        printf("bad syntax : %s\n" , val->str);
        elt = (Obj) err;
    }
    return elt;
}

Obj eval_func(Obj func , Obj args , Obj env , Obj set){
#ifdef ARG_OPT
    int len = length(args);
    Argelt ae = len ? args_aloc(len) : NULL;
#endif
    if((args = map_eval(args , env ,
#ifdef ARG_OPT
                    (len ? ae->args : (Obj)nil)
#else
                    NULL
#endif
                    )) == err)
        return (Obj)err;

    Obj val = func->proc->apply(args , env , set);
#ifdef ARG_OPT
    if(len) args_rles(ae);
#endif
    return val;
}

Obj eval(Obj val , Obj env , Obj set){
    if(is_selfeval(val))
        return val;
    else if(is_symbol(val))
        return eval_symbol(val , env);
    else if(is_pair(val)){
        Obj app = car(val) , args = cdr(val);
        if(!is_list(args)){
#ifdef LISTLEN_OPT
            alert(" : " , args);
            printf("-> %d\n" , args->pair->len);
#endif
            return alert("func call should be list , got " , val);
        }
        else if(is_symbol(app) || is_pair(app)){

            app = is_symbol(app) ?
                lookup_symbol(app->str , env) : eval(app , env , NULL);
            /* consider it */
            if(app == err)
                return (Obj)err;
            else if(app == NULL)
                return alert("not a procedure : " , app);

            else if(app->type == SYNTAX)
                return
                    app->proc->apply == apply_quote ?
                    app->proc->apply(args , env , set) :
                    eval(app->proc->apply(args , env , set) , env , set);
            /* consider it */

            else if(app->type == MACRO)
                return eval(eval_macro(app , args , env) , env , set);
            /* consider it */

            else if(app->type == FUNCTION)
                return eval_func(app , args , env , set);

            else if(app->type == CLOSURE){

                if((args = map_eval(args , env , NULL)) == err)
                    return (Obj)err;

                env = clos_env(app);
#ifdef TCO_OPT
                return tco(app , args , env);
#else
                return eval_clos(app , args , env);
#endif
            }
        }
        return alert("cannot apply : " , app);
    }
    return alert("cannot eval known type : " , val);
}
