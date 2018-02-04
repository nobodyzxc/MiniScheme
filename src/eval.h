#ifndef EVAL_H
#define EVAL_H
#include "type.h"
#include "proc.h"
#include "token.h"
#include <stdbool.h>
Obj eval(Obj val , Obj env , Obj set);
Obj eval_symbol(Obj val , Obj env);
Obj eval_clos(Obj pcr , Obj args , Obj env);
Obj eval_func(Obj func , Obj args , Obj env , Obj set);
Obj eval_macro(Obj macro , Obj args , Obj env);
Obj map_eval(Obj val , Obj env , Obj set);
#endif
