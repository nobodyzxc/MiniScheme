#include "macro.h"
#include "type.h"
#include "util.h"
#include "proc.h"
#include "memory.h"

int least_elts(Obj pats){
    int cnt = 0;
    for(Obj t = pats ; t && not_nil(t) &&
            cdr(t) != nil ; t = cdr(t)){
        cnt += 1;
        if(car(t) == eli) cnt -= 1;
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
#ifdef LISTLEN_OPT
    int list_len = 0;
#endif
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
#ifdef LISTLEN_OPT
            list_len++;
#endif
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
#ifdef LISTLEN_OPT
            list_len++;
#endif
            car(last->cdr) = car(sub);
            for(Obj it = cdr(sub) ;
                    iterable(it) ; it = cdr(it)){
                last = last->cdr->pair;
                last->cdr = new(PAIR , new_cons(NULL , NULL));
#ifdef LISTLEN_OPT
            list_len++;
#endif
                car(last->cdr) = car(it);
            }
        }
        else{
            last->cdr = new(PAIR , new_cons(NULL , NULL));
#ifdef LISTLEN_OPT
            list_len++;
#endif
            car(last->cdr) = substitute(unit , pat , tab);
        }
        last = last->cdr->pair;
    }
    last->cdr = (Obj)nil;
#ifdef LISTLEN_OPT
    remark_len(head.cdr , list_len);
#endif
    return head.cdr;
}

Obj expand(Obj rule , Obj args){
    Obj pat = car(rule) , tml = cadr(rule);
    pat = cdr(pat); /* discard _ */
    Obj tab = zip_pat(pat , args , new(ENV , NULL));
    return substitute(tml , pat , tab);
}
