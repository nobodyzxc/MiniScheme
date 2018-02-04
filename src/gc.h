#ifndef GC_H
#define GC_H

#include "type.h"
#include "memory.h"

typedef struct obj_list_tag *ObjList;
typedef struct obj_list_tag obj_list_t;

struct obj_list_tag{
    Obj v;
    ObjList next;
};

void gc();
void auto_try_gc();
void gc_list_cons(Obj obj);
#endif
