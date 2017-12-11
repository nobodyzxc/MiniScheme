#include "mem.h"
#include "gc.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>
#define LEAKFILE ".leak"

FILE *memchk = NULL;

void *MALLOC(size_t size){
    return malloc(size);
}

void *FREE(void *p){
    free(p);
}

Obj new_obj(type_t type){
    Obj inst = (Obj)MALLOC(sizeof(obj_t));
    inst->type = type;
    gc_list_cons(inst);
    return inst;
}

Cons new_cons(kObj car , kObj cdr){
    Cons inst = (Cons)MALLOC(sizeof(cons_t));
    inst->car = (Obj)car , inst->cdr = (Obj)cdr;
    return inst;
}

Clos new_clos(Obj expr , Obj env){
    Clos inst = (Clos)MALLOC(sizeof(clos_t));
    inst->exp = expr , inst->env = env;
    return inst;
}

Expr new_expr(char *name , Obj args , Obj body){
    Expr inst = (Expr)MALLOC(sizeof(expr_t));
    inst->name = name , inst->args = args , inst->body = body;
    return inst;
}

Env new_env(Obj env){
    Env inst = (Env)MALLOC(sizeof(env_t));
    inst->symtab = NULL , inst->parent = env;
    return inst;
}

Token new_token(char *p , Token next){
    Token new_tok = (Token)MALLOC(sizeof(token_t));
    new_tok->p = p;
    new_tok->next = next;
    return new_tok;
}

Obj new_BOOLEAN(bool v){
    Obj inst = new_obj(BOOLEAN);
    inst->boolean = v;
    return inst;
}

Obj new_INTEGER(long long v){
    Obj inst = new_obj(INTEGER);
    inst->integer = v;
    return inst;
}

Obj new_DECIMAL(double v){
    Obj inst = new_obj(DECIMAL);
    inst->decimal = v;
    return inst;
}

Obj new_CHAR   (char v){
    Obj inst = new_obj(CHAR);
    inst->chr = v;
    return inst;
}

Obj new_STRING (char *v){
    Obj inst = new_obj(STRING);
    inst->str = v;
    return inst;
}

Obj new_SYMBOL (char* v){
    Obj inst = new_obj(SYMBOL);
    inst->str = v;
    return inst;
}

Obj new_PAIR   (Cons kons){
    Obj inst = new_obj(PAIR);
    inst->pair = kons;
    return inst;
}

Obj new_CLOSURE(Obj expr , Obj env){
    Obj inst = new_obj(CLOSURE);
    inst->clos = new_clos(expr , env);
    return inst;
}

Obj new_EXPR   (char *name , Obj args , Obj body){
    Obj inst = new_obj(EXPR);
    inst->expr = new_expr(name , args , body);
    return inst;
}

Obj new_ENV    (Obj env){
    Obj inst = new_obj(ENV);
    inst->env = new_env(env);
    return inst;
}

Obj new_FUNCTION(char *name , func_ptr fp){
    Obj inst = new_obj(FUNCTION);
    inst->proc = (Proc)MALLOC(sizeof(proc_t));
    inst->proc->name = name;
    inst->proc->apply = fp;
    return inst;
}

Obj new_SYNTAX(char *name , func_ptr fp){
    Obj inst = new_FUNCTION(name , fp);
    inst->type = SYNTAX;
    return inst;
}

#define side(iter , diff) (diff < 0 ? (iter)->lt : (iter)->rt)
void add_symbol(Obj sym , Obj val , Obj env_obj){
    assert(sym->type == SYMBOL);
    Symtree inst = (Symtree)MALLOC(sizeof(symtree_t));
    inst->sym = sym , inst->val = val;
    inst->lt = NULL , inst->rt = NULL;
    Symtree iter = env_obj->env->symtab;
    if(!iter) env_obj->env->symtab = inst;
    else{
        while(1){
            int df = strcmp(iter->sym->str , sym->str);
            if(!df){ // redef symbol
                iter->val = val;
                return;
            }
            if(side(iter , df))
                iter = side(iter , df);
            else{
                if(df < 0)
                    iter->lt = inst;
                else
                    iter->rt = inst;
                return;
            }
        }
    }
}

//Obj  copy_obj(Obj obj){
//    puts("copy_obj not ready yet");
//    // evil
//    exit(0);
//    if(obj->type == NIL)
//        return obj;
//    else if(obj->type == STRING)
//        return new(STRING , strdup(obj->str));
//    else if(obj->type == SYMBOL)
//        return new(SYMBOL , strdup(obj->str));
//    else if(obj->type == PAIR)
//        return new(PAIR , copy_cons(obj->pair));
//    else if(obj->type == SYNTAX)
//        return new(SYNTAX , obj->proc->name , obj->proc->apply);
//    else if(obj->type == FUNCTION)
//        return new(FUNCTION , obj->proc->name , obj->proc->apply);
//    else if(obj->type == CLOSURE)
//        return new(CLOSURE ,
//                copy_obj(obj->clos->exp),
//                copy_obj(obj->clos->env));
//    else if(obj->type == EXPR)
//        return new(EXPR ,
//                strdup(obj->expr->name),
//                copy_obj(obj->expr->args),
//                copy_obj(obj->expr->body));
//    else if(obj->type == ENV)
//        return new(ENV , obj->env->parent);
//    else{
//        // shallow copy can handle
//        // bool , int , dec , chr
//        Obj inst = new_obj(obj->type);
//        (*inst) = (*obj);
//        return inst;
//    }
//}

//Cons copy_cons(Cons pr){
//    Cons inst = new_cons(pr->car , NULL);
//    Cons it = inst;
//    pr = pr->cdr;
//    while(pr){
//        it->cdr = new_cons(pr->car , NULL);
//        it = it->cdr , pr = pr->cdr;
//    }
//    return inst;
//}

void free_symtree(Symtree tree){
    if(!tree) return;
    if(tree->lt) free_symtree(tree->lt);
    if(tree->rt) free_symtree(tree->rt);
    free(tree);
}

void free_obj(Obj obj){
    printf("free : ") , print_obj(obj) , puts("");
    if(!obj) return;
    if(obj->type == NIL)
        return;
    if(obj->type == STRING || obj->type == SYMBOL)
        free(obj->str);
    else if(obj->type == EXPR)
        free(obj->expr->name);
    else if(obj->type == ENV)
        free_symtree(obj->env->symtab);
    FREE(obj);
}

//void free_cons(Cons pr){
//    if(!pr) return;
//    char s[100];
//    while(pr && pr->cdr){
//        free_obj(pr->car);
//        Cons fp = pr;
//        pr = pr->cdr;
//        FREE(fp);
//    }
//    if(pr->car->type == NIL){
//        FREE(pr);
//    }
//}
//
//void free_cons_shallow(Cons pr){
//    while(pr && pr->cdr){
//        Cons fp = pr;
//        pr = pr->cdr;
//        FREE(fp);
//    }
//    if(pr->car->type == NIL){
//        FREE(pr);
//    }
//}

void free_token(Token tok){
    Token pre = tok;
    while(tok){
        pre = tok , tok = tok->next;
        FREE(pre->p);
        FREE(pre);
    }
}
