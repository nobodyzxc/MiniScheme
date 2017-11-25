#ifndef MEM_H
#define MEM_H
#include "type.h"
#include "token.h"

Obj  new_obj(type_t type , const char *loc);

void free_obj(Obj);

Obj  copy_obj(Obj obj);

Pair new_pair(kObj car  , Pair cdr , const char *loc);

void free_pair(Pair);

Token new_token(char *p , Token next);

void free_token(Token tok);

extern FILE *memchk;
#endif
