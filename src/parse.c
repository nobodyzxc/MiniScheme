#include <string.h>
#include <ctype.h>
#include "mem.h"
#include "util.h"
#include "parse.h"

bool parse_bool(char *v){
    return *(v + 1) == 't' ? true : false;
}

double parse_decimal(char *v){
    return atof(v);
}

long long parse_integer(char *v){
    return atoll(v);
}

char *parse_str(char *beg , char *end){
    // "abcde"
    //  ^    ^ ya_strndup(v + 1 , strlen(v) - 2))
    char *v =                    /* for \0 */
        malloc(sizeof(char) * (end - beg) + 1);
    char *p = v;
    char *k = "abfnrtv\\'?\"" , *e = "\a\b\f\n\r\t\v\\\'\?\"";
    /* todo : parse \n \x \e \U \u */
    while(beg < end){
        if(*beg == '\\'){
            char *t = strchr(k , *(beg + 1));
            if(t) *p = e[t - k] , beg++;
            else *p = *beg;
        }
        else *p = *beg;
        p++ , beg++;
    }
    *p = 0;
    return v;
}

Obj new_lit(char *v){
    Obj obj;
    if(v[0] == '#') // not impl char yet
        obj = new(BOOLEAN , parse_bool(v));
    else if(isdigit(v[0]) || v[0] == '-' && isdigit(v[1])){
        if(!strchr(v , '.'))
            obj = new(INTEGER, parse_integer(v));
        else
            obj = new(DECIMAL , parse_decimal(v));
    }
    else if(v[0] == '"')
        obj = new(STRING , parse_str(v + 1 , v + strlen(v) - 1));
    else if(EQS(v , "nil"))
        obj = (Obj)nil;
    else if(EQS(v , "..."))
        obj = (Obj)eli;
    else if(EQS(v , "else"))
        obj = (Obj)els;
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
    bool last_elt = false;
    if(!EQS(tok->p , "("))
        error("parse %s as (" , tok->p);
    cons_t head;
    Cons pr = &head;
    tok = tok->next;
    while(tok && !EQS(tok->p , ")")){
        if(EQS(tok->p , ".")){
            if(last_elt)
                error("unexpected token: .\n");
            last_elt = true;
            tok = tok->next;
            continue;
        }
        if(last_elt) last_elt++;
        if(last_elt > 2) error("expected one element after .\n");
        if(EQS(tok->p , "(")){
            Obj sublist;
            tok = parse_list(tok , &sublist);
            pr->cdr = new(PAIR , new_cons(sublist , NULL));
        }
        else{
            if(last_elt)
                pr->cdr = parse_listlit(tok);
            else
                pr->cdr = new(PAIR , new_cons(parse_listlit(tok) , NULL));
            tok = tok->next;
        }
        pr = pr->cdr->pair;
    }
    if(!tok) error("miss )");
    else tok = tok->next;
    if(!last_elt)
        pr->cdr = (Obj)nil;
    (*rtn) = head.cdr;
    return tok;
}
