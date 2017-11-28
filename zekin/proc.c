#include "proc.h"
#include "mem.h"
#include <stdlib.h>
#define new_pair(a , b) new_pair(a , b , __FILE__ ":" xstr(__LINE__))
#define new_obj(a) new_obj(a , __FILE__ ":" xstr(__LINE__))
#ifdef __cplusplus //or __GNUG__
#define make_proc_obj(op_name , op) \
obj_t proc_ ## op_name = { \
    .type = PROCEDURE , \
    { .proc = { (char*)xstr(op) , apply_ ## op_name } } , \
}
#else
#define make_proc_obj(op_name , op) \
obj_t proc_ ## op_name = { \
    .type = PROCEDURE  , \
    .proc.name = xstr(op) , \
    .proc.func = apply_ ## op_name   , \
}
#endif
bool is_list(Pair pr){
    while(pr->cdr)
        pr = pr->cdr;
    return pr->car->type == NIL;
}

int length(Pair pr){
    int rtn = 0;
    if(!is_list(pr))
        error("apply length on pair\n");
    while(pr->cdr)
        rtn++ , pr = pr->cdr;
    return rtn;
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

void print_type(Obj obj){
    if(obj->type == PAIR){
        Obj pr = obj;
        printf("(");
        print_type(pr->pair->car);
        Pair it = pr->pair->cdr;
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

void print_obj(kObj obj){
    if(obj) print_method[obj->type]((Obj)obj);
}

void print_bool(Obj obj){
    printf("%s" , obj->boolean ? "#t" : "#f");
}

void print_int(Obj obj){
    printf("%lld" , obj->integer);
}

void print_float(Obj obj){
    printf("%llf" , obj->decimal);
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
    printf("<procedure:%s>" , obj->proc.name);
}

void print_keyw(Obj obj){
    printf("<keyword:%s>" , obj->symbol);
}

void print_nil(Obj obj){
    printf("nil");
}

void print_pair(Obj pr){
    printf("(");
    if(pr->pair->car->type != NIL)
        print_obj(pr->pair->car);
    Pair it = pr->pair->cdr;
    while(it && it->cdr)
        printf(" ") , print_obj(it->car) , it = it->cdr;

    if(it && it->car->type != NIL)
        printf(" . ") , print_obj(it->car);
    printf(")");
}

Obj apply_length(Pair pr){
    if(pr->car->type != PAIR){
        printf("cannot apply length on ");
        print_type(pr->car);
        return NULL;
    }
    Obj rtn = new_obj(INTEGER);
    rtn->integer = length(pr->car->pair);
    return rtn;
}
make_proc_obj(length , length);

Obj apply_car(Pair pr){
    if(pr->car->type == PAIR)
        return pr->car->pair->car;
    else if(pr->car->type == NIL)
        printf("cannot apply car on nil");
    else
        printf("cannot apply car on ") , print_obj(pr->car);
    return NULL;
}
make_proc_obj(car , car);

Obj apply_cdr(Pair pr){
    if(pr->car->type == PAIR){
        Obj rtn = new_obj(PAIR);
        rtn->pair = pr->car->pair->cdr;
        return rtn;
    }
    else if(pr->car->type == NIL)
        printf("cannot apply cdr on nil");
    else
        printf("cannot apply cdr on ") , print_obj(pr->car);
    return NULL;

}
make_proc_obj(cdr , cdr);

Obj apply_cons(Pair pr){
    if(!pr || !pr->cdr)
        error("apply cons on incomplete list\n");
    kObj head = pr->car , body = pr->cdr->car;
    Pair body_pr;
    if(body->type != PAIR)
        body_pr = new_pair(body , NULL);
    else
        body_pr = body->pair;
    Obj obj = new_obj(PAIR);
    obj->pair = new_pair(head , body_pr);
    return obj;
}
make_proc_obj(cons , cons);

bool cmp_num(Obj a , Obj b){
    if(a->type == b->type)
        return num_of(a) == num_of(b);
    else
        return (double)num_of(a) == (double)num_of(b);
}

Obj apply_eqnum(Pair pr){
    if(length(pr) < 2)
        error("apply = on list whose length < 2\n");
    Obj rtn = new_obj(BOOLEAN);
    rtn->boolean = true;
    Obj head = pr->car;
    while(pr->cdr && pr->car->type != NIL)
        if(!is_num(pr->car))
            error("apply = on non-number obj");
        else
            rtn->boolean &= cmp_num(head , pr->car) , pr = pr->cdr;
    return rtn;
}
make_proc_obj(eqnum , =);

#define arith(pr , rtn , op , base) \
    Obj rtn = new_obj(INTEGER); \
    int pr_len = length(pr); \
    int chk = 6 op 2; \
    if((6 op 2 > 5 && pr_len) || pr_len > 1){ \
        if(pr->car->type == INTEGER){ \
            rtn->integer = pr->car->integer; \
        } \
        else if(pr->car->type == DECIMAL){ \
            rtn->type = DECIMAL; \
            rtn->decimal = pr->car->decimal; \
        } \
        else \
            error("cannot apply " xstr(op) " on non-number obj"); \
        pr = pr->cdr; \
    } \
    else rtn->integer = base; \
    for(Pair it = pr ; it ; it = it->cdr){ \
        if(it->car->type == NIL) \
            break; \
        if(6 op 3 == 2){ \
            if(num_of(it->car) == 0) \
                error("cannot div zero"); \
        } \
        if(!is_num(it->car)) \
            error("cannot apply " xstr(op) " on non-number obj"); \
        else if(rtn->type == DECIMAL){ \
            if(it->car->type == INTEGER) \
                rtn->decimal op ## = (double) it->car->integer; \
            else if(it->car->type == DECIMAL) \
                rtn->decimal op ## = it->car->decimal; \
        } \
        else if(rtn->type == INTEGER){ \
            if(it->car->type == INTEGER) \
                rtn->integer op ## = it->car->integer; \
            else if(it->car->type == DECIMAL){ \
                rtn->type = DECIMAL; \
                rtn->decimal = rtn->integer; \
                rtn->decimal op ## = it->car->decimal; \
            } \
        } \
    }

#define make_arith(op_name , op , base) \
Obj apply_ ## op_name(Pair pr){ \
    arith(pr , rtn , op , base); \
    return rtn; \
} \
make_proc_obj(op_name , op);

make_arith(add , + , 0);
make_arith(mul , * , 1);
make_arith(sub , - , 0);
make_arith(div , / , 1);

char *procs[] = {
    (char*)"car" ,
    (char*)"cdr" ,
    (char*)"cons" ,
    (char*)"+"    ,
    (char*)"*"    ,
    (char*)"-"    ,
    (char*)"/"    ,
    (char*)"="    ,
    (char*)"length" ,
};

Obj buildins[] = {
    &proc_car  ,
    &proc_cdr  ,
    &proc_cons ,
    &proc_add  ,
    &proc_mul  ,
    &proc_sub  ,
    &proc_div  ,
    &proc_eqnum ,
    &proc_length ,
};

bool is_proc(char *s){
    int r = sizeof(procs) / sizeof(procs[0]);
    while(r--)
        if(EQS(procs[r] , s))
            return true;
    return false;
}

Obj lookup_buildins(char *p){
    int r = sizeof(procs) / sizeof(procs[0]);
    while(r--)
        if(EQS(procs[r] , p))
            return buildins[r];
    //error("<procedure:%s> not found\n" , p);
    return NULL;
}

char *keyws[] = {
    (char*)"if" ,
    (char*)"let" ,
    (char*)"letrec" ,
    (char*)"cond" ,
    (char*)"define" ,
    (char*)"lambda" ,
    (char*)"quote" ,
};

bool is_keyw(char *s){
    int r = sizeof(keyws) / sizeof(keyws[0]);
    while(r--)
        if(EQS(keyws[r] , s))
            return true;
    return false;
}
