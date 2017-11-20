#include "type.h"
#include <stdlib.h>

const pair_t nil_pr = {
    .car = NULL ,
    .cdr = NULL ,
};

const obj_t nil_t = {
    .type = NIL ,
    .pair = (Pair)&nil_pr ,
};

kObj nil = &nil_t;

void (*print_method[])(Obj) = {
    print_bool  ,
    print_int   ,
    print_float ,
    print_char  ,
    print_str   ,
    print_sym   ,
    print_proc  ,
    print_keyw  ,
    print_nil   ,
    print_pair  ,
};

char type_name[10][10] = {
    "boolean"   ,
    "integer"   ,
    "floating"  ,
    "character" ,
    "string"    ,
    "symbol"    ,
    "procedure" ,
    "keyword"   ,
    "nil"       ,
    "pair"      ,
};

void print_type(Obj obj){
    printf("%s " , type_name[obj->type]);
}

void print_obj(kObj obj){
    print_method[obj->type]((Obj)obj);
}

void print_bool(Obj obj){
    printf("%s" , obj->boolean ? "#t" : "#f");
}

void print_int(Obj obj){
    printf("%d" , obj->integer);
}

void print_float(Obj obj){
    printf("%ld" , obj->floating);
}

void print_char(Obj obj){
    printf("#\\%c" , obj->character);
}

void print_str(Obj obj){
    printf("\"%s\"" , obj->string);
}

void print_sym(Obj obj){
    printf("'%s" , obj->symbol);
}

void print_proc(Obj obj){
    printf("<procedure:%s>" , obj->symbol);
}

void print_keyw(Obj obj){
    printf("<keyword:%s>" , obj->symbol);
}

void print_nil(Obj obj){
    printf("()");
}

void print_pair(Obj pr){
    Pair it = pr->pair->cdr;
    printf("(") , print_obj(pr->pair->car);
    while(it->cdr)
        printf(" ") , print_obj(it->car) , it = it->cdr;

    if(it->car->type != NIL)
        printf(" . ") , print_obj(it->car);
    printf(")");
}

Obj  new_obj(type_t type){
    Obj obj = (Obj)malloc(sizeof(obj_t));
    obj->type = type;
    return obj;
}

Pair new_pair(kObj car , Pair cdr){
    Pair pr = (Pair)malloc(sizeof(pair_t));
    pr->car = (Obj)car , pr->cdr = cdr;
    return pr;
}

Obj cons(kObj head , kObj body){
    Pair body_pr;
    if(body->type != PAIR)
        body_pr = new_pair(body , NULL);
    else
        body_pr = body->pair;

    Obj obj = new_obj(PAIR);
    obj->pair = new_pair(head , body_pr);
    return obj;
}
