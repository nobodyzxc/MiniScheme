#include "mem.h"
#include "func.h"

bool is_list(Cons pr){
    while(pr->cdr)
        pr = pr->cdr;
    return pr->car->type == NIL;
}

int length(Cons pr){
    int rtn = 0;
    if(!is_list(pr))
        error("apply length on pair\n");
    while(pr->cdr)
        rtn++ , pr = pr->cdr;
    return rtn;
}

Obj cons(kObj head , kObj body){
    Cons body_pr;
    if(body->type != PAIR)
        body_pr = new_cons(body , NULL);
    else
        body_pr = body->pair;

    return new(PAIR , new_cons(head , body_pr));
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

Obj apply_listq(Cons pr , Obj env){
    if(pr->car->type != PAIR)
        return new(BOOLEAN , false);
    return new(BOOLEAN , is_list(pr->car->pair));
}

Obj apply_pairq(Cons pr , Obj env){
    return new(BOOLEAN , pr->car->type == PAIR);
}

Obj apply_print(Cons pr , Obj env){
    print_obj(pr->car);
    printf("\n");
    return NULL;
}

Obj apply_length(Cons pr , Obj env){
    if(pr->car->type != PAIR){
        printf("cannot apply length on ");
        print_type(pr->car);
        return NULL;
    }
    return new(INTEGER , length(pr->car->pair));
}

Obj apply_car(Cons pr , Obj env){
    if(pr->car->type == PAIR)
        return pr->car->pair->car;
    else if(pr->car->type == NIL)
        printf("cannot apply car on nil");
    else
        printf("cannot apply car on ") , print_obj(pr->car);
    return NULL;
}

Obj apply_cdr(Cons pr , Obj env){
    if(pr->car->type == PAIR)
        return new(PAIR , pr->car->pair->cdr);
    else if(pr->car->type == NIL)
        printf("cannot apply cdr on nil");
    else
        printf("cannot apply cdr on ") , print_obj(pr->car);
    return NULL;

}

Obj apply_cons(Cons pr , Obj env){
    if(!pr || !pr->cdr)
        error("apply cons on incomplete list\n");
    kObj head = pr->car , body = pr->cdr->car;
    Cons body_pr;
    if(body->type != PAIR)
        body_pr = new_cons(body , NULL);
    else
        body_pr = body->pair;
    return new(PAIR , new_cons(head , body_pr));
}

bool cmp_num(Obj a , Obj b){
    if(a->type == b->type)
        return num_of(a) == num_of(b);
    else
        return (double)num_of(a) == (double)num_of(b);
}

Obj apply_eqnum(Cons pr , Obj env){
    if(length(pr) < 2)
        error("apply = on list whose length < 2\n");
    Obj rtn = new(BOOLEAN , true);
    Obj head = pr->car;
    while(pr->cdr && pr->car->type != NIL)
        if(!is_num(pr->car))
            error("apply = on non-number obj");
        else
            rtn->boolean &= cmp_num(head , pr->car) , pr = pr->cdr;
    return rtn;
}

#define arith(pr , rtn , op , base) \
    Obj rtn = new(INTEGER , base); \
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
        else{ \
            printf("cannot apply " xstr(op) " on non-number obj"); \
            print_obj(pr->car); error("");\
        } \
        pr = pr->cdr; \
    } \
    for(Cons it = pr ; it ; it = it->cdr){ \
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
    Obj apply_ ## op_name(Cons pr , Obj env){ \
        arith(pr , rtn , op , base); \
        return rtn; \
    }

make_arith(add , + , 0);
make_arith(mul , * , 1);
make_arith(sub , - , 0);
make_arith(div , / , 1);
