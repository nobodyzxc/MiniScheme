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
    if(val->type == SYMBOL)
        return lookup_symbol(val->str , env);
    else if(val->type == PAIR){ //bug here
        Obj app = val->pair->car;
        Obj args = val->pair->cdr;
        if(app->type == SYMBOL || app->type == PAIR){
            if(!is_list(args)) error("cannot apply procedure on pair");
            Obj pcr = app->type == SYMBOL ?
                lookup_symbol(app->str , env) : eval(app , env);
            if(!pcr) return NULL;
            if(pcr->type == SYNTAX){
                return pcr->proc->apply(args , env);
            }
            else if(pcr->type == FUNCTION){
                args = map_eval(args , env); //consider cost of space
                return args ? pcr->proc->apply(args , env) : NULL;
            }
            else if(pcr->type == CLOSURE){
                //zip env and eval expr
                args = map_eval(args , env);
                if(args == NULL) return NULL;
                Clos pcr_clos = pcr->clos;
                Expr pcr_expr = pcr_clos->exp->expr;
                if(length(args) != length(pcr_expr->args))
                    error("unmatched args");
                Obj zipped_env = new(ENV , pcr->clos->env); // if add current env ?
                Obj param = pcr->clos->exp->expr->args;
                while(param && !is_nil(param)){
                    add_symbol(param->pair->car , args->pair->car , zipped_env);
                    param = param->pair->cdr , args = args->pair->cdr;
                }
                return eval(pcr->clos->exp->expr->body , zipped_env);
            }
        }
        printf("cannot apply : ") , print_obj(app) , puts("");
        return NULL;
    }
    else return val;
}
