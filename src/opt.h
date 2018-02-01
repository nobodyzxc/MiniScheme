#ifndef OPT_H
#define OPT_H

#include "type.h"

void push_sym_pool(Obj sym);
Obj lookup_sym_pool(char *s);

Obj tco(Obj clos , Obj args , Obj env);

bool exist_lambda(Obj exprs);

typedef struct objls_tag *Objls;
typedef struct objls_tag objls_t;
typedef struct arg_elt  *Argelt;
typedef struct arg_elt  arg_elt_t;

struct objls_tag{
    Obj val;
    Objls next;
};

struct arg_elt{
    Obj args;
    Objls objs;
    Argelt next;
};

Argelt args_aloc(int len);
void args_rles(Argelt arg);
#endif
