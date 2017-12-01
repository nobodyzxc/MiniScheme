#include "util.h"
#include <stdlib.h>

bool is_list(Cons pr){
    while(pr->cdr)
        pr = pr->cdr;
    return pr->car->type == NIL;
}

bool cmp_num(Obj a , Obj b){
    if(a->type == b->type)
        return num_of(a) == num_of(b);
    else
        return (double)num_of(a) == (double)num_of(b);
}

int length(Cons pr){
    int rtn = 0;
    if(!is_list(pr))
        error("apply length on pair\n");
    while(pr->cdr)
        rtn++ , pr = pr->cdr;
    return rtn;
}

void print_type(Obj obj){
    if(obj->type == PAIR){
        Obj pr = obj;
        printf("(");
        print_type(pr->pair->car);
        Cons it = pr->pair->cdr;
        while(it && it->cdr)
            printf(" ") , print_type(it->car) , it = it->cdr;

        if(it && it->car->type != NIL)
            printf(" . ");
        if(it)
            printf(" ") , print_type(it->car);
        printf(")");

    }
    else{
        printf("%s" , type_name[obj->type]);
    }
}

void print_pair(kObj pr){
    print_cons(pr->pair);
}

void print_cons(Cons kons){
    printf("(");
    if(kons->car->type != NIL)
        print_obj(kons->car);
    kons = kons->cdr;
    while(kons && kons->cdr)
        printf(" ") , print_obj(kons->car) , kons = kons->cdr;
    if(kons && kons->car->type != NIL)
        printf(" . ") , print_obj(kons->car);
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
            case FUNCTION:
                printf("<procedure:%s>" , obj->proc->name);
                break;
            case CLOSURE :
                printf("<closure ");
                print_obj(obj->clos->expr->expr->args);
                printf(" . ");
                print_obj(obj->clos->expr->expr->body);
                printf(">");
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
