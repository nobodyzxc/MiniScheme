#include "mem.h"
#include <stdlib.h>
#define LEAKFILE ".leak"

#define new_pair(a , b) new_pair(a , b , __FILE__ ":" xstr(__LINE__))
#define free_pair(a)    free_pair(a , __FILE__ ":" xstr(__LINE__))
#define free_obj(a) free_obj(a , __FILE__ ":" xstr(__LINE__))

FILE *memchk = NULL;

void *MALLOC(size_t size , const char *loc){
    if(!memchk) memchk = fopen(LEAKFILE , "w");
    void *p = malloc(size);
    fprintf(memchk , "alloc %p %s->%s" ,
            p , size == sizeof(obj_t) ? "obj" : "par" , loc);
    return p;
}

void *FREE(void *p , const char *t){
    if(!memchk) memchk = fopen(LEAKFILE , "w");
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
    if(obj->type == NIL)
        return obj;
    Obj rtn = new_obj(obj->type , "mm:27");
    (*rtn) = (*obj);
    if(rtn->type == STRING)
        rtn->string = strdup(obj->string);
    else if(rtn->type == SYMBOL
            || rtn->type == KEYWORD)
        rtn->symbol = strdup(obj->symbol);
    else if(rtn->type == PAIR)
        rtn->pair = copy_pair(obj->pair);
    return rtn;
}

Pair copy_pair(Pair pr){
    Pair rtn = new_pair(copy_obj(pr->car) , NULL);
    Pair it = rtn;
    pr = pr->cdr;
    while(pr){
        it->cdr = new_pair(copy_obj(pr->car) , NULL);
        it = it->cdr , pr = pr->cdr;
    }
    return rtn;
}

#undef free_obj
void free_obj(Obj obj , const char *loc){
    if(obj->type == NIL)
        return;
    if(obj->type == PAIR)
        free_pair(obj->pair);
    char s[100];
    sprintf(s , "%s:(pair->%s)" , loc , type_name[obj->type]);
    FREE(obj , loc);
}
#define free_obj(a) free_obj(a , __FILE__ ":" xstr(__LINE__))

#undef new_pair
Pair new_pair(kObj car , Pair cdr , const char *loc){
    Pair pr = (Pair)MALLOC(sizeof(pair_t) , loc);
    fprintf(memchk , "(non)\n");
    pr->car = (Obj)car , pr->cdr = cdr;
    return pr;
}
#define new_pair(a , b) new_pair(a , b , __FILE__ ":" xstr(__LINE__))

#undef free_pair
void free_pair(Pair pr , const char *loc){
    char s[100];
    while(pr && pr->cdr){
        free_obj(pr->car);
        Pair fp = pr;
        pr = pr->cdr;
        sprintf(s , "%s:(pair->%s)" , loc , type_name[pr->car->type]);
        FREE(fp , s);
    }
    if(pr->car->type == NIL){
        sprintf(s , "%s:(pair->%s)" , loc , type_name[pr->car->type]);
        FREE(pr , s);
    }
}
#define free_pair(a)    free_pair(a , __FILE__ ":" xstr(__LINE__))

void free_pair_shallow(Pair pr){
    while(pr && pr->cdr){
        Pair fp = pr;
        pr = pr->cdr;
        FREE(fp , "shallow pr");
    }
    if(pr->car->type == NIL){
        FREE(pr , "shallow pr");
    }
}

Token new_token(char *p , Token next){
    if(!memchk) memchk = fopen(LEAKFILE , "w");
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
