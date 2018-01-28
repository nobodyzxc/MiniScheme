#ifndef PROC_H
#define PROC_H
#include "type.h"
#include "token.h"

bool is_proc(char *s);
bool is_keyw(char *s);
bool is_list(Obj pr);

Obj search_symbol(char *p , Obj env_obj);
Obj lookup_symbol(char *p , Obj env_obj);
Obj lookup_symenv(char *p , Obj env_obj);
Obj add_symbol(Obj sym , Obj val , Obj env_obj);

void init_buildins();

extern Obj lambda_symbol;

#define LoR(node , v) \
    (v > 0 ? &((node)->lt) : &((node)->rt))
#define cmp_node(s , n) \
    (strcmp((s) , (n)->sym->str))
#endif
