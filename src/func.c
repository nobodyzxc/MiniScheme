#include "memory.h"
#include "main.h"
#include "opt.h"
#include "func.h"
#include "util.h"
#include "eval.h"
#include "main.h"
#include "parse.h"
#include "token.h"
#include "io.h"

#include <math.h>

/* predictors */
Obj apply_eqq(Obj args , Obj env , Obj set){
    if(length(args) != 2)
        alert("eq? : only accepts 2 args , got " , args);
    else
        return new(BOOLEAN , car(args) == cadr(args));
    return (Obj)err;
}

Obj apply_eqvq(Obj args , Obj env , Obj set){
    if(length(args) != 2)
        alert("eqv? : only accepts 2 args , got " , args);
    else
        return new(BOOLEAN , eqv(car(args) , cadr(args)));
    return (Obj)err;
}

Obj apply_equalq(Obj args , Obj env , Obj set){
    if(length(args) != 2)
        alert("equal? : only accepts 2 args , got " , args);
    else
        return new(BOOLEAN , equal(car(args) , cadr(args)));
    return (Obj)err;
}

Obj apply_nullq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("null? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_nil(car(args)));
    return (Obj)err;
}

Obj apply_listq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("list? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_list(car(args)));
    return (Obj)err;
}

Obj apply_pairq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("pair? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , car(args)->type == PAIR);
    return (Obj)err;
}

Obj apply_booleanq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("boolean? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_bool(car(args)));
    return (Obj)err;
}

Obj apply_numberq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("number? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_num(car(args)));
    return (Obj)err;
}

Obj apply_exactq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("exact? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_exact(car(args)));
    return (Obj)err;
}

Obj apply_integerq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("integer? : only accepts 1 arg , got " , args);
    else if(is_num(car(args)))
        return new(BOOLEAN ,
                is_exact(car(args))
                || floor(num_of(car(args))) == num_of(car(args)));
    else
        alert("integer? : only accepts number , got " , args);
    return (Obj)err;
}

Obj apply_stringq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("string? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_str(car(args)));
    return (Obj)err;
}

Obj apply_closureq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("closure? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_clos(car(args)));
    return (Obj)err;
}

Obj apply_envq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("env? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_env(car(args)));
    return (Obj)err;
}

Obj apply_portq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("port? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_port(car(args)));
    return (Obj)err;
}

Obj apply_output_portq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("output-port? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_port_of(car(args) , "w"));
    return (Obj)err;
}

Obj apply_input_portq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("input-port? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_port_of(car(args) , "r"));
    return (Obj)err;
}

/* precedures */
Obj apply_exit(Obj args , Obj env , Obj set){
    if(length(args) > 0 &&
            car(args)->type == INTEGER)
            end_io() , exit((int)(car(args)->integer));
    end_io() , exit(0);
    return NULL;
}

Obj apply_gc(Obj args , Obj env , Obj set){
    gc();
    return NULL;
}

Obj apply_source(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("source : only accepts 1 arg , got " , args);
    else if(car(args)->type != STRING)
        puts("source file name must be string");
    else if(load_script(car(args)->str , true)){
        imode_msg("ok , %-24s loaded\n" , car(args)->str);
        return NULL;
    }
    return (Obj)err;
}

Obj apply_system(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("system : only accepts 1 arg , got " , args);
    else if(car(args)->type != STRING)
        puts("system : command must be string");
    else if(!system(car(args)->str))
        return NULL;
    return (Obj)err;
}

Obj apply_eval(Obj args , Obj env , Obj set){
    int len = length(args);
    if(len != 1 && len != 2)
        alert("eval : accepts 1 to 2 args , got " , args);
    else if(len == 2 && !is_env(cadr(args)))
        alert("eval : second arg must be env , got " , cadr(args));
    else if(len == 1)
        return eval(car(args) , env , NULL);
    else if(len == 2)
        return eval(car(args) , cadr(args) , NULL);
    else
        alert("cannot eval " , args);
    return (Obj)err;
}

Obj apply_apply(Obj args , Obj env , Obj set){
    if(length(args) != 2)
        alert("apply : only accepts 2 args , got " , args);
    else if(car(args)->type == CLOSURE)
#ifdef TCO_OPT
        return tco(car(args) , cadr(args) , env);
#else
        return eval_clos(car(args) , cadr(args) , env);
#endif
    else if(car(args)->type == FUNCTION)
        return car(args)->proc->apply(
                cadr(args) , env , NULL);
    else{
        printf("cannot apply ");
        print_obj(car(args));
        alert(" on " , cadr(args));
    }
    return (Obj)err;
}

Obj apply_get_env(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("get-env : only accepts 1 arg , got " , args);
    else if(!car(args) || car(args)->type != CLOSURE)
        puts("get-env only accepts closure");
    else
        return clos_env(car(args));
    return (Obj)err;
}

