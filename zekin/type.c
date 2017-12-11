#include "mem.h"
#include "type.h"

Obj glenv;

char *type_name[] = {
    (char*)"BOOLEAN ",
    (char*)"INTEGER ",
    (char*)"DECIMAL ",
    (char*)"CHAR    ",
    (char*)"STRING  ",
    (char*)"SYMBOL  ",
    (char*)"PAIR    ",
    (char*)"NIL     ",
    (char*)"SYNTAX  ",
    (char*)"FUNCTION",
    (char*)"CLOSURE ",
    (char*)"EXPR    ",
    (char*)"ENV     ",
};

#ifdef __cplusplus //or __GNUG__
const obj_t nil_t = { .type = NIL , .mark = true};
#else
const obj_t nil_t = { .type = NIL , .mark = true};
#endif
/* mark nil is very very important or mark it then get seg fault */
kObj nil = &nil_t;
