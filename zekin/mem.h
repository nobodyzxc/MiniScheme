#ifndef MEM_H
#define MEM_H
#include "type.h"
#include "token.h"

Obj  new_obj(type_t type , const char *loc);

void free_obj(Obj obj , const char *loc);

Obj  copy_obj(Obj obj);

Pair new_pair(kObj car  , Pair cdr , const char *loc);

void free_pair(Pair , const char *loc);

Pair copy_pair(Pair pr);

Token new_token(char *p , Token next);

void free_token(Token tok);

void free_pair_shallow(Pair pr);

extern FILE *memchk;
#endif
