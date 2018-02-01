#include "mem.h"
#include "eval.h"
#include "util.h"
#include "func.h"
#include "syntax.h"
#include "util.h"
#include "opt.h"

#include <assert.h>

bool match(Obj keyws , Obj patn , Obj args);

int least_elts(Obj pats){
    int cnt = 0;
    for(Obj t = pats ; t && not_nil(t) &&
            cdr(t) != nil ; t = cdr(t)){
        cnt += 1;
        if(EQS(car(t)->str , "...")) cnt -= 1;
    }
    return cnt;
}

bool has_eli(Obj ls){
    for( ; iterable(ls) ; ls = cdr(ls))
        if(car(ls) == eli) return true;
    return false;
}

bool eli_match(Obj keyws , Obj patn , Obj largs){
    bool v = true;
    for( ; iterable(largs) ; largs = cdr(largs))
        v &= match(keyws , patn , car(largs));
    return v;
}

bool match(Obj keyws , Obj patn , Obj args){
    int v = true;
    if(patn->type == SYMBOL){
        if(lssym(keyws , patn)) /* literal identifier */
            v &= args->type == SYMBOL
                && EQS(patn->str , args->str);
        else /* pattern varible */
            v &= args != nil;
    }
    else if(patn->type == PAIR){
        if(has_eli(patn)){
            int psym_len = length(patn) - 2; /* 'rest '... */
            ASSERT(psym_len >= 0 ,
                    "misplaced ellipsis in pattern");
            if(psym_len > length(args)){
                return false;
            }
            for( ; psym_len ; psym_len-- ,
                    patn = cdr(patn) , args = cdr(args)){
                if(!match(keyws , car(patn) , car(args)))
                    v &= false;
            }
            v &= eli_match(keyws , car(patn) , args);
        }
        else{
            if(is_list(patn)){
                if(length(patn) != length(args))
                    return false;
            }
            else{ /* improper pattern */
                if(pat_num(patn) > pat_num(args))
                    return false;
            }
            for( ; patn && patn->type == PAIR ;
                    patn = cdr(patn) , args = cdr(args))
                if(!match(keyws , car(patn) , car(args)))
                    v &= false;
            if(patn != nil)
                if(!match(keyws , patn , args))
                    v &= false;
            v &= patn == args;
        }
    }
    else{
        v &= patn == args;
    }
    return v;
}

Obj substitute(Obj tml , Obj pat , Obj tab){
    cons_t head;
    Cons last = &head;
    if(tml->type != PAIR){
        return tml->type == SYMBOL && lssym_rec(pat , tml) ?
            lookup_symbol(tml->str , tab) : tml;
    }
    for( ; iterable(tml) ; tml = cdr(tml)){
        Obj unit = car(tml);
        Obj next = is_nil(cdr(tml)) ? NULL : (cadr(tml));
        if(next && next == eli && unit->type != SYMBOL){
            puts("ellipsis must follow a symbol in template");
            return (Obj) err;
        }
        if(unit->type == PAIR){
            last->cdr = new(PAIR , new_cons(NULL , NULL));
            car(last->cdr) = substitute(unit , pat , tab);
        }
        else if(next && next == eli){
            tml = cdr(tml); /* discard ... */
            Obj sub = lookup_symbol(unit->str , tab);
            if(!sub){
                puts("ellipsis must follow pat var");
                return (Obj) err;
            }
            if(is_nil(sub)) continue;
            last->cdr = new(PAIR , new_cons(NULL , NULL));
            car(last->cdr) = car(sub);
            for(Obj it = cdr(sub) ;
                    iterable(it) ; it = cdr(it)){
                last = last->cdr->pair;
                last->cdr = new(PAIR , new_cons(NULL , NULL));
                car(last->cdr) = car(it);
            }
        }
        else{
            last->cdr = new(PAIR , new_cons(NULL , NULL));
            car(last->cdr) = substitute(unit , pat , tab);
        }
        last = last->cdr->pair;
    }
    last->cdr = (Obj)nil;
    return head.cdr;
}

