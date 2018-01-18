#include "mem.h"
#include "func.h"
#include "util.h"
#include "eval.h"
#include "main.h"
#include "parse.h"

Obj apply_exit(Obj args , Obj env){
    exit(0);
    return NULL;
}

Obj apply_clos(Obj pcr , Obj args , Obj env){

    Clos pcr_clos = pcr->clos; // if ref cur env ?
    Expr pcr_expr = pcr_clos->exp->expr;
    env = zipped_env(pcr_expr->args , args , pcr_clos->env);
    Obj iter = pcr->clos->exp->expr->body , val;
    while(!IS_NIL(iter))
        val = eval(car(iter) , env) , iter = cdr(iter);
    return val;
}

Obj apply_gc(Obj pr , Obj env){
    gc();
    return NULL;
}

Obj apply_source(Obj pr , Obj env){
    //assert car(pr) is STRING
    if(length(pr) != 1)
        puts("give me one filename");
    else if(car(pr)->type != STRING)
        puts("source file name must be string");
    else{
        FILE *prev_stream = stream;
        char *filename = car(pr)->str;
        stream = fopen(filename , "r");
        if(!stream)
            printf("cannot open file %s\n" , filename);
        else
            repl(false , false) , fclose(stream);
        stream = prev_stream;
    }
    return NULL;
}

Obj apply_senv(Obj pr , Obj env){
    //assert len(pr) == 1
    pr = car(pr);
    if(pr->type == ENV)
        print_symtree(pr->env->symtab);
    return NULL;
}

Obj apply_apply(Obj pr , Obj env){
    // assert arity == 2
    // avoid apply special form
    if(car(pr)->type == CLOSURE)
        return apply_clos(car(pr) ,
                cadr(pr) , env);
    else if(car(pr)->type == FUNCTION)
        return car(pr)->proc->apply(
                cadr(pr) , env);
    alert("cannot apply " , car(pr));
    return NULL;
}

Obj apply_nullq(Obj pr , Obj env){
    // assert arith == 1
    return new(BOOLEAN , IS_NIL(car(pr)));
}

Obj apply_listq(Obj pr , Obj env){
    return new(BOOLEAN , is_list(car(pr)));
}

Obj apply_pairq(Obj pr , Obj env){
    return new(BOOLEAN , car(pr)->type == PAIR);
}

Obj apply_flush_output(Obj pr , Obj env){
    fflush(stdout);
    return NULL;
}

Obj apply_display(Obj pr , Obj env){
    if(!car(pr))
        print_obj(pr);
    else if(car(pr)->type == STRING)
        print_esc(car(pr)->str);
    else if(car(pr)->type == CLOSURE)
        detail(car(pr));
    else
        print_obj(car(pr));
    return NULL;
}

Obj apply_length(Obj pr , Obj env){
    if(car(pr)->type != PAIR){
        alert("cannot apply length on " , car(pr));
        return NULL;
    }
    return new(INTEGER , length(car(pr)));
}

Obj apply_car(Obj pr , Obj env){
    if(car(pr)->type == PAIR)
        return caar(pr);
    else
        alert("cannot apply car on " , car(pr));
    return NULL;
}

Obj apply_cdr(Obj pr , Obj env){
    if(car(pr)->type == PAIR)
        return cdar(pr);
    else
        alert("cannot apply cdr on " , car(pr));
    return NULL;
}

Obj apply_cons(Obj pr , Obj env){
    // assert arity == 2
    kObj head = car(pr) , body = cadr(pr);
    return new(PAIR , new_cons(head , body));
}

Obj apply_eqnum(Obj pr , Obj env){
    // assert arity > 1
    if(length(pr) < 2)
        error("apply = on list whose length < 2\n");
    bool rtn = true;
    Obj head = car(pr);
    while(pr->type == PAIR)
        if(!is_num(car(pr)))
            print_obj(car(pr)) , error("apply = on non-number obj");
        else
            rtn &= cmp_num(head , car(pr)) , pr = cdr(pr);
    return new(BOOLEAN , rtn);
}

Obj apply_eqq(Obj pr , Obj env){
    //assert arity == 2
    return new(BOOLEAN , car(pr) == cadr(pr));
}

Obj apply_eqvq(Obj pr , Obj env){
    //assert arity == 2
    return new(BOOLEAN , eqv(car(pr) , cadr(pr)));
}

