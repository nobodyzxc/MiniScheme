#include "mem.h"
#include "eval.h"
#include "util.h"
#include "func.h"
#include "syntax.h"
#include "opt.h"

Obj map_eval(Obj ls , Obj env){
    cons_t head;
    Cons last = &head;
    for( ; !IS_NIL(ls) ; ls = cdr(ls)){
        Obj obj = eval(car(ls) , env);
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
        if(elt && (elt->type == SYNTAX || elt->type == MACRO)){
            printf("bad syntax : ");
            print_obj(elt) , puts("");
            elt = NULL;
        }
        return elt;
    }
    else if(val->type == PAIR){ //bug here
        Obj app = car(val);
        Obj args = cdr(val);
        if(app->type == SYMBOL || app->type == PAIR){

            if(!is_list(args))
                error("cannot apply procedure on pair");

            app = app->type == SYMBOL ?
                lookup_symbol(app->str , env) : eval(app , env);

            if(!app) return NULL;

            if(app->type == SYNTAX)
                return app->proc->apply(args , env);
            else if(app->type == MACRO)
                return apply_macro(app , args , env);

            args = map_eval(args , env); //consider cost of space
            if(!args) return NULL;

            if(app->type == FUNCTION)
                return args ? app->proc->apply(args , env) : NULL;
            else if(app->type == CLOSURE){
                //return args ? apply_clos(app , args , env) : NULL;
                /* closure of tail form */
                app = new(CLOSURE , new(EXPR , NULL , args , app) , env);
                Obj tail = app->clos->exp->expr->body;
                env = new_ENV(env);
                /* ^ keep env clean */
                while(tail){
                    args = app->clos->exp->expr->args;
                    if(tail->type == FUNCTION)
                        return tail->proc->apply(args , env);
                    else if(tail->type == CLOSURE){
                        Obj body = tail->clos->exp->expr->body;
                        Obj pars = tail->clos->exp->expr->args;
                        /* fix fatal bug : use zip instead of zipped */
                        env = zip_env(pars , args , env);
                        app = find_tail(app , body , env);
                    }
                    else{
                        printf("TCO cannot apply tail");
                        return NULL;
                    }
                    tail = app->clos->exp->expr->body;
                }
                return app->clos->exp->expr->args;
            }
        }
        printf("cannot apply : ") , print_obj(app) , puts("");
        return NULL;
    }
    else return val;
}
