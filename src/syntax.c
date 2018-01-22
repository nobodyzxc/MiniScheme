#include "mem.h"
#include "eval.h"
#include "util.h"
#include "opt.h"

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
        if(next && next == eli && unit->type != SYMBOL)
            error("ellipsis must follow a symbol in template");
        if(unit->type == PAIR){
            last->cdr = new(PAIR , new_cons(NULL , NULL));
            car(last->cdr) = substitute(unit , pat , tab);
        }
        else if(next && next == eli){
            tml = cdr(tml); /* discard ... */
            Obj sub = lookup_symbol(unit->str , tab);
            if(!sub) error("ellipsis must follow pat var");
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

Obj apply_macro(Obj macro , Obj args , Obj env){
    for(Obj rules = macro->mac->rules ;
            rules && rules != nil ; rules = cdr(rules)){
        Obj rule = car(rules);
        Obj patn = car(rule);
        /* cdr(patn) to discard _ */
        if(match(macro->mac->keyws , cdr(patn) , args)){
            return expand(rule , args);
        }
    }
    printf("cannot match any rule");
    printf("\n > ") , detail(macro);
    return alert("\n > " , args);
}

Obj apply_if(Obj args , Obj env){
    int len = length(args);
    if(len > 3 || len < 2)
        alert("if : accepts 2 to 3 args , got " , args);
    else{
        Obj rest = cdr(args);
        Obj predict = eval(car(args) , env);
        if(!is_false(predict)) /* important */
            return car(rest);  /* DON'T USE is_true */
        else if (len > 2)
            return cadr(rest);
        else
            return NULL;
    }
    return (Obj)err;
}

Obj apply_quote(Obj args , Obj env){
    if(length(args) != 1)
        alert("quote : only accepts 1 args , got " , args);
    else
        return car(args);
    return (Obj)err;
}

Obj apply_define(Obj args , Obj env){
    if(length(args) < 2)
        alert("define : accepts at least 2 args , got " , args);
    else{
        Obj id = car(args) , expr = cdr(args);
        if(id->type == SYMBOL){
            if(length(expr) == 1){
                add_symbol(id , eval(car(expr) , env) , env);
                return NULL;
            }
            else alert("define : id should only be"
                    " followd by 1 expr"" , got" , expr);
        }
        else if(id->type == PAIR){ /* func short form */
            //add_symbol(car(id) , new(CLOSURE ,
            //            new(EXPR , NULL , cdr(id) , expr) , env) , env);
            //return NULL;
            add_symbol(prid(id) ,
                    eval(
                        new_nested_lambda(id , expr) , env)
                    , env);
            return NULL;
        }
        else alert("def with a non-sym/non-pair obj : " , id);
    }
    return (Obj)err;
}

Obj apply_lambda(Obj args , Obj env){
    if(length(args) < 2)
        alert("lambda : accepts at least 2 args , got " , args);
    else if(is_symbol(car(args)) || is_sympr(car(args)))
        return new(CLOSURE ,
                new(EXPR , NULL ,
                    car(args) , cdr(args)) , env);
    else
        alert("lambda args contains non-symbol atom : "
                , car(args));
    return (Obj)err;
}

Obj apply_define_syntax(Obj args , Obj env){
    puts("not ready yet");
    return (Obj)err;
}

Obj apply_syntax_rules(Obj args , Obj env){
    if(length(args) < 1)
        alert("syntax-rules : accepts at least 1 arg , got "
                , args);
    else
        return new(MACRO , car(args) , cdr(args));
    return (Obj)err;
}

Obj apply_set(Obj args , Obj env){
    if(length(args) < 2)
        alert("set! : accepts at least 2 args , got " , args);
    else{
        Obj id = car(args);
        if(!id || id->type != SYMBOL)
            alert("set! : only accepts symbol as id , got "
                    , id);
        else{
            Obj expr = eval(cadr(args) , env);
            env = lookup_symenv(id->str , env);
            if(expr != err && env != err){
                if(add_symbol(id , expr , env) != err)
                    return NULL;
            }
        }
    }
    return (Obj)err;
}

Obj apply_set_cdr(Obj args , Obj env){
    if(length(args) != 2)
        alert("set-cdr! : only accepts 2 args , got " , args);
    else{
        Obj ls = eval(car(args) , env);
        Obj tail = eval(cadr(args) , env);
        if(!is_pair(ls))
            alert("set-cdr! : first arg must be pair , got " , args);
        else if(tail != err){
            cdr(ls) = tail;
            return NULL;
        }
    }
    return (Obj)err;
}

Obj apply_set_car(Obj args , Obj env){
    if(length(args) != 2)
        alert("set-car! : only accepts 2 args , got " , args);
    else{
        Obj ls = eval(car(args) , env);
        Obj head = eval(cadr(args) , env);
        if(!is_pair(ls))
            alert("set-car! : first arg must be pair , got " , ls);
        else if(head != err){
            car(ls) = head;
            return NULL;
        }
    }
    return (Obj)err;
}
