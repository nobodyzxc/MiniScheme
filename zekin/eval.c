#include "eval.h"
#include "mem.h"
#define new_pair(a , b) new_pair(a , b , __FILE__ ":" xstr(__LINE__))

Obj lookup_symbol(char *p , Env env){
    Obj func = lookup_buildins(p);
    if(func) return func;
    printf("<symbol:%s> not found" , p);
    return NULL;
}

Pair map_eval(Pair ls , Env env){
    pair_t head;
    Pair last = &head;
    for(Pair it = ls ; it ; it = it->cdr){
        Obj obj = eval(it->car , env);
        if(!obj){
            free_pair(head.cdr);
            return NULL;
        }
        last->cdr = new_pair(obj , NULL);
        last = last->cdr;
    }
    return head.cdr;
}

Obj eval(Obj val , Env env){
    if(!val) return NULL;
    if(val->type == SYMBOL)
        return lookup_symbol(val->symbol , env);
    else if(val->type != PAIR)
        return val;
    else if(val->type == PAIR){
        Obj car = val->pair->car;
        Pair cdr = val->pair->cdr;
        if(car->type != SYMBOL){
            printf("cannot apply : ") , print_obj(car);
            return NULL;
        }
        if(is_keyw(car->symbol)){
            if(EQS(car->symbol , "if")){
                Obj tf = eval(cdr->car , env); //leak
                if(tf->type == BOOLEAN
                        && tf->boolean == false)
                    return eval(cdr->cdr->cdr->car , env);
                else
                    return eval(cdr->cdr->car , env);
            }
            else if(EQS(car->symbol , "quote")){
                return cdr->car;
            }
            //define
            //lambda?
            //cond?
            //let?
            //letrec?
        }
        else if(is_proc(car->symbol)){
            if(!is_list(cdr)) error("cannot apply function on pair");
            Pair args = map_eval(cdr , env);
            if(!args) return NULL;
            else{ //for mem leak
                val = eval(car , env)->proc.func(args);
                free_pair(args);
                return val;
            }
        }
    }
}
