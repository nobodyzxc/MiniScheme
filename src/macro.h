#ifndef MACRO_H
#define MACRO_H

#include "type.h"

bool has_eli(Obj);
int least_elts(Obj);
Obj expand(Obj , Obj);
bool match(Obj , Obj , Obj);
bool eli_match(Obj , Obj , Obj);
Obj substitue(Obj , Obj , Obj);

#endif
