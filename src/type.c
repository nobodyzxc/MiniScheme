#include "mem.h"
#include "type.h"

port_t stdin_port = {
    .fp = NULL ,
    .mode = "r" ,
    .ctx = NULL ,
    .ptr = NULL ,
    .name = "stdin" ,
};

const obj_t nil_t = { .type = NIL , .mark = true};
const obj_t eli_t = { .type = SYMBOL , .mark = true , .str = "..."};
const obj_t els_t = { .type = SYMBOL , .mark = true , .str = "else"};
const obj_t err_t = { .type = SYMBOL , .mark = true , .str = "(ERROR)"};
const obj_t true_t = { .type = BOOLEAN , .mark = true , .boolean = true};
const obj_t false_t = { .type = BOOLEAN , .mark = true , .boolean = false};

obj_t sip_t = { .type = PORT , .mark = true , .port = &stdin_port};
/* mark nil is very very important or mark it then get seg fault */
kObj nil = &nil_t;
kObj eli = &eli_t;
kObj els = &els_t;
kObj err = &err_t;
kObj true_obj = &true_t;
kObj false_obj = &false_t;

Obj stdin_pt = &sip_t;
Obj glenv;
