#include "mem.h"
#include "type.h"

Obj glenv;

char *type_name[] = {
    "BOOLEAN ",
    "INTEGER ",
    "DECIMAL ",
    "CHAR    ",
    "STRING  ",
    "SYMBOL  ",
    "PAIR    ",
    "NIL     ",
    "SYNTAX  ",
    "FUNCTION",
    "CLOSURE ",
    "EXPR    ",
    "ENV     ",
};

#ifdef __cplusplus //or __GNUG__
const cons_t nil_pr = { .car = NULL , .cdr =  NULL , };
const obj_t nil_t = { .type = NIL , (Cons)&nil_pr , };
#else
const cons_t nil_pr = { .car = NULL , .cdr = NULL , };
const obj_t nil_t = { .type = NIL , .pair = (Cons)&nil_pr , };
#endif

kObj nil = &nil_t;
