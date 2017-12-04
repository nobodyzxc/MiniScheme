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
const obj_t nil_t = { .type = NIL };
#else
const obj_t nil_t = { .type = NIL };
#endif
kObj nil = &nil_t;
