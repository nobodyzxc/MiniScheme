#ifndef UTIL_H
#define UTIL_H
#include "type.h"
bool is_list(Obj pr);
bool cmp_num(Obj a , Obj b);
int patnum(Obj pr);
int  length(Obj pr);
void print_type(Obj obj);
void print_pair(kObj pr);
void print_cons(Cons kons);
void print_obj(kObj obj);
void print_symtree(Symtree tree);
Obj cons(kObj head , kObj body);
Obj zipped_env(Obj syms , Obj args , Obj env);

#define error(fmt , ...) \
    printf(fmt , ##__VA_ARGS__) , exit(1)

#define stdin_printf(fmt , ...) \
    printf(stream == stdin ? fmt : "" , ##__VA_ARGS__)

#define alert(str , obj) \
    printf(str) , print_obj(obj) , puts("")

#endif
