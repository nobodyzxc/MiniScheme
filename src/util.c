#include "mem.h"
#include "type.h"
#include "util.h"
#include <stdlib.h>
#define max(a , b) (a > b ? a : b)

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

void detail(Obj obj){
    if(obj->type == CLOSURE){
        printf("<closure ");
        print_obj(obj->clos->exp->expr->args);
        printf(" . ");
        print_obj(obj->clos->exp->expr->body);
        printf(">");
    }
    if(obj->type == MACRO){
        printf("<macro ");
        print_obj(obj->mac->keyws);
        printf(" . ");
        print_obj(obj->mac->rules);
        printf(">");
    }
}

void print_esc(char *str){
    char *k = "abfnrtv\\'?" , *v = "\a\b\f\n\r\t\v\\\'\?";
    while(*str){
        if(*str == '\\' && strchr("abfnrtv\\'?" , *(str + 1)))
            putchar(v[strchr(k , *(str + 1)) - k]) , str++;
        else
            putchar(*str);
        *str++;
    }
}

void print_symtree(Symtree tree){
    if(tree == NULL) return;
    print_symtree(tree->rt);
    print_obj(tree->sym);
    printf(" : ");
    print_obj(tree->val);
    puts("");
    print_symtree(tree->lt);
}

Obj map_car(Obj ls){
    cons_t head;
    Cons last = &head;
    for( ; ls && !IS_NIL(ls) ; ls = cdr(ls)){
        last->cdr = new(PAIR , new_cons(caar(ls) , NULL));
        last = last->cdr->pair;
    }
    last->cdr = (Obj)nil;
    return head.cdr;
}

void set_map_cdr(Obj ls){
    for( ; ls && !IS_NIL(ls) ; ls = cdr(ls))
        car(ls) = cdr(car(ls));
}

Obj zip_pat(Obj pat , Obj args , Obj env);

Obj zip_elipat(Obj pat , Obj args , Obj env){
    if(pat->type == SYMBOL)
        add_symbol(pat , args , env);
    else if(pat->type == PAIR){
        for( ; pat && !IS_NIL(pat) ; pat = cdr(pat)){
            zip_elipat(car(pat) , map_car(args) , env);
            set_map_cdr(args);
        }
    }
    else{
        error("wrong eli zip type");
    }
    return env;
}

Obj zip_pat(Obj pat , Obj args , Obj env){
    bool isls = is_list(pat);
    int argslen = length(args);
    while(pat->type == PAIR){
        Obj mch = car(pat);
        Obj nxt = cdr(pat);
        nxt = nxt ? (nxt->type == PAIR ? car(nxt) : nxt) : NULL;
        if(nxt == eli)
            return zip_elipat(mch , args , env);

        if(mch->type == PAIR){ /* rec zip */
            zip_pat(mch , car(args) , env);
        }
        else if(mch->type == SYMBOL){
            if(IS_NIL(args)) error("is nil? in util.c");
            add_symbol(mch , car(args) , env);
        }
        pat = cdr(pat);
        args = cdr(args);
    }
    return env;
}

Obj zip_env(Obj syms , Obj args , Obj env){
    //assert args is list
    //len(sym) > len(args)
    bool isls = is_list(syms);
    int argslen = length(args);
    if(isls && length(syms) != argslen ||
            !isls && pat_num(syms) > max(argslen , 1)){
        printf("unmatch args: ") , print_obj(syms);
        printf(" <- ") , print_obj(args) , error("");
    }
    while(syms->type == PAIR){
        add_symbol(car(syms), car(args), env);
        syms = cdr(syms) , args = cdr(args);
    }
    if(!IS_NIL(syms))
        add_symbol(syms , args , env);
    return env;
}

bool is_list(Obj pr){
    while(pr && pr->type == PAIR)
        pr = cdr(pr);
    return pr && pr->type == NIL;
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
        for( ; a && !IS_NIL(a) ; a = cdr(a) , b = cdr(b))
            if(!equal(car(a) , car(b))) return false;
        return true;
    }
    return false;
}

int pat_num(Obj pr){
    int rtn = 0;
    while(pr && !IS_NIL(pr))
        rtn ++ , pr = cdr(pr);
    return rtn;
}

int length(Obj pr){
    int rtn = 0;
    if(!is_list(pr))
        error("apply length on pair\n");
    while(!IS_NIL(pr))
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
    while(pr && pr->type == PAIR)
        fprintf(s , " ") , fprint_obj(s , car(pr)) , pr = cdr(pr);
    if(pr && !IS_NIL(pr))
        fprintf(s , " . ") , fprint_obj(s , pr);
    fprintf(s , ")");
}

void print_pair(kObj pr){
    return fprint_pair(stdout , pr);
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
                fprintf(s , "\"%s\"" , obj->str);
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
                //print_obj(obj->clos->exp);
                break;
            case EXPR    :
                fprintf(s , "<expression> : ");
                fprint_obj(s , obj->expr->args);
                falert(s , " . " , obj->expr->body);
                break;
            case ENV     :
                fprintf(s , "<environment>");
                break;
        }
    }
}

void print_obj(kObj obj){
    return fprint_obj(stdout , obj);
}
