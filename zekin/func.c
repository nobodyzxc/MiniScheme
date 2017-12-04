#include "mem.h"
#include "func.h"
#include "util.h"

Obj cons(kObj head , kObj body){
    return new(PAIR , new_cons(head , body));
}

Obj apply_listq(Obj pr , Obj env){
    if(pr->pair->car->type != PAIR)
        return new(BOOLEAN , false);
    return new(BOOLEAN , is_list(pr->pair->car));
}

Obj apply_pairq(Obj pr , Obj env){
    return new(BOOLEAN , pr->pair->car->type == PAIR);
}

Obj apply_print(Obj pr , Obj env){
    print_obj(pr->pair->car);
    printf("\n");
    return NULL;
}

Obj apply_length(Obj pr , Obj env){
    if(pr->pair->car->type != PAIR){
        printf("cannot apply length on ");
        print_type(pr->pair->car);
        return NULL;
    }
    return new(INTEGER , length(pr->pair->car));
}

Obj apply_car(Obj pr , Obj env){
    if(pr->pair->car->type == PAIR)
        return pr->pair->car->pair->car;
    else if(pr->pair->car->type == NIL)
        printf("cannot apply car on nil");
    else
        printf("cannot apply car on ") , print_obj(pr->pair->car);
    return NULL;
}

Obj apply_cdr(Obj pr , Obj env){
    if(pr->pair->car->type == PAIR)
        return pr->pair->car->pair->cdr;
    else if(pr->pair->car->type == NIL)
        printf("cannot apply cdr on nil");
    else
        printf("cannot apply cdr on ") , print_obj(pr->pair->car);
    return NULL;
}

Obj apply_cons(Obj pr , Obj env){
    // assert arity == 2
    kObj head = pr->pair->car , body = pr->pair->cdr->pair->car;
    return new(PAIR , new_cons(head , body));
}

Obj apply_eqnum(Obj pr , Obj env){
    // assert arity > 1
    if(length(pr) < 2)
        error("apply = on list whose length < 2\n");
    Obj rtn = new(BOOLEAN , true);
    Obj head = pr->pair->car;
    while(pr->type == PAIR)
        if(!is_num(pr->pair->car))
            print_obj(pr->pair->car) , error("apply = on non-number obj");
        else
            rtn->boolean &= cmp_num(head , pr->pair->car) , pr = pr->pair->cdr;
    return rtn;
}

Obj apply_not(Obj pr , Obj env){
    //assert airth == 1
    return new(BOOLEAN , is_false(pr->pair->car));
}

#define arith(pr , rtn , op , base) \
    Obj rtn = new(INTEGER , base); \
    int pr_len = length(pr); /* here*/ \
    int chk = 6 op 2; \
    if((6 op 2 > 5 && pr_len) || pr_len > 1){ \
        if(pr->pair->car->type == INTEGER){ \
            rtn->integer = pr->pair->car->integer; \
        } \
        else if(pr->pair->car->type == DECIMAL){ \
            rtn->type = DECIMAL; \
            rtn->decimal = pr->pair->car->decimal; \
        } \
        else{ \
            printf("cannot apply " xstr(op) " on non-number obj"); \
            print_obj(pr->pair->car); error("");\
        } \
        pr = pr->pair->cdr; \
    } \
    for( ; pr ; pr = pr->pair->cdr){ \
        if(pr->type == NIL) \
            break; \
        if(6 op 3 == 2){ \
            if(num_of(pr->pair->car) == 0) \
            error("cannot div zero"); \
        } \
        if(!is_num(pr->pair->car)) \
        error("cannot apply " xstr(op) " on non-number obj"); \
        else if(rtn->type == DECIMAL){ \
            if(pr->pair->car->type == INTEGER) \
            rtn->decimal op ## = (double) pr->pair->car->integer; \
            else if(pr->pair->car->type == DECIMAL) \
            rtn->decimal op ## = pr->pair->car->decimal; \
        } \
        else if(rtn->type == INTEGER){ \
            if(pr->pair->car->type == INTEGER) \
            rtn->integer op ## = pr->pair->car->integer; \
            else if(pr->pair->car->type == DECIMAL){ \
                rtn->type = DECIMAL; \
                rtn->decimal = rtn->integer; \
                rtn->decimal op ## = pr->pair->car->decimal; \
            } \
        } \
    }

#define apply_opr(op_name , op , base) \
    Obj apply_ ## op_name(Obj pr , Obj env){ \
        arith(pr , rtn , op , base); \
        return rtn; \
    }

apply_opr(add , + , 0);
apply_opr(mul , * , 1);
apply_opr(sub , - , 0);
apply_opr(div , / , 1);
