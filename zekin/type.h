#ifndef TYPE_H
#define TYPE_H
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define EQS(a , b) (!strcmp(a , b))
#define is_num(obj) \
    ((obj)->type == INTEGER || (obj)->type == DECIMAL)
#define num_of(obj) \
    ((obj)->type == INTEGER ? (obj)->integer : (obj)->decimal)
#define is_false(obj) \
    ((obj)->type == BOOLEAN && (obj)->boolean == false)
#define is_true(obj) \
    ((obj)->type == BOOLEAN && (obj)->boolean == true)
#define is_nil(obj) ((obj)->type == NIL)
#define xstr(s) str(s)
#define str(s)  #s

enum types{
    BOOLEAN ,
    INTEGER ,
    DECIMAL ,
    CHAR    ,
    STRING  ,
    SYMBOL  ,
    PAIR    ,
    NIL     ,
    SYNTAX  ,
    FUNCTION,
    CLOSURE ,
    EXPR    ,
    ENV     ,
};

typedef enum types type_t;
typedef struct obj_tag  *Obj;
typedef struct obj_tag  obj_t;
typedef const  obj_t    *kObj;
typedef struct cons_tag *Cons;
typedef struct cons_tag cons_t;
typedef struct proc_tag *Proc;
typedef struct proc_tag proc_t;
typedef struct clos_tag clos_t;
typedef struct clos_tag *Clos;
typedef struct expr_tag expr_t;
typedef struct expr_tag *Expr;
typedef struct symt_tag symtree_t;
typedef struct symt_tag *Symtree;
typedef struct env_tag  env_t;
typedef struct env_tag  *Env;
typedef Obj (*func_ptr)(Obj , Obj);

struct cons_tag{
    Obj car;
    Obj cdr;
};

struct obj_tag{
    type_t type;
    bool mark;
    union{
        bool         boolean;
        long long    integer;
        double       decimal;
        char         chr;
        char         *str;
        Proc         proc;
        Cons         pair;
        Clos         clos;
        Expr         expr;
        Env          env;
    };
};

struct symt_tag{
    Obj sym;
    Obj val;
    Symtree lt;
    Symtree rt;
};

struct env_tag{
    Symtree symtab;
    Obj parent;
};

struct expr_tag{
    char *name;
    Obj args;
    Obj body;
};

struct clos_tag{
    Obj exp;
    Obj env;
};

struct proc_tag{
    char *name;
    func_ptr apply;
};

extern kObj nil;
extern Obj glenv;
extern char *type_name[];
#endif
