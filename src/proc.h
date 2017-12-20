#ifndef PROC_H
#define PROC_H
#include "type.h"
#include "token.h"

Obj cons(kObj head , kObj body);

void print_obj(kObj obj);

bool is_proc(char *s);
bool is_keyw(char *s);
bool is_list(Obj pr);

Obj lookup_symbol(char *p , Obj env_obj);
Obj lookup_symenv(char *p , Obj env_obj);

void init_buildins();
#endif
