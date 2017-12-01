#include "eval.h"
#include "mem.h"
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
        Obj car = val->pair->car;
        Cons cdr = val->pair->cdr;
        if(car->type == SYMBOL){
            if(!is_list(cdr)) error("cannot apply procedure on pair");
            Obj pcr = lookup_symbol(car->str , env);
            if(!pcr) return NULL;
            if(pcr->type == SYNTAX)
                return pcr->proc->apply(cdr , env);
            else if(pcr->type == FUNCTION)
                return pcr->proc->apply(map_eval(cdr , env)->pair , env);
        }
        printf("cannot apply : ") , print_obj(car) , puts("");
        return NULL;
    }
    else return val;
}
