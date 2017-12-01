#include "eval.h"
#include "func.h"

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
/*
   else if(is_keyw(car->str)){
   printf("key word %s\n" , car->str);
   if(EQS(car->str , "if")){
   Obj tf = eval(cdr->car , env);
   if(tf->type == BOOLEAN
   && tf->boolean == false)
   return eval(cdr->cdr->cdr->car , env);
   else
   return eval(cdr->cdr->car , env);
   }
   else if(EQS(car->str , "quote")){
   return copy_obj(cdr->car); //think twice
   }
   return NULL;
   }
   */
