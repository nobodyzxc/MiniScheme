#ifndef UTIL_H
#define UTIL_H
#include "type.h"
bool is_list(Cons pr);
bool cmp_num(Obj a , Obj b);
int length(Cons pr);
void print_type(Obj obj);
void print_pair(kObj pr);
void print_cons(Cons kons);
void print_obj(kObj obj);

#define error(fmt , ...) \
    printf(fmt , ##__VA_ARGS__) , exit(1)

#define stdin_printf(fmt , ...) \
    printf(stream == stdin ? fmt : "" , ##__VA_ARGS__)
#endif
