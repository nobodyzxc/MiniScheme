#include "mem.h"
#include "type.h"
#include "util.h"
#include <stdlib.h>
#define max(a , b) (a > b ? a : b)

Obj lssym(Obj ls , Obj sym){
    if(sym->type != SYMBOL) return NULL;
    for(Obj it = ls ; it &&
            it != nil ; it = it->pair->cdr)
        if(it->pair->car->type == SYMBOL)
            if(EQS(it->pair->car->str , sym->str))
                return it;
    return NULL;
}

Obj lsobj(Obj ls , Obj obj){
    for(Obj it = ls ; it &&
            it != nil ; it = it->pair->cdr)
        if(it->pair->car == obj) return it;
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

Obj zipped_pat(Obj pat , Obj args , Obj env){
    bool isls = is_list(pat);
    int argslen = length(args);
    env = new(ENV , env);
    while(pat->type == PAIR){
        if(EQS(pat->pair->car->str , "...")){
            add_symbol(pat->pair->car , args , env);
            return env;
        }
        add_symbol(
                pat->pair->car ,
                args->pair->car ,
                env);
        pat = pat->pair->cdr;
        args = args->pair->cdr;
    }
    return env;
}


Obj zipped_env(Obj syms , Obj args , Obj env){
    //assert args is list
    //len(sym) > len(args)
    bool isls = is_list(syms);
    int argslen = length(args);
    if(isls && length(syms) != argslen ||
            !isls && patnum(syms) > max(argslen , 1)){
        printf("unmatch args: ") , print_obj(syms);
        printf(" <- ") , print_obj(args) , error("");
    }
    env = new(ENV , env);
    while(syms->type == PAIR){
        add_symbol(
                syms->pair->car ,
                args->pair->car ,
                env);
        syms = syms->pair->cdr;
        args = args->pair->cdr;
    }
    if(!is_nil(syms))
        add_symbol(syms , args , env);
    return env;
}

bool is_list(Obj pr){
    while(pr && pr->type == PAIR)
        pr = pr->pair->cdr;
    return pr && pr->type == NIL;
}

bool cmp_num(Obj a , Obj b){
    if(a->type == b->type)
        return num_of(a) == num_of(b);
    else
        return (double)num_of(a) == (double)num_of(b);
}

int patnum(Obj pr){
    int rtn = 0;
    while(pr && !is_nil(pr))
        rtn ++ , pr = pr->pair->cdr;
    return rtn;
}

int length(Obj pr){
    int rtn = 0;
    if(!is_list(pr))
        error("apply length on pair\n");
    while(!is_nil(pr))
        rtn++ , pr = pr->pair->cdr;
    return rtn;
}

Obj cons(kObj head , kObj body){
    return new(PAIR , new_cons(head , body));
}

void print_pair(kObj pr){
    printf("(");
    print_obj(pr->pair->car);
    pr = pr->pair->cdr;
    while(pr && pr->type == PAIR)
        printf(" ") , print_obj(pr->pair->car) , pr = pr->pair->cdr;
    if(pr && !is_nil(pr))
        printf(" . ") , print_obj(pr);
    printf(")");
}

void print_obj(kObj obj){
    if(!obj) printf("<void>");
    else{
        switch(obj->type){
            case BOOLEAN :
                printf("%s" , obj->boolean ? "#t" : "#f");
                break;
            case INTEGER :
                printf("%lld" , obj->integer);
                break;
            case DECIMAL :
                printf("%llf" , obj->decimal);
                break;
            case CHAR    :
                printf("#\\%c" , obj->chr);
                break;
            case STRING  :
                printf("\"%s\"" , obj->str);
                break;
            case SYMBOL  :
                printf("'%s" , obj->str);
                break;
            case PAIR    :
                print_pair(obj);
                break;
            case NIL     :
                printf("nil");
                break;
            case SYNTAX  :
                printf("<syntax:%s>" , obj->proc->name);
                break;
            case MACRO   :
                printf("<macro>");
                break;
            case FUNCTION:
                printf("<procedure:%s>" , obj->proc->name);
                break;
            case CLOSURE :
                printf("<closure>");
                break;
            case EXPR    :
                printf("<expression>");
                break;
            case ENV     :
                printf("<environment>");
                break;
        }
    }
}
