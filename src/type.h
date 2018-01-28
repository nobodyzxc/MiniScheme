#ifndef TYPE_H
#define TYPE_H
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define xstr(s) str(s)
#define str(s)  #s
#define EQS(a , b) (!strcmp(a , b))

#define is_bool(obj) \
    ((obj) && (obj)->type == BOOLEAN)

#define is_num(obj) \
    ((obj) && ((obj)->type == INTEGER || (obj)->type == DECIMAL))

#define is_exact(obj) \
    ((obj) && ((obj)->type == INTEGER))

#define num_of(obj) \
    ((obj)->type == INTEGER ? (obj)->integer : (obj)->decimal)
#define is_false(obj) \
    ((obj) && (obj)->type == BOOLEAN && (obj)->boolean == false)
#define is_true(obj) \
    (!(obj) || !is_false(obj))

#define is_pair(expr) ((expr) && (expr)->type == PAIR)
#define is_symbol(expr) ((expr) && (expr)->type == SYMBOL)

#define is_clos(expr) ((expr) && (expr)->type == CLOSURE)
#define is_macro(expr) ((expr) && (expr)->type == MACRO)
#define is_env(expr) ((expr) && (expr)->type == ENV)
#define is_port(expr) ((expr) && (expr)->type == PORT)
#define is_port_of(expr , mode) \
    (is_port(expr) && strstr(port_mode(expr) , (mode)))

#define is_nil(obj) ((obj) == (nil))
#define not_nil(obj) (!is_nil(obj))
#define iterable(obj) (is_pair(obj))

#define is_str(obj) ((obj) && (obj)->type == STRING)
#define is_selfeval(expr) \
    ((!expr) || (expr == err) || \
     ((expr) \
      && (expr)->type != PAIR \
      && (expr)->type != SYMBOL))

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
    MACRO   ,
    ENV     ,
    PORT    ,
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
typedef struct mac_tag  mac_t;
typedef struct mac_tag  *Mac;
typedef struct env_tag  env_t;
typedef struct env_tag  *Env;
typedef struct port_tag port_t;
typedef struct port_tag *Port;
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
        Mac          mac;
        Env          env;
        Port         port;
    };
};

struct expr_tag{
    char *name;
    Obj args;
    Obj body;
    /* body : a list of expr */
};

struct clos_tag{
    Obj exp;
    Obj env;
};

struct proc_tag{
    char *name;
    func_ptr apply;
};

struct symt_tag{
    Obj sym;
    Obj val;
    Symtree lt;
    Symtree rt;
    Symtree parent;
};

struct mac_tag{
    Obj keyws;
    Obj rules;
};

struct env_tag{
    Symtree symtab;
    Obj parent;
};

struct port_tag{
    FILE *fp;
    char *name;
    char *mode;
    char *ctx;
    char *ptr;
    bool open;
};

extern kObj nil;
extern kObj eli; /* ellipsis */
extern kObj els; /* else sym */
extern kObj err;
extern kObj true_obj;
extern kObj false_obj;
extern Obj glenv;
extern Obj stdin_pt;

#endif
