#include "mem.h"
#include "type.h"

Obj glenv;

const obj_t nil_t = { .type = NIL , .mark = true};
const obj_t eli_t = { .type = SYMBOL , .mark = true , .str = "..."};
const obj_t true_t = { .type = BOOLEAN , .mark = true , .boolean = true};
const obj_t false_t = { .type = BOOLEAN , .mark = true , .boolean = false};
/* mark nil is very very important or mark it then get seg fault */
kObj nil = &nil_t;
kObj eli = &eli_t;
kObj true_obj = &true_t;
kObj false_obj = &false_t;
