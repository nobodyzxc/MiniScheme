#include "eval.h"
#include "mem.h"
#define new_pair(a , b) new_pair(a , b , __FILE__ ":" xstr(__LINE__))
#define free_pair(a)    free_pair(a , __FILE__ ":" xstr(__LINE__))
#define free_obj(a) free_obj(a , __FILE__ ":" xstr(__LINE__))

Obj lookup_symbol(char *p , Env env){
    Obj func = lookup_buildins(p);
    if(func) return copy_obj(func); //think twice
    printf("<symbol:%s> not found\n" , p);
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
        return copy_obj(val); //think twice
    else if(val->type == PAIR){
        Obj car = val->pair->car;
        Pair cdr = val->pair->cdr;
        if(car->type != SYMBOL){
            printf("cannot apply : ") , print_obj(car) , puts("");
            return NULL;
        }
        else if(is_keyw(car->symbol)){
            if(EQS(car->symbol , "if")){
                Obj tf = eval(cdr->car , env); //leak
                if(tf->type == BOOLEAN
                        && tf->boolean == false)
                    return eval(cdr->cdr->cdr->car , env);
                else
                    return eval(cdr->cdr->car , env);
            }
            else if(EQS(car->symbol , "quote")){
                return copy_obj(cdr->car); //think twice
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
                Obj apply = eval(car , env); //think twice
                val = apply->proc.func(args);
                //free_pair_shallow(args);
                free_pair(args); //think twice
                free_obj(apply);
                return val;
            }
        }
        else{
            printf("cannot apply : %s\n" , car->symbol);
            return NULL;
        }
    }
}
