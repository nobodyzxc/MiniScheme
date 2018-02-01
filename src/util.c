#include "mem.h"
#include "type.h"
#include "util.h"
#include "proc.h"
#include <stdlib.h>
#define max(a , b) (a > b ? a : b)

void print_short_pair(Obj obj);

Obj lssym_rec(Obj ls , Obj sym){
    if(sym->type != SYMBOL) return NULL;
    for(Obj it = ls ; it &&
            it != nil ; it = cdr(it))
        if(car(it)->type == SYMBOL){
            if(EQS(car(it)->str , sym->str))
                return it;
        }
        else if(car(it)->type == PAIR){
            Obj s = lssym_rec(car(it) , sym);
            if(s) return s;
        }
    return NULL;
}

Obj lssym(Obj ls , Obj sym){
    if(sym->type != SYMBOL) return NULL;
    for(Obj it = ls ; it &&
            it != nil ; it = cdr(it))
        if(car(it)->type == SYMBOL)
            if(EQS(car(it)->str , sym->str))
                return it;
    return NULL;
}

Obj lsobj(Obj ls , Obj obj){
    for(Obj it = ls ; it &&
            it != nil ; it = cdr(it))
        if(car(it) == obj) return it;
    return NULL;
}

/* ues ca*r find the inner symbol in nested ls */
Obj prid(Obj ls){
    if(!is_pair(ls)) return (Obj)err;
    while(is_pair(ls)) ls = car(ls);
    return is_symbol(ls) ? ls : (Obj)err;
}

/* check all elt in pr is all sym ,
 * accepts nil but not accepts sym */
bool is_sympr(Obj ls){
    if(!is_pair(ls) && !is_nil(ls))
        return false;
    for( ; ls && iterable(ls) ; ls = cdr(ls))
        if(!is_symbol(car(ls))) return false;
    return true;
}

void fprint_obj_dtl(FILE *s , Obj obj){
    if(is_clos(obj)){
        fprintf(s , "<closure ");
        fprint_obj(s , clos_args(obj));
        fprintf(s , " . ");
        fprint_obj(s , clos_body(obj));
        fprintf(s , ">");
    }
    else if(is_macro(obj)){
        fprintf(s , "<macro ");
        fprint_obj(s , mac_keys(obj));
        fprintf(s , " . ");
        for(Obj it = mac_rules(obj) ;
                iterable(it) ; it = cdr(it)){
            falert(s , "\n  (" , caar(it));
            falert(s , "\n    " , cadar(it));
            fprintf(s , ")");
        }
        fprintf(s , ">");
    }
    else if(is_env(obj)){
        fprintf(s , "================ENV================\n");
        fprint_symtree(s , obj->env->symtab);
        fprintf(s , "===================================");
    }
    else if(is_port(obj)){
        fprintf(s , "<%s-port %s ptr:%s>" ,
            (is_port_of(obj , "r") ? "in" : "out") ,
            port_name(obj) ,
            port_ptr(obj) ? port_ptr(obj) : "NULL");
    }
    else{
        fprint_obj(s , obj);
    }
}

void fprint_symtree(FILE *s , Symtree tree){
    if(tree == NULL) return;
    fprint_symtree(s , tree->rt);
    fprintf(s , "'%-15s : " , tree->sym->str);
    fprint_obj(s , tree->val);
    fprintf(s , "\n");
    fprint_symtree(s , tree->lt);
}

Obj map_car(Obj ls){
    cons_t head;
    Cons last = &head;
    for( ; iterable(ls) ; ls = cdr(ls)){
        last->cdr = new(PAIR , new_cons(caar(ls) , NULL));
        last = last->cdr->pair;
    }
    last->cdr = (Obj)nil;
    return head.cdr;
}

Obj map_cdr(Obj ls){
    cons_t head;
    Cons last = &head;
    for( ; iterable(ls) ; ls = cdr(ls)){
        last->cdr = new(PAIR , new_cons(cdar(ls) , NULL));
        last = last->cdr->pair;
    }
    last->cdr = (Obj)nil;
    return head.cdr;
}

/* cannot use it. macro args will be mutable */
void set_map_cdr(Obj ls){
    for( ; iterable(ls) ; ls = cdr(ls))
        car(ls) = cdr(car(ls));
}

Obj zip_pat(Obj pat , Obj args , Obj env);

Obj zip_elipat(Obj pat , Obj args , Obj env){
    if(pat->type == SYMBOL)
        add_symbol(pat , args , env);
    else if(pat->type == PAIR){
        for( ; iterable(pat) ; pat = cdr(pat)){
            zip_elipat(car(pat) , map_car(args) , env);
            args = map_cdr(args); /* bug : set_map_cdr(args) */
        }
    }
    else{
        error("wrong eli zip type");
    }
    return env;
}

Obj zip_pat(Obj pat , Obj args , Obj env){
    Obj p = args;
    bool isls = is_list(pat);
    while(pat->type == PAIR){
        Obj mch = car(pat);
        Obj nxt = cdr(pat);
        nxt = nxt ? (nxt->type == PAIR ? car(nxt) : nxt) : NULL;
        if(nxt == eli){
            return zip_elipat(mch , args , env);
        }

        if(mch->type == PAIR){ /* rec zip */
            zip_pat(mch , car(args) , env);
        }
        else if(mch->type == SYMBOL){
            if(is_nil(args)) error("is nil? in util.c");
            add_symbol(mch , car(args) , env);
        }
        pat = cdr(pat);
        args = cdr(args);
    }
    return env;
}

