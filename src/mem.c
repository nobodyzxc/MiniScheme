#include "mem.h"
#include "gc.h"
#include "util.h"
#include "opt.h"
#include <stdlib.h>
#include <assert.h>

long long obj_count = 0;

long long get_obj_num(){
    return obj_count;
}

void *MALLOC(size_t size){
    return malloc(size);
}

void *FREE(void *p){
    free(p);
}

Obj new_obj(type_t type){
    obj_count++;
    Obj inst = (Obj)MALLOC(sizeof(obj_t));
    inst->type = type;
    inst->mark = false;
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
    return (Obj)(v ? true_obj : false_obj);
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
    Obj inst;
    if(inst = lookup_sym_pool(v))
        return inst;
    inst = new_obj(SYMBOL);
    inst->str = v;
    push_sym_pool(inst);
    return inst;
}

Obj new_PAIR   (Cons kons){
    Obj inst = new_obj(PAIR);
    inst->pair = kons;
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

Obj new_CLOSURE(Obj expr , Obj env){
    Obj inst = new_obj(CLOSURE);
    inst->clos = new_clos(expr , env);
    return inst;
}

Obj new_SYNTAX(char *name , func_ptr fp){
    Obj inst = new_FUNCTION(name , fp);
    inst->type = SYNTAX;
    return inst;
}

Obj new_MACRO(Obj keyws , Obj rules){
    Obj inst = new_obj(MACRO);
    inst->mac = malloc(sizeof(mac_t));
    inst->mac->keyws = keyws;
    inst->mac->rules = rules;
    return inst;
}

#define side(iter , diff) (diff < 0 ? (iter)->lt : (iter)->rt)
void add_symbol(Obj sym , Obj val , Obj env_obj){
    if(sym->type != SYMBOL){
        print_obj(sym);
        print_obj(val);
        puts("define only accept symbol");
        exit(0);
    }
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

void free_symtree(Symtree tree){
    if(!tree) return;
    if(tree->lt) free_symtree(tree->lt);
    if(tree->rt) free_symtree(tree->rt);
    free(tree);
}

void free_obj(Obj obj){
    obj_count--;
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

void free_token(Token tok){
    Token pre = tok;
    while(tok){
        pre = tok , tok = tok->next;
        FREE(pre->p);
        FREE(pre);
    }
}
