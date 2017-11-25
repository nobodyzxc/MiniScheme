#include "type.h"

char type_name[10][10] = {
    "boolean"   ,
    "integer"   ,
    "floating"  ,
    "character" ,
    "string"    ,
    "symbol"    ,
    "procedure" ,
    "keyword"   ,
    "nil"       ,
    "pair"      ,
};
#ifdef __cplusplus //or __GNUG__
const pair_t nil_pr = {
    .car = NULL ,
    .cdr =  NULL ,
};

const obj_t nil_t = {
    .type = NIL ,
    (Pair)&nil_pr ,
};
#else
const pair_t nil_pr = {
    .car = NULL ,
    .cdr = NULL ,
};

const obj_t nil_t = {
    .type = NIL ,
    .pair = (Pair)&nil_pr ,
};
#endif

kObj nil = &nil_t;
