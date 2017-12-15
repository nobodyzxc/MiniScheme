#ifndef UTIL_H
#define UTIL_H
#include "type.h"
#define cdr(p) ((p)->pair->cdr)
#define car(p) ((p)->pair->car)
#define cadr(p) (car(cdr(p)))
#define caar(p) (car(car(p)))
#define caaar(p) (car(car(car(p))))
#define cdar(p) (cdr(car(p)))
#define cdaar(p) (cdr(car(car(p))))
bool is_list(Obj pr);
bool cmp_num(Obj a , Obj b);
int pat_num(Obj pr);
int  length(Obj pr);
void print_pair(kObj pr);
void print_cons(Cons kons);
void print_obj(kObj obj);
void print_symtree(Symtree tree);
Obj cons(kObj head , kObj body);
Obj zipped_env(Obj syms , Obj args , Obj env);
Obj zip_pat(Obj pat , Obj args , Obj env);
void print_esc(char *str);
Obj lssym(Obj ls , Obj sym);
Obj lssym_rec(Obj ls , Obj sym);
Obj lsobj(Obj ls , Obj obj);
void detail(Obj obj);

#define error(fmt , ...) \
    printf(fmt , ##__VA_ARGS__) , exit(1)

#define stdin_printf(fmt , ...) \
    printf(stream == stdin ? fmt : "" , ##__VA_ARGS__)

#define alert(str , obj) \
    printf(str) , print_obj(obj) , puts("")

#define ASSERT(expr , msg) \
    if(!(expr)) error("%s : %s %d" , msg , __FILE__ , __LINE__)

#endif
