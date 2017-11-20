#include "type.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
        free(pre->p) , free(pre);
    }
}

char *input(){
    char *p;
    printf("... ");
    //    if(getline(&p , NULL , stream) == EOF)
    //        exit(0);
    while(!fgets(ss , sizeof(ss) , stdin));
    ss[strlen(ss) - 1] = 0;
    return ss;
}

bool is_blank(char p){
    char *c = " \n\r\t";
    for(int i = 0 ; i < strlen(c) ; i++)
        if(p == c[i]) return true;
    return false;
}

bool is_paren(char p){
    char *c = "()[]";
    for(int i = 0 ; i < strlen(c) ; i++)
        if(p == c[i]) return true;
    return false;
}
char *tokstr(char *p){
    while(*p
            && !(is_blank(*p))
            && !(is_paren(*p)))
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
        printf("unmatched quote %s\n" , p) , exit(0);
    else
        p += 1;
    while(*p == ' ') p++;
    if(*p == '(' || *p == '\''){
        add_token(strdup("(") , plast);
        add_token(strdup("quote") , plast);
        if(*p == '('){
            p = parse_list(p , &((*plast)->next));
            while((*plast)->next) (*plast) = (*plast)->next;
        }
        else{
            p = add_quote(p , plast);
        }
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

char *parse_atom(char *p , token *phead){
    token_t head;
    token last = &head;
    while(is_blank(*p)) p++;
    if(*p == ')') puts("unmatched paren") , exit(1);
    if(*p == '\'')
        p = add_quote(p , &last);
    else{
        char *d = tokstr(p);
        add_token(strndup(p , d - p) , &last);
        p = d;
    }
    (*phead) = head.next;
    return p;
}

char *parse_list(char *p , token *phead){
    if(*p != '(')
        printf("unmatched list %s\n" , p) , exit(0);
    p += 1;
    int paren = 1;
    char *str = NULL;
    token head , tail;
    head = tail = new_token("(" , NULL);
    while(paren){
        if(!*p){
            if(paren) p = input();
            else break;
        }
        switch(*p){
            case '#':
            case ';':
                if(!str)
                    return strchr(p , '\0');
                else
                    break;
            case '(':
            case ')':
                if(!str){
                    paren += (*p == '(') * 2 - 1;
                    if(paren < 0) puts("unmatched paren") , exit(1);
                    add_token(strndup(p , 1) , &tail);
                }
                break;
            case '"':
                if(str)
                    add_token(strndup(str , p - str + 1) , &tail)
                        , str = NULL;
                else
                    str = p;
                break;
            case '\'':
                if(!str)
                    p = add_quote(p , &tail) - 1;
                break;
            default:
                if(!str && *p != ' '){
                    char *d = tokstr(p);
                    add_token(strndup(p , d - p) , &tail);
                    p = d - 1;
                }
                break;
        }
        p += 1;
    }
    (*phead) = head;
    return p;
}

void print_token(token tok){
    for( ; tok ; tok = tok->next)
        printf(" %s %s" , tok->p , tok->next ? ", " : " \n");
}

void tokenize(FILE *strm){
    size_t s = 100;
    char *p;
    stream = strm;
    //getline(&p , &s , stdin);
    printf("> ");
    while(fgets(ss , sizeof(ss) , stdin)){
        p = ss;
        p[strlen(p) - 1] = 0;
        while(*p){
            token head = NULL , t;
            while(*p && *p == ' ') p += 1;
            if(*p == '(') p = parse_list(p , &head);
            else p = parse_atom(p , &head);
            print_token(head);
            //free_token(head);
        }
        printf("> ");
    }
    puts("");
}
