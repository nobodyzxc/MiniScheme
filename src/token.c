#include "token.h"
#include "util.h"
#include "mem.h"

#include <string.h>
#include <stdbool.h>
#include <assert.h>

char glo_buffer[SIZE];
char *glo_bufptr;

char *tok_list(char *buffer , char *p , Token *phead , Token *ptail);

void clear_buf(void){
    memset(glo_buffer , 0 , sizeof(glo_buffer));
}

char *input(char *buffer , const char *prompt , bool lock){
    stdin_printf(prompt);
    if(lock)
        while(!fgets(buffer , SIZE , stream));
    else
        if(!fgets(buffer , SIZE , stream)) return NULL;
    buffer[strlen(buffer) - 1] = 0;
    return buffer;
}

char read_char(){
    return getc(stream);
}
bool is_blank(char p){
    return p && strchr(" \n\r\t" , p) != NULL;
}

bool is_paren(char p){
    return p && strchr("()[]{}" , p) != NULL;
}

bool is_paren_l(char p){
    return p && strchr("([{" , p) != NULL;
}

bool is_paren_r(char p){
    return p && strchr(")]}" , p) != NULL;
}

char rev_paren(char p){
    char *paren = (char*)"([{)]}";
    char *v = strchr(paren , p);
    if(v && *v) return *(v + ((v > paren + 2) ? -3 : 3));
    else return 0;
}

bool is_comnt(char p){
    char *c = (char*)";";
    for(int i = 0 ; i < strlen(c) ; i++)
        if(p == c[i]) return true;
    return false;
}

bool is_tokch(char ch){
    return (ch == '\0'
            || is_blank(ch)
            || is_paren(ch)
            || is_comnt(ch)
            || ch == '"');
}

#define mulcmt_beg "#|"
#define mulcmt_end "|#"

bool is_multi_comnt(char *p){
    for(int i = 0 ; i < strlen(mulcmt_beg) ; i++)
        if(p[i] != mulcmt_beg[i]) return false;
    return is_tokch(*(p + strlen(mulcmt_beg)));
}

/* Multi-line Comments #| |# */
char *ignore_comnt(char *buffer , char *p){
    assert(p);
    if(!is_multi_comnt(p))
        error("unexpected token #*");
    p += 2;
    while(1){
        while(!(p = strstr(p , mulcmt_end)))
            p = get_non_blank(buffer , p);
        if(is_tokch(*(p + strlen(mulcmt_end)))) break;
    }
    return p + strlen(mulcmt_end);
}

char *tokstr(char *p){
    while(p && !is_tokch(*p))
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

char *get_non_blank(char* buffer , char *p){
    while(p && *p && is_blank(*p)) p++;
    while(!p || !*p){
        p = input(buffer , p ? "... " : "" , true);
        while(*p && is_blank(*p)) p++;
    }
    return p;
}

char *add_quote(char* buffer , char *p , Token *plast){
    if(*p != '\'')
        error("unmatched quote %s\n" , p);
    else
        p += 1;

    p = get_non_blank(buffer , p);

    add_token(strdup("(") , plast);
    add_token(strdup("quote") , plast);
    if(*p == '(' || *p == '\''){
        if(*p == '(')
            p = tok_list(buffer , p , &(*plast)->next , plast);
        else
            p = add_quote(buffer , p , plast);
    }
    else{
        char *s = p;
        add_token(ya_strndup(s , (p = tokstr(p)) - s) , plast);
    }
    add_token(strdup(")") , plast);
    return p;
}

char *tok_string(char* buffer , char *p , Token *phead , Token *ptail){
    if(*p != '"')
        error("parse string start with %c\n" , *p);
    char *q = p , buf[SIZE];
    while((q = strchr(q + 1 , '"')) && *(q - 1) == '\\');
    if(q) add_token(ya_strndup(p , q - p + 1) , ptail) , p = q + 1;
    else{
        int l = strlen(p) + 1;
        if(l > SIZE)
            error("exceed string limit\n");
        sprintf(buf , "%s\n" , p);
        while(1){
            if(buf[l - 1] == '\n')
                stdin_printf("... ");
            if(l > SIZE)
                error("exceed string limit : " xstr(SIZE) "\n");
            else
                buf[l] = read_char();
            if(buf[l] == '"' && buf[l - 1] != '\\'){
                add_token(ya_strndup(buf , l + 1) , ptail);
                return input(buffer , "" , true);
            }
            l++;
        }
    }
    return p;
}

char *tok_atom(char* buffer , char *p , Token *phead , Token *ptail){
    if(!p) return p;
    while(*p && is_blank(*p)) p++;
    token_t head
        = {.p = NULL , .next = NULL};
    Token last = &head;
    if(is_paren_r(*p))
        error("unmatched paren while parsing atom\n");
    if(*p == '\'')
        p = add_quote(buffer , p , &last);
    else if(*p == ';')
        return strchr(p , '\0');
    else if(*p == '"')
        p = tok_string(buffer , p , &head.next , &last);
    else if(is_multi_comnt(p))
        p = ignore_comnt(buffer , p);
    else{
        char *d = tokstr(p);
        add_token(ya_strndup(p , d - p) , &last);
        p = d;
    }
    (*phead) = head.next;
    if(ptail) (*ptail) = last;
    return p;
}

char *tok_list(char* buffer , char *p , Token *phead , Token *ptail){
    char hp = *p;
    if(!is_paren_l(*p))
        error("unmatched list %s\n" , p);
    p += 1;
    Token head , tail;
    head = tail = new_token(strdup("(") , NULL); //must use strdup
    while(1){
        p = get_non_blank(buffer , p);
        switch(*p){
            case '(':
            case '[':
            case '{':
                p = tok_list(buffer , p , &tail->next , &tail) - 1;
                break;
            case ')':
            case ']':
            case '}':
                if(*p != rev_paren(hp))
                    error("unmatched paren %c , %c\n" , hp , *p);
                add_token(ya_strndup(p , 1) , &tail);
                (*phead) = head;
                if(ptail) (*ptail) = tail;
                return p + 1;
            default:
                p = tok_atom(buffer , p , &tail->next , &tail) - 1;
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

char *tokenize(char *buffer , char *p , Token *tok){
    if(!*p) p = input(buffer , "> " , false);
    Token head = NULL , t;
    while(*p && is_blank(*p)) p += 1;
    if(*p == '(') p = tok_list(buffer , p , &head , NULL);
    else if(*p) p = tok_atom(buffer , p , &head , NULL);
    (*tok) = head;
    return p;
}
