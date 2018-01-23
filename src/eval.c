#include "mem.h"
#include "eval.h"
#include "util.h"
#include "func.h"
#include "syntax.h"
#include "opt.h"

#include <assert.h>

Obj map_eval(Obj ls , Obj env){
    cons_t head;
    Cons last = &head;
    for( ; not_nil(ls) ; ls = cdr(ls)){
        Obj obj = eval(car(ls) , env);
        if(obj == err) return (Obj)err;
        last->cdr = new(PAIR , new_cons(obj , NULL));
        last = last->cdr->pair;
    }
    last->cdr = (Obj)nil;
    return head.cdr;
}

Obj eval_symbol(Obj val , Obj env){
    Obj elt = lookup_symbol(val->str , env);
    if(elt && (elt->type == SYNTAX
                || elt->type == MACRO)){
        printf("bad syntax : ");
        detail(elt) , puts("");
        elt = NULL;
    }
    return elt;
}

Obj tco(Obj args , Obj clos , Obj env){
    Obj tail = new(CLOSURE ,
            new(EXPR , NULL ,
                args , /* return args as tail call's Ans */
                clos), /* contain tail call's pars & def */
            env);
    bool is_tail = false;
    while(clos && clos != err
            && clos_args(tail) != err
            && clos->type == CLOSURE){
        tail = find_tail(tail , clos_body(clos) ,
                zip_env(
                    clos_args(clos) ,
                    clos_args(tail) ,
                    is_tail ? clos_env(clos) :
                    new(ENV , clos_env(clos))));
        is_tail = clos == clos_body(tail);
        clos = clos_body(tail);
    }
    if(clos && clos != err) alert("not a procedure : " , clos);
    return clos || args == err ? (Obj)err : clos_args(tail);
}

Obj eval(Obj val , Obj env){
    if(!val || val == err)
        return val;
    else if(val->type == SYMBOL)
        return eval_symbol(val , env);
    else if(val->type == PAIR){
        Obj app = car(val) , args = cdr(val);
        if(!is_list(args))
            return alert("func call should be list , got" , val);
        else if(app->type == SYMBOL || app->type == PAIR){

            app = app->type == SYMBOL ?
                lookup_symbol(app->str , env) : eval(app , env);

            if(app == err)
                return (Obj)err;
            else if(app == NULL)
                return alert("not a procedure : " , app);

            else if(app->type == SYNTAX)
                return
                    app->proc->apply == apply_quote ?
                    app->proc->apply(args , env) :
                    eval(app->proc->apply(args , env) , env);

            else if(app->type == MACRO)
                return eval(apply_macro(app , args , env) , env);

            if((args = map_eval(args , env)) == err)
                return (Obj)err;
            if(app->type == FUNCTION)
                return app->proc->apply(args , env);
            else if(app->type == CLOSURE){
                env = clos_env(app);
#ifndef TCO_OPT
                return apply_clos(app , args , env);
#endif
                return tco(args , app , env);
            }
        }
        return alert("not a procedure : " , app);
    }
    else return val;
}
