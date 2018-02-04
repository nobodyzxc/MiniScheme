#include <string.h>
#include <ctype.h>
#include "memory.h"
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
        MALLOC(sizeof(char) * (end - beg) + 1);
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
    if(v[0] == '#'
            && (v[1] == 't' || v[1] == 'f')
            && v[2] == '\0') // not impl char yet
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
        obj = new(SYMBOL , ya_strdup(v));
    return obj;
}

// <bool> | <num> | <symbol> | <list>
Obj parse(Token tok){
    if(!tok) puts("parse NULL tok") , exit(1);
    if(tok == NULL) return NULL;
    Obj val;
    if(EQS(tok->p , "("))
        parse_list(tok , &val);
    else
        val = parse_listlit(tok);
    return val;
}

// <list-lit>       ::= <id> | <num> | <bool> |  ( <list-lit>* )
Obj parse_listlit(Token tok){
    return new_lit(tok->p);
}


#ifdef LISTLEN_OPT
void remark_len(Obj lst , int len){
    int d = len < 0 ? 1 : -1;
    while(iterable(lst)){
        lst->pair->len = len;
        lst = cdr(lst);
        len += d;
    }
}
#endif

Token parse_list(Token tok , Obj *rtn){
#ifdef LISTLEN_OPT
    int list_len = 0;
#endif
    bool is_pair = false;
    if(!EQS(tok->p , "("))
        error("parse %s as (" , tok->p);
    cons_t head;
    Cons pr = &head;
    tok = tok->next;
    while(tok && !EQS(tok->p , ")")){
        if(EQS(tok->p , ".")){
            if(pr == &head){
                puts(". : need first elt");
                *rtn = (Obj)err;
                return tok;
            }
            tok = tok->next;
            if(EQS(tok->p , ")")){
                puts(". : need second elt");
                *rtn = (Obj)err;
                return tok;
            }
            is_pair = true;
        }
        if(EQS(tok->p , "(")){
            Obj sublist;
            tok = parse_list(tok , &sublist);
            if(sublist == err){
                *rtn = (Obj)err;
                return tok;
            }
            pr->cdr = is_pair ?  sublist :
                new(PAIR , new_cons(sublist , NULL));
        }
        else{
            Obj lit = parse_listlit(tok);
            if(lit == err){
                *rtn = (Obj)err;
                return tok;
            }
            pr->cdr = is_pair ? lit :
                new(PAIR , new_cons(lit , NULL));

            tok = tok->next;
        }
#ifdef LISTLEN_OPT
        list_len++;
#endif
        pr = pr->cdr->pair;
        if(is_pair){
            if(tok && EQS(tok->p , ")")){
#ifdef LISTLEN_OPT
            list_len = -list_len;
#endif
                break;
            }
            else{
                puts(". : can only followed by 1 elt");
                *rtn = (Obj)err;
                return tok;
            }
        }
    }
    if(!tok) error("miss token : \")\"");
    else tok = tok->next;
    if(!is_pair)
        pr->cdr = (Obj)nil;
    (*rtn) = head.cdr;
#ifdef LISTLEN_OPT
    remark_len(*rtn , list_len);
#endif
    return tok;
}
