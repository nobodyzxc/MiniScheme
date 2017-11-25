#include <string.h>
#include <ctype.h>
#include "parse.h"
#include "mem.h"
#define new_pair(a , b) new_pair(a , b , __FILE__ ":" xstr(__LINE__))
#define new_obj(a) new_obj(a , __FILE__ ":" xstr(__LINE__))

bool parse_bool(char *v){
    return *(v + 1) == 't' ? true : false;
}

double parse_decimal(char *v){
    return atof(v);
}

long long parse_integer(char *v){
    return atoi(v);
}

Obj new_lit(char *v){
    Obj obj;
    if(v[0] == '#') // not impl char yet
        obj = new_obj(BOOLEAN) , obj->boolean = parse_bool(v);
    else if(isdigit(v[0])){
        if(!strchr(v , '.'))
            obj = new_obj(INTEGER) , obj->integer = parse_integer(v);
        else
            obj = new_obj(DECIMAL) , obj->decimal = parse_decimal(v);
    }
    else if(v[0] == '"')
        obj = new_obj(STRING) ,
            obj->string = strndup(v + 1 , strlen(v) - 2);
    else if(EQS(v , "nil"))
        obj = (Obj)nil;
    else
        obj = new_obj(SYMBOL) , obj->symbol = strdup(v);
    return obj;
}

// <bool> | <num> | <symbol> | <list>
Obj parse(Token tok){
    if(tok == NULL) return NULL;
    Obj rtn;
    if(EQS(tok->p , "("))
        parse_list(tok , &rtn);
    else
        rtn = parse_listlit(tok);
    return rtn;
}

// <list-lit>       ::= <id> | <num> | <bool> |  ( <list-lit>* )
Obj parse_listlit(Token tok){
    return new_lit(tok->p);
}

Token parse_list(Token tok , Obj *rtn){
    int last_elt_cnt = 0;
    if(!EQS(tok->p , "("))
        error("parse %s as (" , tok->p);
    pair_t head;
    Pair pr = &head;
    tok = tok->next;
    while(tok && !EQS(tok->p , ")")){
        if(EQS(tok->p , ".")){
            if(last_elt_cnt)
                error("unexpected token: .\n");
            last_elt_cnt = 1;
            tok = tok->next;
            continue;
        }
        if(last_elt_cnt) last_elt_cnt++;
        if(last_elt_cnt > 2) error("expected one element after .\n");
        if(EQS(tok->p , "(")){
            Obj sublist;
            tok = parse_list(tok , &sublist);
            pr->cdr = new_pair(sublist , NULL);
        }
        else{
            pr->cdr = new_pair(parse_listlit(tok) , NULL);
            tok = tok->next;
        }
        pr = pr->cdr;
    }
    if(!tok) error("miss )");
    else tok = tok->next;
    if(!last_elt_cnt)
        pr->cdr = new_pair(nil , NULL);
    (*rtn) = new_obj(PAIR);
    (*rtn)->pair = head.cdr;
    return tok;
}