Obj zip_env(Obj syms , Obj args , Obj env){
    //assert args is list
    bool isls = is_list(syms);
    int argslen = length(args);
    if(argslen == -1)
        return alert("zip_env : args must be list , "
                "got " , args);
    if(isls && length(syms) != argslen ||
            !isls && pat_num(syms) > argslen){
        printf("unmatched args: ");
        isls ? print_short_pair(syms) : print_obj(syms);
        printf(" <- ") , print_short_pair(args) , puts("");
        return (Obj)err;
    }
    while(syms->type == PAIR){
        add_symbol(car(syms), car(args), env);
        syms = cdr(syms) , args = cdr(args);
    }
    if(not_nil(syms))
        add_symbol(syms , args , env);
    return env;
}

bool is_list(Obj pr){
    while(pr && pr->type == PAIR)
        pr = cdr(pr);
    return is_nil(pr);
}

bool cmp_num(Obj a , Obj b){
    if(!is_num(a) || !is_num(b))
        return false;
    if(a->type == b->type)
        return num_of(a) == num_of(b);
    else
        return (double)num_of(a) == (double)num_of(b);
}

bool eqv(Obj a , Obj b){
    if(a == b)
        return true;
    if(a->type != b->type)
        return false;
    switch(a->type){
        case INTEGER:
        case DECIMAL:
            return cmp_num(a , b);
        case STRING:
            return EQS(a->str , b->str);
        default:
            return false;
    }
}

bool equal(Obj a , Obj b){
    if(a->type != b->type)
        return false;
    if(eqv(a , b)) return true;
    if(a->type == PAIR){
        if(is_list(a) != is_list(b)) return false;
        if(pat_num(a) != pat_num(b)) return false;
        for( ; iterable(a) ; a = cdr(a) , b = cdr(b))
            if(!equal(car(a) , car(b))) return false;
        return true;
    }
    return false;
}

int pat_num(Obj pr){
    int rtn = 0;
    while(iterable(pr))
        rtn ++ , pr = cdr(pr);
    return rtn;
}

int length(Obj pr){
    int rtn = 0;
    if(!is_list(pr))
        return -1;
    while(not_nil(pr))
        rtn++ , pr = cdr(pr);
    return rtn;
}

Obj cons(kObj head , kObj body){
    return new(PAIR , new_cons(head , body));
}

void fprint_pair(FILE *s , kObj pr){
    fprintf(s , "(");
    fprint_obj(s , car(pr));
    pr = cdr(pr);
    while(is_pair(pr))
        fprintf(s , " ") , fprint_obj(s , car(pr)) , pr = cdr(pr);
    if(!is_nil(pr))
        fprintf(s , " . ") , fprint_obj(s , pr);
    fprintf(s , ")");
}

void fprint_esc_str(FILE *f , char* s){
    char *k = "abfnrtv\\'?\"" , *v = "\a\b\f\n\r\t\v\\\'\?\"";
    for(fprintf(f , "\"") ; *s ; s++)
        if(strchr(v , *s))
            fprintf(f , "\\%c" , k[strchr(v , *s) - v]);
        else
            fprintf(f , "%c" , *s);
    fprintf(f , "\"");
}

void fprint_obj(FILE *s , kObj obj){
    if(!obj) fprintf(s , "<void>");
    else{
        switch(obj->type){
            case BOOLEAN :
                fprintf(s , "%s" , obj->boolean ? "#t" : "#f");
                break;
            case INTEGER :
                fprintf(s , "%lld" , obj->integer);
                break;
            case DECIMAL :
                fprintf(s , "%llf" , obj->decimal);
                break;
            case CHAR    :
                fprintf(s , "#\\%c" , obj->chr);
                break;
            case STRING  :
                fprint_esc_str(s , obj->str);
                break;
            case SYMBOL  :
                fprintf(s , "'%s" , obj->str);
                break;
            case PAIR    :
                fprint_pair(s , obj);
                break;
            case NIL     :
                fprintf(s , "nil");
                break;
            case SYNTAX  :
                fprintf(s , "<syntax:%s>" , obj->proc->name);
                break;
            case MACRO   :
                fprintf(s , "<macro>");
                break;
            case FUNCTION:
                fprintf(s , "<procedure:%s>" , obj->proc->name);
                break;
            case CLOSURE :
                fprintf(s , "<closure>");
                break;
            case EXPR    :
                fprintf(s , "<expression> : ");
                fprint_obj(s , obj->expr->args);
                falert(s , " . " , obj->expr->body);
                break;
            case ENV     :
                fprintf(s , "<environment>");
                break;
            case PORT    :
                fprintf(s , "<port>");
                break;
            default:
                fprintf(s , "<unknown:%d>" , obj->type);
                break;
        }
    }
}

void print_short_obj(Obj obj){
    if(!is_pair(obj))
        print_obj(obj);
    else
        print_short_pair(obj);
}

void print_short_pair(Obj obj){
    /* assert is_list(obj)*/
#define LIM 5
    int lim = LIM;
    printf("(");
    while(lim && is_pair(obj)){
        if(lim != LIM)
            printf(" ");
        print_short_obj(car(obj));
        lim-- , obj = cdr(obj);
    }
    if(lim == 0 && !is_nil(obj))
        printf(" ...");
    printf(")");
}

Obj alert(char *str , Obj obj){
    falert(stderr , str , obj);
    fprintf(stderr , "\n");
    return (Obj)err;
}

Obj alert_dtl(char *str , Obj obj){
    fprintf(stderr , str);
    fprint_obj_dtl(stderr , obj);
    fprintf(stderr , "\n");
    return (Obj)err;
}

Obj warning(char *str){
    puts(str);
    return (Obj)err;
}
