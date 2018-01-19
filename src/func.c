#include "mem.h"
#include "func.h"
#include "util.h"
#include "eval.h"
#include "main.h"
#include "parse.h"
#include "token.h"

Obj apply_exit(Obj args , Obj env){
    if(length(args) > 0 &&
            car(args)->type == INTEGER)
        exit((int)(car(args)->integer));
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

Obj apply_gc(Obj args , Obj env){
    gc();
    return NULL;
}

Obj apply_source(Obj args , Obj env){
    if(length(args) != 1)
        alert("source only accept 1 arg : " , args);
    else if(car(args)->type != STRING)
        puts("source file name must be string");
    else{
        if(load_script(car(args)->str))
            stdin_printf("ok , \"%s\" loaded\n" , car(args)->str);
        else
            printf("cannot load \"%s\"\n" , car(args)->str);
    }
    return NULL;
}

Obj apply_senv(Obj args , Obj env){
    if(length(args) != 1)
        alert("senv only accept 1 arg : " , args);
    args = car(args);
    if(args->type == ENV)
        print_symtree(args->env->symtab);
    return NULL;
}

Obj apply_apply(Obj args , Obj env){
    if(length(args) != 2)
        alert("apply only accept 2 arg : " , args);
    else if(car(args)->type == CLOSURE)
        return apply_clos(car(args) ,
                cadr(args) , env);
    else if(car(args)->type == FUNCTION)
        return car(args)->proc->apply(
                cadr(args) , env);
    else{
        printf("cannot apply ");
        print_obj(car(args));
        alert(" on " , cadr(args));
    }
    return NULL;
}

Obj apply_nullq(Obj args , Obj env){
    if(length(args) != 1)
        alert("null? only accept 1 arg : " , args);
    else
        return new(BOOLEAN , IS_NIL(car(args)));
    return NULL;
}

Obj apply_listq(Obj args , Obj env){
    if(length(args) != 1)
        alert("list? only accept 1 arg : " , args);
    else
        return new(BOOLEAN , is_list(car(args)));
    return NULL;
}

Obj apply_pairq(Obj args , Obj env){
    if(length(args) != 1)
        alert("pair? only accept 1 arg : " , args);
    else
        return new(BOOLEAN , car(args)->type == PAIR);
    return NULL;
}

Obj apply_flush_output(Obj args , Obj env){
    fflush(stdout);
    return NULL;
}

Obj apply_display(Obj args , Obj env){
    /* todo : extend arity up to 2
     * to support output-port? */
    if(length(args) != 1)
        alert("display? only accept 1 arg : " , args);
    else if(!car(args)) /* handle void */
        print_obj(args);
    else if(car(args)->type == STRING)
        print_esc(car(args)->str);
    else if(car(args)->type == CLOSURE)
        detail(car(args));
    else
        print_obj(car(args));
    return NULL;
}

Obj apply_length(Obj args , Obj env){
    if(length(args) != 1)
        alert("length only accept 1 arg : " , args);
    else if(car(args)->type == PAIR)
        return new(INTEGER , length(car(args)));
    else
        alert("cannot apply length on " , car(args));
    return NULL;
}

Obj apply_car(Obj args , Obj env){
    if(length(args) != 1)
        alert("car only accept 1 arg : " , args);
    else if(car(args) && car(args)->type == PAIR)
        return caar(args);
    else
        alert("cannot apply car on " , car(args));
    return NULL;
}

Obj apply_cdr(Obj args , Obj env){
    if(length(args) != 1)
        alert("cdr only accept 1 arg : " , args);
    else if(car(args)->type == PAIR)
        return cdar(args);
    else
        alert("cannot apply cdr on " , car(args));
    return NULL;
}

Obj apply_cons(Obj args , Obj env){
    // assert arity == 2
    if(length(args) != 2)
        alert("cons only accept 2 arg : " , args);
    else
        return new(PAIR , new_cons(car(args) , cadr(args)));
    return NULL;
}

Obj apply_eqnum(Obj args , Obj env){
    // assert arity > 1
    if(length(args) < 2)
        alert("= accept at least 2 arg : " , args);
    else{
        bool rtn = true;
        Obj head = car(args);
        while(args->type == PAIR)
            if(is_num(car(args)))
                rtn &= cmp_num(head , car(args)) , args = cdr(args);
            else{
                alert("apply = on non-number obj" , car(args));
                return NULL;
            }
        return new(BOOLEAN , rtn);
    }
    return NULL;
}

Obj apply_eqq(Obj args , Obj env){
    //assert arity == 2
    if(length(args) == 2)
        alert("eq? only accept 2 arg : " , args);
    else
        return new(BOOLEAN , car(args) == cadr(args));
    return NULL;
}

Obj apply_eqvq(Obj args , Obj env){
    //assert arity == 2
    if(length(args) == 2)
        alert("eqv? only accept 2 arg : " , args);
    else
        return new(BOOLEAN , eqv(car(args) , cadr(args)));
    return NULL;
}

Obj apply_equalq(Obj args , Obj env){
    if(length(args) == 2)
        alert("equal? only accept 2 arg : " , args);
    else
        return new(BOOLEAN , equal(car(args) , cadr(args)));
    return NULL;
}

#define apply_cmp(name , op) \
    Obj apply_ ## name (Obj args , Obj env){ \
        if(length(args) < 2) \
        alert(str(op) " accept at least 2 arg : " , args); \
        else{ \
            for( ; args && !IS_NIL(args) && !IS_NIL(cdr(args)) ; \
                    args = cdr(args)) \
            if(!(num_of(car(args)) op num_of(cadr(args)))) \
            return (Obj)false_obj; \
            return (Obj)true_obj; \
        } \
        return NULL; \
    }