Obj apply_equalq(Obj pr , Obj env){
    return new(BOOLEAN , equal(car(pr) , cadr(pr)));
}

#define apply_cmp(name , op) \
    Obj apply_ ## name (Obj pr , Obj env){ \
        for( ; pr && !IS_NIL(pr) && !IS_NIL(cdr(pr)) ; \
                pr = cdr(pr)) \
        if(!(num_of(car(pr)) op num_of(cadr(pr)))) \
        return (Obj)false_obj; \
        return (Obj)true_obj; \
    }

apply_cmp(lt , <);
apply_cmp(gt , >);
apply_cmp(let , <=);
apply_cmp(get , >=);

Obj apply_not(Obj pr , Obj env){
    //assert airth == 1
    return new(BOOLEAN , IS_FALSE(car(pr)));
}

Obj apply_void(Obj pr , Obj env){
    return NULL;
}

Obj apply_voidq(Obj pr , Obj env){
    return new(BOOLEAN , car(pr) == NULL);
}

Obj apply_symbolq(Obj pr , Obj env){
    return new(BOOLEAN , car(pr)->type == SYMBOL);
}

Obj apply_procedureq(Obj pr , Obj env){
    return new(BOOLEAN , car(pr)
            && (car(pr)->type == CLOSURE
                || car(pr)->type == FUNCTION));
}

Obj apply_read(Obj pr , Obj env){
    char buffer[300];
    FILE *prev_stream = stream;
    stream = stdin;
    char *p = input(buffer , "" , true);
    while(!*p) p = input(buffer , "" , true);
    /* fgets until not null */
    stream = prev_stream;
    Token tok = NULL;
    tokenize(buffer , p , &tok);
    Obj val = parse(tok);
    free_token(tok);
    return val;
}

#define arith(pr , rtn , op , base) \
    Obj rtn = new(INTEGER , base); \
    int pr_len = length(pr); /* here*/ \
    int chk = 6 op 2; \
    if((6 op 2 > 5 && pr_len) || pr_len > 1){ \
        if(car(pr)->type == INTEGER){ \
            rtn->integer = car(pr)->integer; \
        } \
        else if(car(pr)->type == DECIMAL){ \
            rtn->type = DECIMAL; \
            rtn->decimal = car(pr)->decimal; \
        } \
        else{ \
            printf("cannot apply " xstr(op) " on non-number obj"); \
            print_obj(car(pr)); error("");\
        } \
        pr = cdr(pr); \
    } \
    for( ; pr ; pr = cdr(pr)){ \
        if(pr->type == NIL) \
        break; \
        if(6 op 3 == 2){ \
            if(num_of(car(pr)) == 0) \
            error("cannot div zero"); \
        } \
        if(!is_num(car(pr))) \
        error("cannot apply " xstr(op) " on non-number obj"); \
        else if(rtn->type == DECIMAL){ \
            HANDEL_DEC1(pr , rtn , op , base) \
        } \
        else if(rtn->type == INTEGER){ \
            if(car(pr)->type == INTEGER) \
            rtn->integer op ## = car(pr)->integer; \
            HANDEL_DEC2(pr , rtn , op , base) \
        } \
    }

#define apply_opr(op_name , op , base) \
    Obj apply_ ## op_name(Obj pr , Obj env){ \
        arith(pr , rtn , op , base); \
        return rtn; \
    }

#define HANDEL_DEC1(pr , rtn , op , base) \
    if(car(pr)->type == INTEGER) \
    rtn->decimal op ## = (double) car(pr)->integer; \
    else if(car(pr)->type == DECIMAL) \
    rtn->decimal op ## = car(pr)->decimal; \

#define HANDEL_DEC2(pr , rtn , op , base) \
    else if(car(pr)->type == DECIMAL){ \
        rtn->type = DECIMAL; \
        rtn->decimal = rtn->integer; \
        rtn->decimal op ## = car(pr)->decimal; \
    }

apply_opr(add , + , 0);
apply_opr(mul , * , 1);
apply_opr(sub , - , 0);
apply_opr(div , / , 1);

#undef HANDEL_DEC1
#undef HANDEL_DEC2
#define HANDEL_DEC1(pr , rtn , op , base) \
    error("cannot apply %% on decimal");
#define HANDEL_DEC2(pr , rtn , op , base) \
    else if(car(pr)->type == DECIMAL){ \
        error("cannot apply %% on decimal"); \
    }

apply_opr(mod , % , 1);
