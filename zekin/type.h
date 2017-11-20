#ifndef TYPE_H
#define TYPE_H
#include <stdio.h>
#include <stdbool.h>
typedef enum types{
    BOOLEAN ,
    INTEGER ,
    FLOAT ,
    CHAR ,
    STRING ,
    SYMBOL ,
    PROCEDURE ,
    KEYWORD ,
    NIL ,
    PAIR ,
} type_t;

typedef struct obj_tag *Obj;
typedef struct obj_tag obj_t;
typedef const  obj_t   *kObj;

typedef struct pair_tag *Pair;
typedef struct pair_tag pair_t;

struct obj_tag{
    type_t type;
    union{
        bool  boolean;
        int   integer;
        float floating;
        char  character;
        char  *symbol;
        char  *string;
        Pair  pair;
    };
};

struct pair_tag{
    Obj  car;
    Pair cdr;
};

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

Obj  cons(kObj head , kObj body);

Obj  new_obj(type_t type);

Pair new_pair(kObj car  , Pair cdr);

void print_obj(kObj obj);

extern kObj nil;

#endif
