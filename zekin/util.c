#include "util.h"
#include <stdlib.h>

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

int length(Obj pr){
    int rtn = 0;
    if(!is_list(pr))
        error("apply length on pair\n");
    while(!is_nil(pr))
        rtn++ , pr = pr->pair->cdr;
    return rtn;
}

void print_type(Obj obj){
    if(obj->type == PAIR){
        Obj pr = obj;
        printf("(");
        print_type(pr->pair->car);
        pr = pr->pair->cdr;
        while(pr && pr->type == PAIR)
            printf(" ") , print_type(pr->pair->car) , pr = pr->pair->cdr;

        if(pr && !is_nil(pr))
            printf(" . ");
        if(pr)
            printf(" ") , print_type(pr);
        printf(")");

    }
    else{
        printf("%s" , type_name[obj->type]);
    }
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
            case FUNCTION:
                printf("<procedure:%s>" , obj->proc->name);
                break;
            case CLOSURE :
                printf("<closure ");
                print_obj(obj->clos->exp->expr->args);
                printf(" . ");
                print_obj(obj->clos->exp->expr->body);
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
