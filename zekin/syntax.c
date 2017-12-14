#include "mem.h"
#include "eval.h"
#include "util.h"

int least_elts(Obj pats){
    int cnt = 0;
    for(Obj t = pats ; t && t != nil &&
            t->pair->cdr != nil ; t = t->pair->cdr){
        cnt += 1;
        if(EQS(t->pair->car->str , "..."))
            cnt -= 1;
    }
    return cnt;
}

bool match(Obj keyws , Obj patn , Obj args){
    patn = cdr(patn); /* discard _ */
    //alert("patn is " , patn);
    if(least_elts(patn) > length(args)) return false;
    Obj t = patn , k = args;
    Obj t_elt = t , k_elt = NULL;
    for( ; t && k && t != nil && k != nil ;
            t = t->pair->cdr , k = k->pair->cdr){
        t_elt = t->pair->car;
        k_elt = k->pair->car;
        if(EQS(t_elt->str , "...")) return true;;
        if(lssym(keyws , t_elt))
            if(k_elt->type != SYMBOL ||
                    strcmp(k_elt->str , t_elt->str))
                return false;
    }
    if(t->type == PAIR && EQS(car(t)->str , "...")){
        return true;
    }
    return t == k;
}

Obj substitute(Obj tml , Obj pat , Obj tab){
    cons_t head;
    Cons last = &head;
    //alert("tml is " , tml);
    //alert("pat is " , pat);
    //exit(0);
    if(tml->type != PAIR)
        return tml->type == SYMBOL && lssym(pat , tml) ?
            lookup_symbol(tml->str , tab) : tml;
    for( ; !is_nil(tml) ; tml = tml->pair->cdr){
        Obj unit = tml->pair->car;
        if(unit->type == PAIR){
            last->cdr = new(PAIR , new_cons(NULL , NULL));
            last->cdr->pair->car = substitute(unit , pat , tab);
        }
        else if(unit->type == SYMBOL && lssym(pat , unit)){
            Obj sub = lookup_symbol(unit->str , tab);
            if(EQS(unit->str , "...")){
                if(is_nil(sub)) continue;
                last->cdr = new(PAIR , new_cons(NULL , NULL));
                last->cdr->pair->car = car(sub);
                for(Obj it = cdr(sub) ;
                        !is_nil(it) ; it = cdr(it)){
                    last = last->cdr->pair;
                    last->cdr = new(PAIR , new_cons(NULL , NULL));
                    last->cdr->pair->car = it->pair->car;
                }
            }
            else{
                last->cdr = new(PAIR , new_cons(NULL , NULL));
                last->cdr->pair->car = sub;
            }
        }
        else{
            last->cdr = new(PAIR , new_cons(NULL , NULL));
            last->cdr->pair->car = unit;
        }
        last = last->cdr->pair;
    }
    last->cdr = (Obj)nil;
    return head.cdr;
}

Obj expand(Obj rule , Obj args){
    Obj pat = car(rule) , tml = cadr(rule);
    pat = cdr(pat); /* discard _ */
    Obj tab = zipped_pat(pat , args , NULL);
//    print_obj(pat) , puts("");
//    print_obj(args) , puts("");
//    print_obj(tml) , puts("");
    return substitute(tml , pat , tab);
}

Obj apply_macro(Obj macro , Obj args , Obj env){
    // go go go
    //for syntax rules
    for(Obj rules = macro->mac->rules ;
            rules && rules != nil ; rules = rules->pair->cdr){
        Obj rule = car(rules);
        Obj patn = car(rule);
        if(match(macro->mac->keyws , patn , args)){
            Obj expansion = expand(rule , args);
            alert("expand : " , expansion);
            return eval(expansion , env);
        }
    }
    printf("cannot match any rule");
    exit(0);
    return NULL;
}

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
                        car->pair->cdr , cdr) , env) , env);
    else
        alert("def with a non-sym/non-pair obj : " , args->pair->car);
    return NULL;
}

Obj apply_lambda(Obj args , Obj env){
    // assert arity = 2
    return new(CLOSURE ,
            new(EXPR , NULL ,
                args->pair->car ,
                args->pair->cdr) , env);
}

Obj apply_define_syntax(Obj args , Obj env){
    return NULL;
}

Obj apply_syntax_rules(Obj args , Obj env){
    // assert arity = 2
    return new(MACRO ,
            args->pair->car ,
            args->pair->cdr);
}
