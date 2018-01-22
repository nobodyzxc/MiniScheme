#include "mem.h"
#include "eval.h"
#include "util.h"
#include "func.h"
#include "syntax.h"
#include "opt.h"

#include <assert.h>


Obj map_eval(Obj ls , Obj env){
    cons_t head;
    Cons last = &head;
    /* assert ls must be list */
    for( ; not_nil(ls) ; ls = cdr(ls)){
        Obj obj = eval(car(ls) , env);
        if(obj == err) return (Obj)err;
        last->cdr = new(PAIR , new_cons(obj , NULL));
        last = last->cdr->pair;
    }
    last->cdr = (Obj)nil;
    return head.cdr;
}

Obj eval_symbol(Obj val , Obj env){
    Obj elt = lookup_symbol(val->str , env);
    if(elt && (elt->type == SYNTAX
                || elt->type == MACRO)){
        printf("bad syntax : ");
        detail(elt) , puts("");
        elt = NULL;
    }
    return elt;
}

Obj eval(Obj val , Obj env){
    while(1){
        if(!val || val == err) return val;
        if(val->type == SYMBOL)
            return eval_symbol(val , env);
        else if(val->type == PAIR){
            Obj app = car(val) , args = cdr(val);
            if(app->type == SYMBOL || app->type == PAIR){

                if(!is_list(args)){
                    alert("last elt of proc call "
                            "expects nil , got " , args);
                    return (Obj)err;
                }

                app = app->type == SYMBOL ?
                    lookup_symbol(app->str , env) : eval(app , env);

                if(!app || app == err) return (Obj)err;

                if(app->type == SYNTAX){
                    val = app->proc->apply(args , env);
                    if(app->proc->apply == apply_quote)
                        return val;
                    continue;
                }
                else if(app->type == MACRO){
                    val = apply_macro(app , args , env);
                    continue;
                }

                if((args = map_eval(args , env)) == err) return (Obj)err;
                assert(args != NULL);
                //consider cost of space
                if(app->type == FUNCTION)
                    return app->proc->apply(args , env);
                else if(app->type == CLOSURE){
                    //return apply_clos(app , args , env);
                    env = clos_env(app);
                    /* important !
                     * use clos_env
                     * instead of env */
                    Obj tail = app;
                    /* tco opt */
                    app = new(CLOSURE , new(EXPR , NULL , args , app) , env);
                    /* closure of tail form ^ , expr in clos :
                     * expr's args : ((1 , 2 , 3)[iter args] ,
                     * expr's body : (f a b c) . (+ a b c) , env)[tail] , env)
                     * */
                    env = new_ENV(env);
                    /* ^ keep original clos's env clean */
                    bool re_eval = false;
                    bool is_tail = false;
                    while(tail){
                        args = clos_args(app);
                        if(args == err || tail == err) break;
                        if(tail->type == FUNCTION)
                            return tail->proc->apply(args , env);
                        else if(tail->type == SYNTAX){
                            val = tail->proc->apply(args , env);
                            if(app->proc->apply == apply_quote)
                                return val;
                            if(app->proc->apply == apply_lambda)
                                return app->proc->apply(args , env);
                            re_eval = true;
                            break;
                        }
                        else if(tail->type == MACRO){
                            val = apply_macro(tail , clos_args(app) , env);
                            re_eval = true;
                            break;
                        }
                        else if(tail->type == CLOSURE){
                            //env = zip_env(clos_args(tail) , args ,
                            //            is_tail ? env : new(ENV , env));
                            //app = find_tail(app , clos_body(tail) , env);
                            env = clos_env(tail);
                            app = find_tail(app , clos_body(tail) ,
                                    zip_env(clos_args(tail) , args ,
                                        is_tail ? env : new(ENV , env)));
                        }
                        else{
                            alert("cannot not apply" , tail);
                            puts(" on TCO");
                            break;
                        }
                        is_tail = tail == clos_body(app);
                        tail = clos_body(app);
                    }
                    if(re_eval) continue;
                    else if(tail == err) return (Obj)err;
                    else if(tail) alert("TCO cannot apply " , tail);
                    return tail ? (Obj)err : clos_args(app);
                }
            }
            printf("cannot apply : ") , print_obj(app) , puts("");
            return (Obj)err;
        }
        else return val;
    }
}
