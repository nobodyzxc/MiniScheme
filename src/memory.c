#include "memory.h"
#include "gc.h"
#include "proc.h"
#include "util.h"
#include "opt.h"
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

long long obj_count = 0;
long long pr_count = 0;
long long mloc_size = 0;

long long get_obj_num(){
    return obj_count;
}

void *MALLOC(size_t size){
    void *mloc = malloc(size);
    mloc_size += malloc_usable_size(mloc);
    if(mloc_size > HEAP_SIZE)
        fprintf(stderr , "malloc size over %.1fG\n" ,
                ((float)HEAP_SIZE / 1000000000)) , exit(1);
    return mloc;
}

void *FREE(void *p){
    mloc_size -= malloc_usable_size(p);
    free(p);
}

Obj new_static_obj(type_t type){
    if(type == PAIR) pr_count++;
    obj_count++;
    Obj inst = (Obj)MALLOC(sizeof(obj_t));
    inst->type = type;
    inst->mark = false;
    return inst;
}

Obj new_obj(type_t type){
    Obj inst = new_static_obj(type);
    gc_list_cons(inst);
    return inst;
}

Cons new_cons(kObj car , kObj cdr){
    Cons inst = (Cons)MALLOC(sizeof(cons_t));
    inst->car = (Obj)car , inst->cdr = (Obj)cdr;
#ifdef LISTLEN_OPT
    if(cdr == nil)
        inst->len = 1;
    else if(is_pair(cdr))
        inst->len =
            cdr->pair->len +
            (cdr->pair->len > 0 ? 1 : -1);
    else inst->len = -1;
#endif
    return inst;
}

Clos new_clos(Obj expr , Obj env){
    Clos inst = (Clos)MALLOC(sizeof(clos_t));
    inst->exp = expr , inst->tr = false;
    //alert("expr ref env => " , expr);
    inst->env = env;
    env->env->ref = true;
    return inst;
}

/* exprs is a list of expris
 * so it must be a list */
Obj new_lambda(Obj args , Obj exprs){
    if(lambda_symbol == NULL)
        puts("cannot get lambda symbol");
    else if(!is_sympr(args) && !is_symbol(args))
        alert("define function with non-symbol args : " , args);
    else if(!is_pair(exprs))
        alert("define function with non-pair exprs : " , args);
    else return cons(lambda_symbol , cons(args , exprs));
    return (Obj)err;
}

Obj new_nested_lambda(Obj head , Obj body){
    if(!head)
        alert("define function with void head : " , head);
    else if(is_nil(head))
        alert("define function with nil head : " , head);
    else if(!is_pair(head))
        alert("define function with non-pair head : " , head);
    else if(is_symbol(car(head)))
        return new_lambda(cdr(head) , body);
    else if(is_pair(car(head)))
        return new_lambda(cdr(head) ,
                cons(new_nested_lambda(car(head) , body) , nil));
    /* lambda exprs expects a list of exprs ,
     * so cons new_nested_lambda with nil */
    else
        alert("define function with unknown head type : " , car(head));
    return (Obj)err;
}

Expr new_expr(char *name , Obj args , Obj body){
    Expr inst = (Expr)MALLOC(sizeof(expr_t));
    inst->name = name , inst->args = args , inst->body = body;
    return inst;
}

Env new_env(Obj env){
    Env inst = (Env)MALLOC(sizeof(env_t));
    inst->symtab = NULL , inst->parent = env;
    inst->ref = false;
    return inst;
}

Port new_port(FILE *fp , char *name , char *mode){
    Port inst = (Port)MALLOC(sizeof(port_t));
    inst->fp = fp , inst->name = name , inst->mode = mode;
    inst->ctx = NULL , inst->ptr = NULL , inst->open = true;
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

Obj set_INTEGER(long long v , Obj set){
    if(!is_num(set))
        return alert("space opt : set should be number , got" , set);
    set->type = INTEGER;
    set->integer = v;
    return set;
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
    inst = new_static_obj(SYMBOL);
    /* for opt */
    inst->str = v;
    push_sym_pool(inst);
    return inst;
}

Obj new_PAIR   (Cons kons){
    Obj inst = new_obj(PAIR);
    inst->pair = kons;
    return inst;
}

Obj set_PAIR   (Cons cons , Obj set){
    set->type = PAIR;
    set->pair = cons;
    return set;
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

Obj  new_PORT   (FILE *fp , char *name , char *mode){
    Obj inst = new_obj(PORT);
    inst->port = new_port(fp , name , mode);
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
    inst->mac = MALLOC(sizeof(mac_t));
    inst->mac->keyws = keyws;
    inst->mac->rules = rules;
    return inst;
}

Symtree
new_symtree(Obj s , Obj v ,
        Symtree parent , Symtree lt , Symtree rt){
    Symtree inst =
        (Symtree)MALLOC(sizeof(symtree_t));
    inst->sym = s , inst->val = v;
    inst->lt = lt , inst->rt = rt;
    inst->parent = parent;
    return inst;
}

void free_symtree(Symtree tree){
    if(!tree) return;
    if(tree->lt) free_symtree(tree->lt);
    if(tree->rt) free_symtree(tree->rt);
    FREE(tree);
}

void free_port(Port port){
    FREE(port->ctx);
    FREE(port->name);
    if(port->open){
        fclose(port->fp);
        port->open = false;
    }
    port->ctx = port->ptr = NULL;
}

void free_obj(Obj obj){
    if(!obj || obj->mark == true)
        return;
    obj_count--;
    if(!obj) return;
    if(is_nil(obj)) return;
    if(obj->type == STRING
            || obj->type == SYMBOL)
        FREE(obj->str);
    else if(obj->type == EXPR)
        FREE(obj->expr->name);
    else if(obj->type == ENV)
        free_symtree(obj->env->symtab);
    else if(obj->type == PORT)
        free_port(obj->port);
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

char *ya_strndup(const char *s, size_t size){
    char *n = MALLOC(sizeof(char) * size + 1);
    for(size_t i = 0 ; i < size ; i++)
        n[i] = s[i];
    n[size] = 0;
    return n;
}

char *ya_strdup(const char *s){
    int len = strlen(s);
    char *n = MALLOC(sizeof(char) * (len + 1));
    char *p = n;
    while(*s)
        *p = *s , p++ , s++;
    *p = *s;
    return n;
}
