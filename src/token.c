#include "token.h"
#include "util.h"
#include "memory.h"

#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define max(a , b) (a > b ? a : b)

char *(*tok_raw_input)(char *p) = NULL;
char *(*tok_non_blank)(char *p , char *prompt) = NULL;

char *tok_list(char *p , Token *phead , Token *ptail);

char *err_tok(char *msg){
    puts(msg);
    return NULL;
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

bool is_multi_comnt(char *p){
    for(int i = 0 ; i < strlen(mulcmt_beg) ; i++)
        if(p[i] != mulcmt_beg[i]) return false;
    return true;
    //return is_tokch(*(p + strlen(mulcmt_beg)));
}

/* Multi-line Comments #| |# */
char *ignore_comnt(char *p){
    assert(p);
    if(!is_multi_comnt(p))
        error("unexpected token #*");
    p += 2;
    while(1){
        while(p && !(p = strstr(p , mulcmt_end)))
            p = tok_non_blank(p , DESIRE_CMT_PROMPT);
        if(p == NULL)
            return err_tok("unexpected EOF instead of " mulcmt_end);
        if(is_tokch(*(p + strlen(mulcmt_end)))) break;
        else p += strlen(mulcmt_end) + 1;
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

char *add_quote(char *p , Token *plast){
    if(*p != '\'')
        error("invalid call add_quote %s\n" , p);
    else
        p += 1;

    p = tok_non_blank(p , DESIRE_QUO_PROMPT);
    if(p == NULL)
        return err_tok("unexpected EOF instead of other elt");

    add_token(ya_strdup("(") , plast);
    add_token(ya_strdup("quote") , plast);
    if(*p == '(')
        p = tok_list(p , &(*plast)->next , plast);
    else if(*p == '\'')
        p = add_quote(p , plast);
    else{
        char *s = p;
        add_token(ya_strndup(s , (p = tokstr(p)) - s) , plast);
    }
    add_token(ya_strdup(")") , plast);
    return p;
}

char *salloc(char *str , int size){
    char *re = MALLOC(sizeof(char) * size);
    sprintf(re , str ? str : "") , free(str);
    return re;
}

bool even_backslash(char* beg , char *end){
    if(!end) return true;
    int acc = 0;
    for(char *p = end - 1 ; p >= beg ; p--)
        if(*p == '\\') acc++;
        else break;
    return acc % 2 == 0;
}

char *tok_string(char *p , Token *phead , Token *ptail){
    if(*p != '"')
        error("invalid call tok_string %c\n" , *p);
    char *q = p;
    while(!even_backslash(p , (q = strchr(q + 1 , '"'))));
    if(q) add_token(ya_strndup(p , q - p + 1) , ptail) , p = q + 1;
    else{
        int l = strlen(p);
        int len = max(SIZE , l) + 100;
        char *str = salloc(NULL , len);
        sprintf(str , "%s" , p) , p = strchr(p , 0);
        while(1){
            if(l > len){
                len = l + 100;
                str = salloc(str , len);
            }
            if(!*p){
                p = tok_raw_input(DESIRE_STR_PROMPT);
                if(p == NULL)
                    return err_tok("unexpected EOF instead of \"");
                str[l] = '\n' , l++;
                continue;
            }
            str[l] = *p;
            if(str[l] == '"' && even_backslash(str , str + l)){
                str[l + 1] = 0;
                add_token(str , ptail);
                return p + 1;
            }
            l++ , p++;
        }
    }
    return p;
}

char *tok_atom(char *p , Token *phead , Token *ptail){
    if(!p) return p;
    while(*p && is_blank(*p)) p++;
    token_t head
        = {.p = NULL , .next = NULL};
    Token last = &head;
    if(is_paren_r(*p)){
        printf("unmatched paren while parsing atom\n");
        (*phead) = head.next;
        if(ptail) (*ptail) = last;
        return NULL;
    }
    if(*p == '\'')
        p = add_quote(p , &last);
    else if(*p == ';')
        return strchr(p , '\0');
    else if(*p == '"')
        p = tok_string(p , &head.next , &last);
    else if(is_multi_comnt(p))
        p = ignore_comnt(p);
    else{
        char *d = tokstr(p);
        add_token(ya_strndup(p , d - p) , &last);
        p = d;
    }
    (*phead) = head.next;
    if(ptail) (*ptail) = last;
    return p;
}

char *tok_list(char *p , Token *phead , Token *ptail){
    char hp = *p;
    Token head , tail;
    if(!is_paren_l(*p))
        error("invalid call tok_list %s\n" , p);
    head = tail = new_token(ya_strdup("(") , NULL); //must use ya_strdup
    p += 1;
    while(1){
        p = tok_non_blank(p , DESIRE_PAR_PROMPT);
        if(p == NULL)
            return err_tok("unexpected EOF instead of )");
        switch(*p){
            case '(':
            case '[':
            case '{':
                p = tok_list(p , &tail->next , &tail) - 1;
                break;
            case ')':
            case ']':
            case '}':
                if(*p == rev_paren(hp)){
                    add_token(ya_strdup(")") , &tail);
                    (*phead) = head;
                    if(ptail) (*ptail) = tail;
                    return p + 1;
                }
                else{
                    printf("unmatched paren %c , %c\n" , hp , *p);
                    (*phead) = head;
                    if(ptail) (*ptail) = tail;
                    return NULL;
                }
            default:
                p = tok_atom(p , &tail->next , &tail) - 1;
                break;
        }
        if(p == NULL)
            return NULL;
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
    assert(p), assert(*p);
    assert(tok_raw_input);
    assert(tok_non_blank);
    Token head = NULL;
    while(*p && is_blank(*p)) p++;
    if(is_paren_l(*p))
        p = tok_list(p , &head , NULL);
    else if(*p)
        p = tok_atom(p , &head , NULL);
    if(p == NULL) free_token(head) , head = NULL;
    (*tok) = head;
    return p;
}
