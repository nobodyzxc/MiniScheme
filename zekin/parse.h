#ifndef PARSE_H
#define PARSE_H

#include "type.h"
#include "proc.h"
#include "token.h"

Obj parse_listlit(Token tok);
Obj parse(Token tok);
Token parse_list(Token tok , Obj *rtn);
#endif