Obj apply_get_curenv(Obj args , Obj env , Obj set){
    return env;
}

Obj apply_lookup_symbol(Obj args , Obj env , Obj set){
    if(length(args) != 2)
        alert("lookup-symbol : only accepts 2 arg , got " , args);
    else if(!car(args) || car(args)->type != SYMBOL)
        alert("lookup-symbol's 1st arg should be symbol , got " , car(args));
    else if(!cadr(args) || cadr(args)->type != ENV)
        alert("lookup-symbol's 2nd arg should be environment , got " , cadr(args));
    else{
        Obj res = search_symbol(car(args)->str , cadr(args));
        if(res != err)
            return res;
        else
            alert("lookup-symbol : cannot found symbol " , car(args));
    }
    return (Obj)err;
}

Obj apply_flush_output(Obj args , Obj env , Obj set){
    fflush(stdout);
    return NULL;
}

Obj apply_display(Obj args , Obj env , Obj set){
    /* todo : extend arity up to 2
     * to support output-port? */
    int len = length(args);
    if(len != 1 && len != 2)
        alert("display : only accepts 1 to 2 args , got " , args);
    else if(len == 2 && !is_port_of(cadr(args) , "w"))
        alert_dtl("display : second arg must be out-port? , got " , args);
    else if(len == 2 && !port_open(cadr(args)))
        alert_dtl("display : output to a closed port => " , cadr(args));
    else{
        FILE *fp = len == 2 ? port_fp(cadr(args)) : stdout;
        if(is_str(car(args)))
            fprintf(fp , car(args)->str);
        else
            fprint_obj_dtl(fp , car(args));
        return NULL;
    }
    return (Obj)err;
}

Obj apply_length(Obj args , Obj env , Obj set){
    int len = 0;
    if(length(args) != 1)
        alert("length : only accepts 1 arg , got " , args);
    else if(!is_pair(car(args)))
        alert("cannot apply length on " , car(args));
    else if((len = length(car(args))) >= 0){
        if(!set)
            return new(INTEGER , len);
        else
            return set(INTEGER , len , set);
    }
    else
        alert("cannot apply length on pr : " , car(args));
    return (Obj)err;
}

Obj apply_car(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("car : only accepts 1 arg , got " , args);
    else if(car(args) && car(args)->type == PAIR){
        env_ref(env) = true;
        return caar(args);
    }
    else
        alert("cannot apply car on " , car(args));
    return (Obj)err;
}

Obj apply_cdr(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("cdr : only accepts 1 arg , got " , args);
    else if(car(args)->type == PAIR){
        env_ref(env) = true;
        /* important */
        return cdar(args);
    }
    else
        alert("cannot apply cdr on " , car(args));
    return (Obj)err;
}

Obj apply_cons(Obj args , Obj env , Obj set){
    if(length(args) != 2)
        alert("cons : only accepts 2 args , got " , args);
    else{
        env_ref(env)= true;
        if(!set)
            return new(PAIR , new_cons(car(args) , cadr(args)));
        else
            return set(PAIR , new_cons(car(args) , cadr(args)) , set);
    }
    return (Obj)err;
}

Obj apply_eqnum(Obj args , Obj env , Obj set){
    if(length(args) < 2)
        alert("= : accepts at least 2 args , got " , args);
    else{
        bool rtn = true;
        Obj head = car(args);
        while(args->type == PAIR)
            if(is_num(car(args)))
                rtn &= cmp_num(head , car(args)) , args = cdr(args);
            else
                return alert("apply = on non-number obj " , car(args));
        return new(BOOLEAN , rtn);
    }
    return (Obj)err;
}

#define apply_cmp(name , op) \
    Obj apply_ ## name (Obj args , Obj env , Obj set){ \
        if(length(args) < 2) \
        alert(str(op) " : accepts at least 2 args , got " , args); \
        else{ \
            for( ; iterable(args) && not_nil(cdr(args)) ; \
                    args = cdr(args)) \
            if(!(num_of(car(args)) op num_of(cadr(args)))) \
            return (Obj)false_obj; \
            return (Obj)true_obj; \
        } \
        return (Obj)err; \
    }

apply_cmp(lt , <);
apply_cmp(gt , >);
apply_cmp(let , <=);
apply_cmp(get , >=);