Obj expand(Obj rule , Obj args){
    Obj pat = car(rule) , tml = cadr(rule);
    pat = cdr(pat); /* discard _ */
    Obj tab = zip_pat(pat , args , new(ENV , NULL));
    return substitute(tml , pat , tab);
}

Obj eval_macro(Obj macro , Obj args , Obj env){
    for(Obj rules = macro->mac->rules ;
            rules && rules != nil ; rules = cdr(rules)){
        Obj rule = car(rules);
        Obj patn = car(rule);
        /* cdr(patn) to discard _ */
        if(match(macro->mac->keyws , cdr(patn) , args)){
            return expand(rule , args);
        }
    }
    msgobj("cannot match any rule\n" , macro);
    return alert("\n--> " , args);
}

Obj eval_clos(Obj pcr , Obj args , Obj env){
    puts("apply clos");
    Obj iter = clos_body(pcr) , val = NULL;
    env = zipped_env(clos_args(pcr) , args , clos_env(pcr));
    while(not_nil(iter))
        val = eval(car(iter) , env , NULL) , iter = cdr(iter);
    /* consider it */
    return val;
}

Obj map_eval(Obj ls , Obj env , Obj set){
    if(set){
        Obj st = set;
        for( ; not_nil(ls) ; ls = cdr(ls) , st = cdr(st)){
            if((car(st) = eval(car(ls) , env , car(st))) == err)
                return (Obj)err;
        }
        return set;
    }
    else{
        cons_t head;
        Cons last = &head;
        for( ; not_nil(ls) ; ls = cdr(ls)){
            Obj obj = eval(car(ls) , env , NULL);
            if(obj == err) return (Obj)err;
            last->cdr = new(PAIR , new_cons(obj , NULL));
            last = last->cdr->pair;
        }
        last->cdr = (Obj)nil;
        return head.cdr;
    }
}

Obj eval_symbol(Obj val , Obj env){
    Obj elt = lookup_symbol(val->str , env);
    if(elt && (elt->type == SYNTAX
                || elt->type == MACRO)){
        printf("bad syntax : %s\n" , val->str);
        elt = (Obj) err;
    }
    return elt;
}

Obj eval(Obj val , Obj env , Obj set){
    if(is_selfeval(val))
        return val;
    else if(is_symbol(val))
        return eval_symbol(val , env);
    else if(is_pair(val)){
        Obj app = car(val) , args = cdr(val);
        if(!is_list(args))
            return alert("func call should be list , got " , val);
        else if(is_symbol(app) || is_pair(app)){

            app = is_symbol(app) ?
                lookup_symbol(app->str , env) : eval(app , env , NULL);
            /* consider it */
            if(app == err)
                return (Obj)err;
            else if(app == NULL)
                return alert("not a procedure : " , app);

            else if(app->type == SYNTAX)
                return
                    app->proc->apply == apply_quote ?
                    app->proc->apply(args , env , set) :
                    eval(app->proc->apply(args , env , set) , env , set);
            /* consider it */

            else if(app->type == MACRO)
                return eval(eval_macro(app , args , env) , env , set);
            /* consider it */

 //           puts("map eval new vvv") , fflush(stdout);

#ifdef ARG_OPT
            Argelt ae = NULL;
            int len = 0;
            if(app->type == FUNCTION){
                len = length(args);
                if(len) ae = args_aloc(len);
            }
#endif
            if((args = map_eval(args , env ,
                            app->type == CLOSURE ? NULL :
#ifdef ARG_OPT
                            (len ? ae->args : (Obj)nil)
#else
                            NULL
#endif
                            )) == err)
                return (Obj)err;  /* consider it */

//            puts("map eval end ^^^") , fflush(stdout);

            if(app->type == FUNCTION){
                msgobjc("apply " , app);
                msgobjc(" with " , args);
                Obj val = app->proc->apply(args , env , set);
#ifdef ARG_OPT
                if(len) args_rles(ae);
#endif
                msgobj(" = " , val);
                return val;
            }
            else if(app->type == CLOSURE){
                env = clos_env(app);
#ifdef TCO_OPT
                return tco(app , args , env);
#else
                return eval_clos(app , args , env);
#endif
            }
        }
        return alert("cannot apply : " , app);
    }
    return alert("cannot eval known type : " , val);
}
