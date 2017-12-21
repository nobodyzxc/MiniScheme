#ifndef FUNC_H
#define FUNC_H
#include "type.h"
#include "mem.h"

Obj apply_gc(Obj pr , Obj env);
Obj apply_source(Obj pr , Obj env);
Obj apply_senv(Obj pr , Obj env);
Obj apply_apply(Obj pr , Obj env);
Obj apply_nullq(Obj pr , Obj env);
Obj apply_not(Obj pr , Obj env);
Obj apply_car(Obj pr , Obj env);
Obj apply_cdr(Obj pr , Obj env);
Obj apply_eqnum(Obj pr , Obj env);
Obj apply_length(Obj pr , Obj env);
Obj apply_add(Obj pr , Obj env);
Obj apply_sub(Obj pr , Obj env);
Obj apply_mul(Obj pr , Obj env);
Obj apply_div(Obj pr , Obj env);
Obj apply_mod(Obj pr , Obj env);
Obj apply_cons(Obj pr , Obj env);
Obj apply_display(Obj pr , Obj env);
Obj apply_listq(Obj pr , Obj env);
Obj apply_pairq(Obj pr , Obj env);
Obj apply_void(Obj pr , Obj env);
Obj apply_voidq(Obj pr , Obj env);
Obj apply_eqq(Obj pr , Obj env);
Obj apply_eqvq(Obj pr , Obj env);
Obj apply_equalq(Obj pr , Obj env);
Obj apply_symbolq(Obj pr , Obj env);
Obj apply_procedureq(Obj pr , Obj env);
Obj apply_lt(Obj pr , Obj env);
Obj apply_gt(Obj pr , Obj env);
Obj apply_let(Obj pr , Obj env);
Obj apply_get(Obj pr , Obj env);
Obj apply_read(Obj pr , Obj env);

Obj apply_clos(Obj pcr , Obj args , Obj env);
#endif
