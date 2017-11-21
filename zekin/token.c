#include "type.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

    //do{printf(fmt , ##__VA_ARGS__) , exit(1);}while(0);
#define error(fmt , ...) \
    printf(fmt , ##__VA_ARGS__) , exit(1)

#define stdin_printf(fmt , ...) \
    printf(stream == stdin ? fmt : "" , ##__VA_ARGS__)

FILE *stream;
char ss[300];

token new_token(char *p , token next){
    token new_tok = (token)malloc(sizeof(token_t));
    new_tok->p = p;
    new_tok->next = next;
    return new_tok;
}

void free_token(token tok){
    token pre = tok;
    while(tok){
        pre = tok , tok = tok->next;
        free(pre->p);
        free(pre);
    }
}

char *input(bool not_null){ //getline?
    if(not_null)
        while(!fgets(ss , sizeof(ss) , stream));
    else
        if(!fgets(ss , sizeof(ss) , stream)) return NULL;
    ss[strlen(ss) - 1] = 0;
    return ss;
}

char read(){
    return getc(stream);
}

bool is_blank(char p){
    return strchr(" \n\r\t" , p) != NULL;
}

bool is_paren(char p){
    return strchr("()[]{}" , p) != NULL;
}

bool is_paren_l(char p){
    return strchr("([{" , p) != NULL;
}

bool is_paren_r(char p){
    return strchr(")]}" , p) != NULL;
}

char rev_paren(char p){
    char *paren = "([{)]}";
    char *v = strchr(paren , p);
    if(v) return *(v + ((v > paren + 2) ? -3 : 3));
    else return 0;
}

bool is_comnt(char p){
    char *c = ";#";
    for(int i = 0 ; i < strlen(c) ; i++)
        if(p == c[i]) return true;
    return false;
}

char *tokstr(char *p){
    while(*p
            && !(is_blank(*p))
            && !(is_paren(*p))
            && !(is_comnt(*p)))
        p++;
    return p;
}

void add_token(char *p , token *plast){
    token new_tok = new_token(p , NULL);
    if(*plast)
        (*plast)->next = new_tok , (*plast) = new_tok;
    else
        (*plast) = new_tok;
}

char *add_quote(char *p , token *plast){
    if(*p != '\'')
        error("unmatched quote %s\n" , p);
    else
        p += 1;
    while(*p == ' ') p++;
    if(*p == '(' || *p == '\''){
        add_token(strdup("(") , plast);
        add_token(strdup("quote") , plast);
        if(*p == '(')
            p = parse_list(p , &((*plast)->next) , plast);
        else
            p = add_quote(p , plast);
        add_token(strdup(")") , plast);
        return p;
    }
    else{
        p -= 1;
        char *d = tokstr(p);
        add_token(strndup(p , d - p) , plast);
        return d;
    }
}

char *parse_string(char *p , token *phead , token *ptail){
    if(*p != '"')
        error("parse string start with %c\n" , *p);
    char *q = p , buf[300];
    while((q = strchr(q + 1 , '"')) && *(q - 1) == '\\');
    if(q) add_token(strndup(p , q - p + 1) , ptail) , p = q + 1;
    else{
        if(strlen(p) > sizeof(buf))
                error("exceed string limit\n");
        strcpy(buf , p);
        int l = strlen(buf);
        stdin_printf("... ");
        while(1){
            if(l > 300)
                error("exceed string limit\n");
            else
                buf[l] = read();
            if(buf[l] == '"' && buf[l - 1] != '\\'){
                add_token(strndup(buf , l + 1) , ptail);
                p = input(true);
                break;
            }
            if(buf[l] == '\n')
                stdin_printf("... ") , l -= 1;
            l++;
        }
    }
    return p;
}

char *parse_atom(char *p , token *phead , token *ptail){
    token_t head = {.p = NULL , .next = NULL};
    token last = &head;
    while(is_blank(*p)) p++;
    if(is_paren_r(*p))
        error("unmatched paren while parsing atom\n");
    if(*p == '\'')
        p = add_quote(p , &last);
    else if(*p == ';' || *p == '#')
        while(*p) p++;
    else if(*p == '"')
        p = parse_string(p , &head.next , &last);
    else{
        char *d = tokstr(p);
        add_token(strndup(p , d - p) , &last);
        p = d;
    }
    (*phead) = head.next;
    if(ptail) (*ptail) = last;
    return p;
}

char *parse_list(char *p , token *phead , token *ptail){
    char hp = *p;
    if(!is_paren_l(*p))
        error("unmatched list %s\n" , p);
    p += 1;
    token head , tail;
    head = tail = new_token(strdup("(") , NULL); //must use strdup
    while(1){
        if(!*p)
            stdin_printf("... ") , p = input(true);
        switch(*p){
            case '#':
            case ';':
                p = strchr(p , '\0') - 1;
                break;
            case '(':
            case '[':
            case '{':
                p = parse_list(p , &tail->next , &tail) - 1;
                break;
            case ')':
            case ']':
            case '}':
                if(*p != rev_paren(hp))
                    error("unmatched paren %c , %c\n" , hp , *p);
                add_token(strndup(p , 1) , &tail);
                (*phead) = head;
                if(ptail) (*ptail) = tail;
                return p + 1;
            case '"':
                p = parse_string(p , &tail->next , &tail) - 1;
                break;
            case '\'': //parse atom?
                p = add_quote(p , &tail) - 1;
                break;
            default:
                if(!is_blank(*p)){
                    char *d = tokstr(p);
                    add_token(strndup(p , d - p) , &tail);
                    p = d - 1;
                }
                break;
        }
        p += 1;
    }
}

void print_token(token tok){
    for( ; tok ; tok = tok->next)
        printf(" %s %s" , tok->p , tok->next ? ", " : " \n");
}

void tokenize(FILE *strm){
    size_t s = 100;
    char *p;
    stream = strm;
    //getline(&p , &s , stream);
    stdin_printf("> ");
    while((p = input(false))){
        while(*p){
            token head = NULL , t;
            while(*p && is_blank(*p)) p += 1;
            if(*p == '(') p = parse_list(p , &head , NULL);
            else p = parse_atom(p , &head , NULL);
            print_token(head);
            free_token(head);
        }
        stdin_printf("> ");
    }
    stdin_printf("\n");
}
