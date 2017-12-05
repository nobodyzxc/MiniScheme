#include "mem.h"
#include "eval.h"
#include "util.h"

Obj apply_if(Obj args , Obj env){
    //consider arity == 1
    //assert arity <= 3
    Obj cdr = args->pair->cdr;
    Obj predict = eval(args->pair->car , env);
    if(!is_false(predict))
        return eval(cdr->pair->car , env);
    else if (length(args) > 2)
        return eval(cdr->pair->cdr->pair->car , env);
    return NULL;
}

Obj apply_quote(Obj args , Obj env){
    //assert arity == 1
    return args->pair->car;
}

Obj apply_and(Obj args , Obj env){
    if(!args) return new(BOOLEAN , true);
    while(!is_nil(args)){
        if(is_false(args->pair->car))
            return new(BOOLEAN , false);
        args = args->pair->cdr;
        if(!args) error("apply add on pair");
    }
    return args->pair->car;
}

Obj apply_or(Obj args , Obj env){
    if(!args) return new(BOOLEAN , false);
    while(!is_nil(args)){
        if(!is_false(args->pair->car))
            return args->pair->car;
        args = args->pair->cdr;
        if(!args) error("apply add on pair");
    }
    return new(BOOLEAN , false);
}

Obj apply_define(Obj args , Obj env){
    //assert args == 2
    Obj car = args->pair->car;
    Obj cdr = args->pair->cdr;
    if(car->type == SYMBOL)
        add_symbol(car , eval(cdr->pair->car , env) , env);
    else if(car->type == PAIR) // func short form
        add_symbol(car->pair->car , new(CLOSURE , new(EXPR , NULL ,
                        car->pair->cdr , cdr->pair->car) , env) , env);
    else
        alert("def with a non-sym/non-pair obj : " , args->pair->car);
    return NULL;
}

Obj apply_lambda(Obj args , Obj env){
    // assert arity = 2
    return new(CLOSURE ,
            new(EXPR , NULL ,
                args->pair->car ,
                args->pair->cdr->pair->car) , env);
}
