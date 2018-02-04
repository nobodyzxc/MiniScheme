#ifndef SYNTAX_H
#define SYNTAX_H
Obj apply_if     (Obj args , Obj env , Obj set);
Obj apply_quote  (Obj args , Obj env , Obj set);
Obj apply_define (Obj args , Obj env , Obj set);
Obj apply_lambda (Obj args , Obj env , Obj set);
Obj apply_set    (Obj args , Obj env , Obj set);
Obj apply_set_car(Obj args , Obj env , Obj set);
Obj apply_set_cdr(Obj args , Obj env , Obj set);
Obj apply_syntax_rules(Obj args , Obj env , Obj set);
#endif
