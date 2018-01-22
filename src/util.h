#ifndef UTIL_H
#define UTIL_H
#include "type.h"

#define car(p) ((p)->pair->car)
#define cdr(p) ((p)->pair->cdr)
#define cadr(p) (car(cdr(p)))
#define caddr(p) (car(cdr(cdr(p))))
#define cadddr(p) (car(cdr(cdr(cdr(p)))))
#define caar(p) (car(car(p)))
#define caaar(p) (car(car(car(p))))
#define cdar(p) (cdr(car(p)))
#define cdaar(p) (cdr(car(car(p))))

#define clos_expr(t) ((t)->clos->exp)
#define clos_body(t) ((t)->clos->exp->expr->body)
#define clos_args(t) ((t)->clos->exp->expr->args)
#define clos_env(t) ((t)->clos->env)


bool is_list(Obj pr);
bool cmp_num(Obj a , Obj b);
bool eqv(Obj a , Obj b);
bool equal(Obj a , Obj b);
int pat_num(Obj pr);
int  length(Obj pr);
void fprint_pair(FILE *s , kObj pr);
void print_pair(kObj pr);
void print_cons(Cons kons);
void print_obj(kObj obj);
void fprint_obj(FILE *s , kObj obj);
void print_symtree(Symtree tree);
Obj cons(kObj head , kObj body);

Obj zip_env(Obj syms , Obj args , Obj env);

#define zipped_env(syms , args , env) \
    zip_env(syms , args , new(ENV , env))

Obj zip_pat(Obj pat , Obj args , Obj env);
void print_esc(char *str);
Obj lssym(Obj ls , Obj sym);
Obj lssym_rec(Obj ls , Obj sym);
Obj lsobj(Obj ls , Obj obj);
Obj prid(Obj ls);
bool is_sympr(Obj ls);
void detail(Obj obj);

#define error(fmt , ...) \
    printf(fmt , ##__VA_ARGS__) , exit(1)

#define stdin_printf(fmt , ...) \
    printf(stream == stdin ? (fmt) : "" , ##__VA_ARGS__)

#define falert(s , str , obj) \
    fprintf(s , str) , fprint_obj(s , obj) , fputs("" , s)

Obj alert(char *str , Obj obj);

#define msgobjc(str , obj) \
    fprintf(stdout , str) , detail(obj)

#define msgobj(str , obj) \
    fprintf(stdout , str) , detail(obj) , puts("")

#define ASSERT(expr , msg) \
    if(!(expr)) error("%s : %s %d" , msg , __FILE__ , __LINE__)

#endif