Obj apply_not(Obj args , Obj env , Obj set){
    //assert airth == 1
    if(length(args) != 1)
        alert("not : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , is_false(car(args)));
    return (Obj)err;
}

Obj apply_void(Obj args , Obj env , Obj set){
    return NULL;
}

Obj apply_voidq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("void? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , car(args) == NULL);
    return (Obj)err;
}

Obj apply_symbolq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("symbol? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , car(args)->type == SYMBOL);
    return (Obj)err;
}

Obj apply_procedureq(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("procedure? : only accepts 1 arg , got " , args);
    else
        return new(BOOLEAN , car(args)
                && (car(args)->type == CLOSURE
                    || car(args)->type == FUNCTION));
    return (Obj)err;
}

Obj apply_read(Obj args , Obj env , Obj set){
    /* todo : input-port ? */
    int len = length(args);
    if(len != 0 && len != 1)
        return alert("read : only accepts 0 to 1 arg , got " , args);
    else if(len && !is_port_of(car(args) , "r"))
        return alert("read : first arg must be in-port? , got " , car(args));
    else if(len && !port_open(car(args)))
        return alert_dtl("read : read from a closed port => " , car(args));

    Obj prev_pt = read_pt;
    /* default stdin */
    read_pt = len ? car(args) : stdin_pt;

    Token tok = NULL;
    port_ptr(read_pt) = read_non_blank(port_ptr(read_pt) , "");

    if(port_ptr(read_pt) == NULL){
        read_pt = prev_pt;
        return warning("recv EOF while applying read");
    }

    tok_raw_input = read_raw_input;
    tok_non_blank = read_non_blank;
    port_ptr(read_pt) = tokenize(port_ptr(read_pt) , &tok);
    if(port_ptr(read_pt) == NULL){
        read_pt = prev_pt;
        return warning("recv EOF while applying read");
    }
#ifdef PURE_READ
    clear_ctx(read_pt);
#endif
    read_pt = prev_pt;
    Obj val = parse(tok);
    free_token(tok);
    return val;
}

Obj apply_open_inport(Obj args , Obj env , Obj set){
    Obj rtn = NULL;
    if(length(args) != 1)
        alert("open-in-port : only accepts 1 arg , got " , args);
    else if(!is_str(car(args)))
        alert("open-in-port : first arg must be string , got " , car(args));
    else if((rtn = open_port(car(args)->str , "r")) != err)
        return rtn;
    else
        alert("open-in-port : cannot open file " , car(args));
    return (Obj)err;
}

Obj apply_open_outport(Obj args , Obj env , Obj set){
    Obj rtn = NULL;
    if(length(args) != 1)
        alert("open-out-port : only accepts 1 arg , got " , args);
    else if(!is_str(car(args)))
        alert("open-out-port : first arg must be string , got " , car(args));
    else if((rtn = open_port(car(args)->str , "w")) != err)
        return rtn;
    else
        alert("open-out-port : cannot open file " , car(args));
    return (Obj)err;
}

Obj apply_fclose(Obj args , Obj env , Obj set){
    if(length(args) != 1)
        alert("fclose : only accepts 1 arg , got " , args);
    else if(!is_port(car(args)))
        alert("fclose : first arg must be port , got " , car(args));
    else{
        close_port(car(args));
        return NULL;
    }
    return (Obj)err;
}

#define arith(args , rtn , op , base) \
    int pr_len = length(args); \
    if((6 op 2 > 5 && pr_len) || pr_len > 1){ \
        if(car(args)->type == INTEGER){ \
            rtn->integer = car(args)->integer; \
        } \
        else if(car(args)->type == DECIMAL){ \
            rtn->type = DECIMAL; \
            rtn->decimal = car(args)->decimal; \
        } \
        else{ \
            printf("cannot apply %s on non-number obj " , xstr(op)); \
            fflush(stdout); \
            return alert("" , car(args)); \
        } \
        args = cdr(args); \
    } \
    for( ; args ; args = cdr(args)){ \
        if(is_nil(args)) \
        break; \
        if(6 op 3 == 2 || 6 op 3 == 0){ \
            if(num_of(car(args)) == 0){ \
                printf("%s" , xstr(op)) , fflush(stdout);    \
                return alert(" : arg cannot be zero , got " , car(args)); \
            } \
        } \
        if(!is_num(car(args))){ \
            printf("cannot apply %s on non-number obj " , xstr(op)); \
            fflush(stdout); \
            return alert("" , car(args)); \
        } \
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
    Obj apply_ ## op_name(Obj args , Obj env , Obj set){ \
        if(set && !is_num(set)){ \
            return alert("space opt : set should be number , got" , set); \
        } \
        obj_t val; \
        val.type = INTEGER; \
        val.integer = base; \
        Obj rtn = &val; \
        arith(args , rtn , op , base); \
        if(!set){ \
            rtn = new(INTEGER , base); \
            *rtn = val; \
        } \
        else{ \
            *set = val; \
            rtn = set; \
        } \
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
{ \
    printf("cannot apply %% on decimal\n"); \
    return (Obj) err; \
}
#define HANDEL_DEC2(args , rtn , op , base) \
    else if(car(args)->type == DECIMAL){ \
        printf("cannot apply %% on decimal\n"); \
        return (Obj) err; \
    }
#define MOD
apply_opr(mod , % , 1);
