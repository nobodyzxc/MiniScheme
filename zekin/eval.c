#include "mem.h"
#include "eval.h"
#include "util.h"
#include "func.h"

Obj map_eval(Obj ls , Obj env){
    cons_t head;
    Cons last = &head;
    for( ; !is_nil(ls) ; ls = ls->pair->cdr){
        Obj obj = eval(ls->pair->car , env);
        if(!obj) return NULL; // let gc do free
        last->cdr = new(PAIR , new_cons(obj , NULL));
        last = last->cdr->pair;
    }
    last->cdr = (Obj)nil;
    return head.cdr;
}

Obj eval(Obj val , Obj env){
    if(!val) return NULL;
    if(val->type == SYMBOL){
        Obj elt = lookup_symbol(val->str , env);
        if(elt && elt->type == SYNTAX){
            printf("bad syntax : ");
            print_obj(elt) , puts("");
            elt = NULL;
        }
        return elt;
    }
    else if(val->type == PAIR){ //bug here
        Obj app = val->pair->car;
        Obj args = val->pair->cdr;
        if(app->type == SYMBOL || app->type == PAIR){
            if(!is_list(args)) error("cannot apply procedure on pair");


            app = app->type == SYMBOL ?
                lookup_symbol(app->str , env) : eval(app , env);

            if(!app) return NULL;

            if(app->type == SYNTAX)
                return app->proc->apply(args , env);
            args = map_eval(args , env); //consider cost of space
            if(app->type == FUNCTION)
                return args ? app->proc->apply(args , env) : NULL;
            else if(app->type == CLOSURE)
                return args ? apply_clos(app , args , env) : NULL;
        }
        printf("cannot apply : ") , print_obj(app) , puts("");
        return NULL;
    }
    else return val;
}
