#include "token.h"
#include "util.h"
#include "mem.h"

#include <string.h>
#include <stdbool.h>

char ss[300];

char *tok_list(char *p , Token *phead , Token *ptail);

char *input(const char *prompt , bool lock){ //getline?
    stdin_printf(prompt);
    if(lock)
        while(!fgets(ss , sizeof(ss) , stream));
    else
        if(!fgets(ss , sizeof(ss) , stream)) return NULL;
    ss[strlen(ss) - 1] = 0;
    return ss;
}

char read_char(){
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
    char *paren = (char*)"([{)]}";
    char *v = strchr(paren , p);
    if(v) return *(v + ((v > paren + 2) ? -3 : 3));
    else return 0;
}

bool is_comnt(char p){
    char *c = (char*)";";
    for(int i = 0 ; i < strlen(c) ; i++)
        if(p == c[i]) return true;
    return false;
}

char *tokstr(char *p){
    while(*p
            && !(is_blank(*p))
            && !(is_paren(*p))
            && !(is_comnt(*p))
            && !(*p == '"'))
        p++;
    return p;
}

void add_token(char *p , Token *plast){
    Token new_tok = new_token(p , NULL);
    if(*plast)
        (*plast)->next = new_tok , (*plast) = new_tok;
    else
        (*plast) = new_tok;
}

char *get_non_blank(char *p){
    while(*p && is_blank(*p)) p++;
    while(!*p){
        p = input("... " , true);
        while(*p && is_blank(*p)) p++;
    }
    return p;
}

char *add_quote(char *p , Token *plast){
    if(*p != '\'')
        error("unmatched quote %s\n" , p);
    else
        p += 1;

    p = get_non_blank(p);

    add_token(strdup("(") , plast);
    add_token(strdup("quote") , plast);
    if(*p == '(' || *p == '\''){
        if(*p == '(')
            p = tok_list(p , &(*plast)->next , plast);
        else
            p = add_quote(p , plast);
    }
    else{
        char *s = p;
        add_token(strndup(s , (p = tokstr(p)) - s) , plast);
    }
    add_token(strdup(")") , plast);
    return p;
}

char *tok_string(char *p , Token *phead , Token *ptail){
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
                buf[l] = read_char();
            if(buf[l] == '"' && buf[l - 1] != '\\'){
                add_token(strndup(buf , l + 1) , ptail);
                p = input("" , true);
                break;
            }
            if(buf[l] == '\n') stdin_printf("... ") , l -= 1;
            l++;
        }
    }
    return p;
}

char *tok_atom(char *p , Token *phead , Token *ptail){
    token_t head = {.p = NULL , .next = NULL};
    Token last = &head; // here
    while(*p && is_blank(*p)) p++;
    if(is_paren_r(*p))
        error("unmatched paren while parsing atom\n");
    if(*p == '\'')
        p = add_quote(p , &last);
    else if(*p == ';')
        while(*p) p++;
    else if(*p == '"')
        p = tok_string(p , &head.next , &last);
    else{
        char *d = tokstr(p);
        add_token(strndup(p , d - p) , &last);
        p = d;
    }
    (*phead) = head.next;
    if(ptail) (*ptail) = last;
    return p;
}

char *tok_list(char *p , Token *phead , Token *ptail){
    char hp = *p;
    if(!is_paren_l(*p))
        error("unmatched list %s\n" , p);
    p += 1;
    Token head , tail;
    head = tail = new_token(strdup("(") , NULL); //must use strdup
    while(1){
        p = get_non_blank(p);
        switch(*p){
            case ';':
                p = strchr(p , '\0') - 1;
                break;
            case '(':
            case '[':
            case '{':
                p = tok_list(p , &tail->next , &tail) - 1;
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
                p = tok_string(p , &tail->next , &tail) - 1;
                break;
            case '\'':
            default:
                p = tok_atom(p , &tail->next , &tail) - 1;
                break;
        }
        p += 1;
    }
}

void print_token(Token tok){
    if(!tok) return;
    printf("%s %s" , tok->p , tok->next ? "," : " \n");
    tok = tok->next;
    for( ; tok ; tok = tok->next)
        printf(" %s %s" , tok->p , tok->next ? "," : " \n");
}

char *tokenize(char *p , Token *tok){
    if(!*p) p = input("> " , false);
    Token head = NULL , t;
    while(*p && is_blank(*p)) p += 1;
    if(*p == '(') p = tok_list(p , &head , NULL);
    else if(*p) p = tok_atom(p , &head , NULL);
    (*tok) = head;
    return p;
}
