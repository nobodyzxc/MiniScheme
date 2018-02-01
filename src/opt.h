#ifndef OPT_H
#define OPT_H

#include "type.h"

void push_sym_pool(Obj sym);
Obj lookup_sym_pool(char *s);

Obj tco(Obj clos , Obj args , Obj env);

bool exist_lambda(Obj exprs);

Obj args_aloc(int len);
void args_rles(Obj head);
#endif
