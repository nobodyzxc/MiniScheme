#include "mem.h"
#include <stdlib.h>

FILE *memchk = NULL;

void *MALLOC(size_t size , const char *loc){
    if(!memchk) memchk = fopen("leak.txt" , "w");
    void *p = malloc(size);
    fprintf(memchk , "alloc %p %s->%s" ,
            p , size == sizeof(obj_t) ? "obj" : "par" , loc);
    return p;
}

void *FREE(void *p , char *t){
    if(!memchk) memchk = fopen("leak.txt" , "w");
    fprintf(memchk , "free %p %s\n" , p , t);
    free(p);
}

Obj  new_obj(type_t type , const char *loc){
    Obj obj = (Obj)MALLOC(sizeof(obj_t) , loc);
    fprintf(memchk , "(%s)\n" , type_name[type]);
    obj->type = type;
    return obj;
}

Obj  copy_obj(Obj obj){
    Obj rtn = new_obj(obj->type , "mm:27");
    rtn = obj;
    if(rtn->type == STRING)
        rtn->string = strdup(obj->string);
    if(rtn->type == SYMBOL
            || rtn->type == PROCEDURE
            || rtn->type == KEYWORD)
        rtn->symbol = strdup(obj->symbol);
    return rtn;
}

void free_obj(Obj obj){
    if(obj->type == NIL)
        return;
    if(obj->type == PAIR)
        free_pair(obj->pair);
    FREE(obj , type_name[obj->type]);
}

Pair new_pair(kObj car , Pair cdr , const char *loc){
    Pair pr = (Pair)MALLOC(sizeof(pair_t) , loc);
    fprintf(memchk , "(non)\n");
    pr->car = (Obj)car , pr->cdr = cdr;
    return pr;
}

void free_pair(Pair pr){
    while(pr && pr->cdr){
        free_obj(pr->car);
        Pair fp = pr;
        pr = pr->cdr;
        char s[100];
        sprintf(s , "pair(%s)" , type_name[pr->car->type]);
        FREE(fp , s);
    }
    if(pr->car->type == NIL)
        FREE(pr , "s");
}

Token new_token(char *p , Token next){
    if(!memchk) memchk = fopen("leak.txt" , "w");
    fprintf(memchk , "alloc %p %s\n" , p , "strdup");

    Token new_tok = (Token)MALLOC(sizeof(token_t) , "tok:0x0\n");
    new_tok->p = p;
    new_tok->next = next;
    return new_tok;
}

void free_token(Token tok){
    Token pre = tok;
    while(tok){
        pre = tok , tok = tok->next;
        FREE(pre->p , "toklit");
        FREE(pre , "tok");
    }
}
