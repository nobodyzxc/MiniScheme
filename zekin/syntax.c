#include "mem.h"
#include "eval.h"
#include "util.h"

Obj apply_if(Cons args , Obj env){
    //consider arity == 1
    Cons cdr = args->cdr;
    Obj predict = eval(args->car , env);
    if(predict->type == BOOLEAN
            && predict->boolean == false)
        return eval(cdr->cdr->car , env);
    else
        return eval(cdr->car , env);
}

Obj apply_quote(Cons args , Obj env){
    return args->car;
}

Obj apply_and(Cons args , Obj env){
    if(!args) return new(BOOLEAN , true);
    while(1){
        if(is_false(args->car))
            return new(BOOLEAN , false);
        if(is_nil(args->cdr->car))
            break;
        args = args->cdr;
        if(!args) error("apply add on pair");
    }
    return args->car;
}

Obj apply_or(Cons args , Obj env){
    if(!args) return new(BOOLEAN , false);
    while(1){
        if(!is_false(args->car))
            return args->car;
        if(is_nil(args->cdr->car))
            break;
        args = args->cdr;
        if(!args) error("apply add on pair");
    }
    return new(BOOLEAN , false);
}

Obj apply_define(Cons args , Obj env){
    //assert args == 2
    if(args->car->type == SYMBOL){
        add_symbol(args->car , eval(args->cdr->car , env) , env);
        return NULL;
    }
    else if(args->car->type == PAIR){
        puts("not ready yet");
        return NULL;
    }
    else{
        error("bad syntax");
    }
}

Obj apply_lambda(Cons args , Obj env){
    // assert arity = 2
    return new(CLOSURE , new(EXPR , NULL , args->car , args->cdr->car) , env);
}
