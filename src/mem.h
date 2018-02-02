#ifndef MEM_H
#define MEM_H
#include "type.h"
#include "token.h"
#include "gc.h"

#define new(TYPE , ...) \
    new_ ## TYPE(__VA_ARGS__)

#define set(TYPE , ...) \
    set_ ## TYPE(__VA_ARGS__)

void *MALLOC(size_t size);
void *FREE(void *p);

Cons new_cons(kObj car  , kObj cdr);
Token new_token(char *p , Token next);

Symtree new_symtree(Obj s , Obj v ,
        Symtree par , Symtree lt , Symtree rt);

Obj new_nested_lambda(Obj head , Obj body);

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
Obj  new_MACRO(Obj keyws , Obj rules);
Obj  new_PORT   (FILE *fp , char *name , char *mode);

Obj set_INTEGER(long long v , Obj set);
Obj set_PAIR   (Cons cons , Obj set);


void free_obj(Obj obj);
void free_token(Token tok);

long long get_obj_num();

char *ya_strndup(const char *s, size_t size);
char *ya_strdup(const char *s);

#endif
