#ifndef TYPE_H
#define TYPE_H
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define EQS(a , b) (!strcmp(a , b))
#define is_num(obj) \
    ((obj)->type == INTEGER || (obj)->type == DECIMAL)
#define num_of(obj) \
    ((obj)->type == INTEGER ? (obj)->integer : (obj)->decimal)
#define xstr(s) str(s)
#define str(s)  #s

enum types{
    BOOLEAN ,
    INTEGER ,
    DECIMAL ,
    CHAR ,
    STRING ,
    SYMBOL ,
    PROCEDURE ,
    KEYWORD ,
    NIL ,
    PAIR ,
};

typedef enum types type_t;
typedef struct obj_tag *Obj;
typedef struct obj_tag obj_t;
typedef const  obj_t   *kObj;
typedef struct pair_tag *Pair;
typedef struct pair_tag pair_t;
typedef struct env_tag *Env;
typedef struct proc_tag *Proc;
typedef struct proc_tag proc_t;

extern char type_name[10][10];
/* char type_name[10][10] = {
    "boolean"   ,
    "integer"   ,
    "floating"  ,
    "character" ,
    "string"    ,
    "symbol"    ,
    "procedure" ,
    "keyword"   ,
    "nil"       ,
    "pair"      ,
};                          */

struct pair_tag{
    Obj  car;
    Pair cdr;
};

struct proc_tag{
    char *name;
    Obj (*func)(Pair);
};

struct obj_tag{
    type_t type;
    union{
        bool         boolean;
        long long    integer;
        double       decimal;
        char         character;
        char         *symbol;
        char         *string;
        Pair         pair;
        proc_t       proc;
    };
};

struct env_tag{
    Env env;
};

extern kObj nil;

#endif
