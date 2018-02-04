#ifndef FUNC_H
#define FUNC_H
#include "type.h"
#include "memory.h"

Obj apply_gc(Obj args , Obj env , Obj set);
Obj apply_exit(Obj args , Obj env , Obj set);
Obj apply_source(Obj args , Obj env , Obj set);
Obj apply_system(Obj args , Obj env , Obj set);
Obj apply_eval(Obj args , Obj env , Obj set);
Obj apply_apply(Obj args , Obj env , Obj set);
Obj apply_nullq(Obj args , Obj env , Obj set);
Obj apply_not(Obj args , Obj env , Obj set);
Obj apply_car(Obj args , Obj env , Obj set);
Obj apply_cdr(Obj args , Obj env , Obj set);
Obj apply_eqnum(Obj args , Obj env , Obj set);
Obj apply_length(Obj args , Obj env , Obj set);
Obj apply_add(Obj args , Obj env , Obj set);
Obj apply_sub(Obj args , Obj env , Obj set);
Obj apply_mul(Obj args , Obj env , Obj set);
Obj apply_div(Obj args , Obj env , Obj set);
Obj apply_mod(Obj args , Obj env , Obj set);
Obj apply_cons(Obj args , Obj env , Obj set);
Obj apply_flush_output(Obj args , Obj env , Obj set);
Obj apply_display(Obj args , Obj env , Obj set);
Obj apply_listq(Obj args , Obj env , Obj set);
Obj apply_pairq(Obj args , Obj env , Obj set);
Obj apply_void(Obj args , Obj env , Obj set);
Obj apply_voidq(Obj args , Obj env , Obj set);
Obj apply_eqq(Obj args , Obj env , Obj set);
Obj apply_eqvq(Obj args , Obj env , Obj set);
Obj apply_equalq(Obj args , Obj env , Obj set);
Obj apply_symbolq(Obj args , Obj env , Obj set);
Obj apply_numberq(Obj args , Obj env , Obj set);
Obj apply_stringq(Obj args , Obj env , Obj set);
Obj apply_booleanq(Obj args , Obj env , Obj set);
Obj apply_exactq(Obj args , Obj env , Obj set);
Obj apply_integerq(Obj args , Obj env , Obj set);
Obj apply_closureq(Obj args , Obj env , Obj set);
Obj apply_procedureq(Obj args , Obj env , Obj set);
Obj apply_envq(Obj args , Obj env , Obj set);
Obj apply_portq(Obj args , Obj env , Obj set);
Obj apply_output_portq(Obj args , Obj env , Obj set);
Obj apply_input_portq(Obj args , Obj env , Obj set);
Obj apply_open_outport(Obj args , Obj env , Obj set);
Obj apply_open_inport(Obj args , Obj env , Obj set);
Obj apply_fclose(Obj args , Obj env , Obj set);

Obj apply_lt(Obj args , Obj env , Obj set);
Obj apply_gt(Obj args , Obj env , Obj set);
Obj apply_let(Obj args , Obj env , Obj set);
Obj apply_get(Obj args , Obj env , Obj set);
Obj apply_read(Obj args , Obj env , Obj set);

Obj apply_get_env(Obj args , Obj env , Obj set);
Obj apply_get_curenv(Obj args , Obj env , Obj set);
Obj apply_lookup_symbol(Obj args , Obj env , Obj set);
#endif
