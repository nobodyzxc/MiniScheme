#include "mem.h"
#include "type.h"

Obj glenv;

#ifdef __cplusplus //or __GNUG__
const obj_t nil_t = { .type = NIL , .mark = true};
const obj_t eli_t = { .type = SYMBOL , .mark = true , .str = "..."};
#else
const obj_t nil_t = { .type = NIL , .mark = true};
const obj_t eli_t = { .type = SYMBOL , .mark = true , .str = "..."};
#endif
/* mark nil is very very important or mark it then get seg fault */
kObj nil = &nil_t;
kObj eli = &eli_t;
