#ifndef PROC_H
#define PROC_H
#include "type.h"
#include "token.h"

Obj cons(kObj head , kObj body);

void print_obj(kObj obj);
/*
void print_bool (Obj);
void print_int  (Obj);
void print_float(Obj);
void print_char (Obj);
void print_str  (Obj);
void print_sym  (Obj);
void print_proc (Obj);
void print_keyw (Obj);
void print_nil  (Obj);
void print_pair (Obj);
void print_type (Obj);
*/

bool is_proc(char *s);
bool is_keyw(char *s);
bool is_list(Obj pr);

Obj lookup_symbol(char *p , Obj env_obj);

void init_buildins();
#endif
