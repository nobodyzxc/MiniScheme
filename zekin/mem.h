#ifndef MEM_H
#define MEM_H
#include "type.h"
#include "token.h"
#include "gc.h"

#define new(TYPE , ...) \
    new_ ## TYPE(__VA_ARGS__)


Cons new_cons(kObj car  , kObj cdr);
Token new_token(char *p , Token next);

Obj  new_BOOLEAN(bool);
Obj  new_INTEGER(long long);
Obj  new_DECIMAL(double);
Obj  new_CHAR   (char);
Obj  new_STRING (char*);
Obj  new_SYMBOL (char*);
Obj  new_PAIR   (Cons kons);
Obj  new_CLOSURE(Obj expr , Obj env_obj);
Obj  new_EXPR   (char *name , Obj args , Obj body);
Obj  new_ENV    (Obj env);
Obj  new_SYNTAX  (char *name , func_ptr fp);
Obj  new_FUNCTION(char *name , func_ptr fp);
Obj new_MACRO(Obj keyws , Obj rules);

//Obj  copy_obj(Obj obj);
//Cons copy_cons(Cons pr);

void free_obj(Obj obj);
void free_cons(Cons);
void free_cons_shallow(Cons pr);
void free_token(Token tok);

void add_symbol(Obj sym , Obj val , Obj env_obj);
extern FILE *memchk;
#endif
