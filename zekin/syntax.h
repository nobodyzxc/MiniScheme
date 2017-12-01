#ifndef SYNTAX_H
#define SYNTAX_H
Obj apply_if(Cons pr , Obj env);
Obj apply_quote(Cons pr , Obj env);
Obj apply_and(Cons args , Obj env);
Obj apply_or(Cons args , Obj env);
Obj apply_define(Cons args , Obj env);
Obj apply_lambda(Cons args , Obj env);
#endif
