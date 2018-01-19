#include "mem.h"
#include "eval.h"
#include "util.h"
#include "opt.h"

bool match(Obj keyws , Obj patn , Obj args);

int least_elts(Obj pats){
    int cnt = 0;
    for(Obj t = pats ; t && t != nil &&
            cdr(t) != nil ; t = cdr(t)){
        cnt += 1;
        if(EQS(car(t)->str , "..."))
            cnt -= 1;
    }
    return cnt;
}

bool has_eli(Obj ls){
    for( ; ls && !IS_NIL(ls) ; ls = cdr(ls))
        if(car(ls) == eli) return true;
    return false;
}

bool eli_match(Obj keyws , Obj patn , Obj largs){
    bool v = true;
    for( ; largs && !IS_NIL(largs) ; largs = cdr(largs))
        v &= match(keyws , patn , car(largs));
    return v;
}

bool match(Obj keyws , Obj patn , Obj args){
    int v = true;
    if(patn->type == SYMBOL){
        if(lssym(keyws , patn)) /* literal identifier */
            v &= args->type == SYMBOL && EQS(patn->str , args->str);
        else /* pattern varible */
            v &= args != nil;
    }
    else if(patn->type == PAIR){
        if(has_eli(patn)){
            int psym_len = length(patn) - 2; /* 'rest '... */
            ASSERT(psym_len >= 0 , "misplaced ellipsis in pattern");
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
    //if(!v){
    //    printf("mismatch ") , print_obj(patn) , printf(" , ");
    //    print_obj(args) , puts("");
    //}
    return v;
}

Obj substitute(Obj tml , Obj pat , Obj tab){
    cons_t head;
    Cons last = &head;
    if(tml->type != PAIR){
        return tml->type == SYMBOL && lssym_rec(pat , tml) ?
            lookup_symbol(tml->str , tab) : tml;
    }
    for( ; tml && !IS_NIL(tml) ; tml = cdr(tml)){
        Obj unit = car(tml);
        Obj next = IS_NIL(cdr(tml)) ? NULL : (cadr(tml));
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
            if(IS_NIL(sub)) continue;
            last->cdr = new(PAIR , new_cons(NULL , NULL));
            car(last->cdr) = car(sub);
            for(Obj it = cdr(sub) ;
                    !IS_NIL(it) ; it = cdr(it)){
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
    //print_symtree(tab->env->symtab);
    //print_obj(pat) , puts("");
    //print_obj(args) , puts("");
    //print_obj(tml) , puts("");
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
    printf("\n > ");
    detail(macro);
    printf("\n > ");
    print_obj(args);
    puts("");
    exit(0);
    return NULL;
}

Obj apply_if(Obj args , Obj env){
    //consider arity == 1
    //assert arity <= 3
    Obj rest = cdr(args);
    Obj predict = eval(car(args) , env);
    if(!IS_FALSE(predict))
        return car(rest);
    else if (length(args) > 2)
        return cadr(rest);
    return NULL;
}

Obj apply_quote(Obj args , Obj env){
    //assert arity == 1
    return car(args);
}

Obj apply_define(Obj args , Obj env){
    //assert args == 2
    Obj id = car(args);
    Obj expr = cdr(args);
    if(id->type == SYMBOL){
        Obj val = eval(car(expr) , env);
        add_symbol(id , val , env);
    }
    else if(id->type == PAIR) // func short form
        add_symbol(car(id) , new(CLOSURE , new(EXPR , NULL ,
                        cdr(id) , expr) , env) , env);
    else
        alert("def with a non-sym/non-pair obj : " , id);
    return NULL;
}

Obj apply_lambda(Obj args , Obj env){
    // assert arity = 2
    // assert car(args) all symbol
    return new(CLOSURE ,
            new(EXPR , NULL ,
                car(args) , cdr(args)) , env);
}

Obj apply_define_syntax(Obj args , Obj env){
    return NULL;
}

Obj apply_syntax_rules(Obj args , Obj env){
    // assert arity = 2
    return new(MACRO , car(args) , cdr(args));
}

Obj apply_set(Obj args , Obj env){
    // assert id == SYM
    Obj id = car(args);
    Obj expr = eval(cadr(args) , env);
    if(!eval) return NULL;
    if(env = lookup_symenv(id->str , env)){
        add_symbol(id , expr , env);
    }
    else printf("cannot find symbol");
    return NULL;
}
