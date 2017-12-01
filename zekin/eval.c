#include "mem.h"
#include "eval.h"
#include "util.h"
#include "func.h"

Obj map_eval(Cons ls , Obj env){
    cons_t head;
    Cons last = &head;
    for(Cons it = ls ; it ; it = it->cdr){
        Obj obj = eval(it->car , env);
        if(!obj){
            free_cons(head.cdr);
            return NULL;
        }
        last->cdr = new_cons(obj , NULL);
        last = last->cdr;
    }
    return new(PAIR , head.cdr);
}

Obj eval(Obj val , Obj env){
    if(!val) return NULL;
    if(val->type == SYMBOL)
        return lookup_symbol(val->str , env);
    else if(val->type == PAIR){ //bug here
        Obj app = val->pair->car;
        Cons args = val->pair->cdr;
        if(app->type == SYMBOL || app->type == PAIR){
            if(!is_list(args)) error("cannot apply procedure on pair");
            Obj pcr = app->type == SYMBOL ?
                lookup_symbol(app->str , env) : eval(app , env);
            if(!pcr) return NULL;
            if(pcr->type == SYNTAX){
                return pcr->proc->apply(args , env);
            }
            else if(pcr->type == FUNCTION){
                Obj args_obj = map_eval(args , env); //consider cost of space
                return args_obj ? pcr->proc->apply(args_obj->pair , env) : NULL;
            }
            else if(pcr->type == CLOSURE){
                //zip env and eval expr
                Obj args_obj = map_eval(args , env);
                if(args_obj == NULL) return NULL;
                args = args_obj->pair;
                Clos pcr_clos = pcr->clos;
                Expr pcr_expr = pcr_clos->expr->expr;
                if(length(args) != length(pcr_expr->args->pair))
                    //if(length(args) != length(pcr->clos->expr->expr->args->pair))
                    error("unmatched args");
                Obj zipped_env = new(ENV , pcr->clos->env); // if add current env ?
                Cons param = pcr->clos->expr->expr->args->pair;
                while(param && !is_nil(param->car)){
                    add_symbol(param->car , args->car , zipped_env);
                    param = param->cdr , args = args->cdr;
                }
                return eval(pcr->clos->expr->expr->body , zipped_env);
            }
        }
        printf("cannot apply : ") , print_obj(app) , puts("");
        return NULL;
    }
    else return val;
}
