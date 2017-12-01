#include <string.h>
#include <ctype.h>
#include "parse.h"
#include "mem.h"

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
        obj = new(BOOLEAN , parse_bool(v));
    else if(isdigit(v[0])){
        if(!strchr(v , '.'))
            obj = new(INTEGER, parse_integer(v));
        else
            obj = new(DECIMAL , parse_decimal(v));
    }
    else if(v[0] == '"')
        obj = new(STRING ,strndup(v + 1 , strlen(v) - 2));
    else if(EQS(v , "nil"))
        obj = (Obj)nil;
    else
        obj = new(SYMBOL , strdup(v));
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
    cons_t head;
    Cons pr = &head;
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
            pr->cdr = new_cons(sublist , NULL);
        }
        else{
            pr->cdr = new_cons(parse_listlit(tok) , NULL);
            tok = tok->next;
        }
        pr = pr->cdr;
    }
    if(!tok) error("miss )");
    else tok = tok->next;
    if(!last_elt_cnt)
        pr->cdr = new_cons(nil , NULL);
    (*rtn) = new(PAIR , head.cdr);
    return tok;
}