apply_cmp(lt , <);
apply_cmp(gt , >);
apply_cmp(let , <=);
apply_cmp(get , >=);

Obj apply_not(Obj args , Obj env){
    //assert airth == 1
    if(length(args) == 1)
        alert("not only accept 1 arg : " , args);
    else
        return new(BOOLEAN , IS_FALSE(car(args)));
    return NULL;
}

Obj apply_void(Obj args , Obj env){
    return NULL;
}

Obj apply_voidq(Obj args , Obj env){
    if(length(args) == 1)
        alert("void? only accept 1 arg : " , args);
    else
        return new(BOOLEAN , car(args) == NULL);
    return NULL;
}

Obj apply_symbolq(Obj args , Obj env){
    if(length(args) == 1)
        alert("symbol? only accept 1 arg : " , args);
    else
        return new(BOOLEAN , car(args)->type == SYMBOL);
    return NULL;
}

Obj apply_procedureq(Obj args , Obj env){
    if(length(args) == 1)
        alert("procedure? only accept 1 arg : " , args);
    else
        return new(BOOLEAN , car(args)
                && (car(args)->type == CLOSURE
                    || car(args)->type == FUNCTION));
    return NULL;
}

Obj apply_read(Obj args , Obj env){
    char buffer[300];
    FILE *prev_stream = stream;
    stream = stdin;
    /* fgets until not null */
    Token tok = NULL;
    tokenize(buffer ,
            get_non_blank(buffer , NULL) ,
            &tok);
    Obj val = parse(tok);
    free_token(tok);
    stream = prev_stream;
    return val;
}

#define arith(args , rtn , op , base) \
    Obj rtn = new(INTEGER , base); \
    int pr_len = length(args); /* here*/ \
    int chk = 6 op 2; \
    if((6 op 2 > 5 && pr_len) || pr_len > 1){ \
        if(car(args)->type == INTEGER){ \
            rtn->integer = car(args)->integer; \
        } \
        else if(car(args)->type == DECIMAL){ \
            rtn->type = DECIMAL; \
            rtn->decimal = car(args)->decimal; \
        } \
        else{ \
            printf("cannot apply " xstr(op) " on non-number obj"); \
            print_obj(car(args)); error("");\
        } \
        args = cdr(args); \
    } \
    for( ; args ; args = cdr(args)){ \
        if(args->type == NIL) \
        break; \
        if(6 op 3 == 2){ \
            if(num_of(car(args)) == 0) \
            error("cannot div zero"); \
        } \
        if(!is_num(car(args))) \
        error("cannot apply " xstr(op) " on non-number obj"); \
        else if(rtn->type == DECIMAL){ \
            HANDEL_DEC1(args , rtn , op , base) \
        } \
        else if(rtn->type == INTEGER){ \
            if(car(args)->type == INTEGER) \
            rtn->integer op ## = car(args)->integer; \
            HANDEL_DEC2(args , rtn , op , base) \
        } \
    }

#define apply_opr(op_name , op , base) \
    Obj apply_ ## op_name(Obj args , Obj env){ \
        arith(args , rtn , op , base); \
        return rtn; \
    }

#define HANDEL_DEC1(args , rtn , op , base) \
    if(car(args)->type == INTEGER) \
    rtn->decimal op ## = (double) car(args)->integer; \
    else if(car(args)->type == DECIMAL) \
    rtn->decimal op ## = car(args)->decimal; \

#define HANDEL_DEC2(args , rtn , op , base) \
    else if(car(args)->type == DECIMAL){ \
        rtn->type = DECIMAL; \
        rtn->decimal = rtn->integer; \
        rtn->decimal op ## = car(args)->decimal; \
    }

apply_opr(add , + , 0);
apply_opr(mul , * , 1);
apply_opr(sub , - , 0);
apply_opr(div , / , 1);

#undef HANDEL_DEC1
#undef HANDEL_DEC2
#define HANDEL_DEC1(args , rtn , op , base) \
    error("cannot apply %% on decimal");
#define HANDEL_DEC2(args , rtn , op , base) \
    else if(car(args)->type == DECIMAL){ \
        error("cannot apply %% on decimal"); \
    }

apply_opr(mod , % , 1);
