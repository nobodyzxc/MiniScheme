#ifndef OPT_H
#define OPT_H

#include "type.h"

Obj lookup_sym_pool(char *s);
void push_sym_pool(Obj sym);
Obj find_last_expr(Obj exprs , Obj env);
Obj find_tail(Obj clos , Obj expr , Obj env);

Obj build_tail(Obj clos , Obj expr , Obj env);
typedef struct map_obj_tag MapObj;

struct map_obj_tag{
    char *key;
    Obj val;
};

#endif
