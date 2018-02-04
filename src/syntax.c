#include "memory.h"
#include "eval.h"
#include "util.h"
#include "opt.h"

Obj apply_if(Obj args , Obj env , Obj set){
    int len = length(args);
    if(len > 3 || len < 2)
        alert("if : accepts 2 to 3 args , got " , args);
    else{
        Obj rest = cdr(args);
        Obj predict = eval(car(args) , env , NULL);
        if(!is_false(predict)) /* important */
            return car(rest);  /* DON'T USE is_true */
        else if (len > 2)
            return cadr(rest);
        else
            return NULL;
    }
    return (Obj)err;
}

Obj apply_quote(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("quote : only accepts 1 args , got " , args);
    else
        return car(args);
    return (Obj)err;
}

Obj apply_define(Obj args , Obj env , Obj set){
    if(length(args) < 2)
        alert("define : accepts at least 2 args , got " , args);
    else{
        Obj id = car(args) , expr = cdr(args);
        if(id == err || expr == err)
            return (Obj) err;
        else if(id->type == SYMBOL){
            if(length(expr) != 1)
                return alert("define : id should only be"
                        " followd by 1 expr"" , got " , expr);
            else if(add_symbol(id ,
                        eval(car(expr) , env , NULL) , env) == err)
                return (Obj) err;
            else return NULL;
        }
        else if(id->type == PAIR){ /* func short form */
            if(add_symbol(
                        prid(id) ,
                        eval(new_nested_lambda(id , expr) , env , NULL) ,
                        env) != err);
            return NULL;
        }
        else alert("def with a non-sym/non-pair obj : " , id);
    }
    return (Obj)err;
}

Obj apply_lambda(Obj args , Obj env , Obj set){
    if(length(args) < 2)
        alert("lambda : accepts at least 2 args , got " , args);
    else if(is_symbol(car(args)) || is_sympr(car(args))){
        return new(CLOSURE ,
                new(EXPR , NULL ,
                    car(args) , cdr(args)) , env);
    }
    else
        alert("lambda args contains non-symbol atom : "
                , car(args));
    return (Obj)err;
}

Obj apply_define_syntax(Obj args , Obj env , Obj set){
    puts("not ready yet");
    return (Obj)err;
}

Obj apply_syntax_rules(Obj args , Obj env , Obj set){
    if(length(args) < 1)
        alert("syntax-rules : accepts at least 1 arg , got "
                , args);
    else
        return new(MACRO , car(args) , cdr(args));
    return (Obj)err;
}

Obj apply_set(Obj args , Obj env , Obj set){
    if(length(args) < 2)
        alert("set! : accepts at least 2 args , got " , args);
    else{
        Obj id = car(args);
        if(!id || id->type != SYMBOL)
            alert("set! : only accepts symbol as id , got "
                    , id);
        else{
            Obj expr = eval(cadr(args) , env , NULL);
            env = lookup_symenv(id->str , env);
            if(expr != err && env != err){
                if(add_symbol(id , expr , env) != err)
                    return NULL;
            }
        }
    }
    return (Obj)err;
}

Obj apply_set_cdr(Obj args , Obj env , Obj set){
    if(length(args) != 2)
        alert("set-cdr! : only accepts 2 args , got " , args);
    else{
        Obj ls = eval(car(args) , env , NULL);
        Obj tail = eval(cadr(args) , env , NULL);
        if(!is_pair(ls))
            alert("set-cdr! : first arg must be pair , got " , args);
        else if(tail != err){
            cdr(ls) = tail;
            return NULL;
        }
    }
    return (Obj)err;
}

Obj apply_set_car(Obj args , Obj env , Obj set){
    if(length(args) != 2)
        alert("set-car! : only accepts 2 args , got " , args);
    else{
        Obj ls = eval(car(args) , env , NULL);
        Obj head = eval(cadr(args) , env , NULL);
        if(!is_pair(ls))
            alert("set-car! : first arg must be pair , got " , ls);
        else if(head != err){
            car(ls) = head;
            return NULL;
        }
    }
    return (Obj)err;
}
