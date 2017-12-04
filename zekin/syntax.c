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
    while(1){
        if(is_nil(args))
            break;
        if(is_false(args->pair->car))
            return new(BOOLEAN , false);
        args = args->pair->cdr;
        if(!args) error("apply add on pair");
    }
    return args->pair->car;
}

Obj apply_or(Obj args , Obj env){
    if(!args) return new(BOOLEAN , false);
    while(1){
        if(is_nil(args))
            break;
        if(!is_false(args->pair->car))
            return args->pair->car;
        args = args->pair->cdr;
        if(!args) error("apply add on pair");
    }
    return new(BOOLEAN , false);
}

Obj apply_define(Obj args , Obj env){
    //assert args == 2
    if(args->pair->car->type == SYMBOL){
        add_symbol(args->pair->car , eval(args->pair->cdr->pair->car , env) , env);
        return NULL;
    }
    else if(args->pair->car->type == PAIR){
        if(is_list(args->pair->car)){
            puts("not ready yet"); // short form
        }
        else{ // var args
        }
        return NULL;
    }
    else{
        printf("def with a non-sym/non-pair obj : ");
        print_obj(args->pair->car) , puts("");
        return NULL;
    }
}

Obj apply_lambda(Obj args , Obj env){
    // assert arity = 2
    return new(CLOSURE ,
            new(EXPR , NULL ,
                args->pair->car ,
                args->pair->cdr->pair->car) , env);
}
