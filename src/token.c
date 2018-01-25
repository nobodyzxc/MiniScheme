#include "token.h"
#include "util.h"
#include "mem.h"

#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <readline/readline.h>
#include <readline/history.h>

#define max(a , b) (a > b ? a : b)

char *buffer = NULL;
char *ctx_p = NULL;

char *tok_list(char *p , Token *phead , Token *ptail);

void clear_buffer(void){
    free(buffer);
    buffer = NULL;
    ctx_p = NULL;
}

char *input(const char *prompt , bool lock){

    free(buffer);

    rl_instream = stream;

    FILE *prev_rl_outstream = rl_outstream;

    rl_outstream = stream == stdin ? stdout : fopen("/dev/null", "w");


    if(lock)
        while(!(buffer = readline(prompt)));
    else
        if(!(buffer = readline(prompt))) return NULL;

    if(stream == stdin) add_history(buffer);
    if(stream != stdin) fclose(rl_outstream);

    rl_outstream = prev_rl_outstream;

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
char *ignore_comnt(char *p){
    assert(p);
    if(!is_multi_comnt(p))
        error("unexpected token #*");
    p += 2;
    while(1){
        while(!(p = strstr(p , mulcmt_end)))
            p = get_non_blank(p);
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

char *get_non_blank(char *p){
    while(p && *p && is_blank(*p)) p++;
    while(!p || !*p){
        p = input(p ? DESIRE_PAR_PROMPT : "" , true);
        while(*p && is_blank(*p)) p++;
    }
    return p;
}

char *add_quote(char *p , Token *plast){
    if(*p != '\'')
        error("invalid call add_quote %s\n" , p);
    else
        p += 1;

    p = get_non_blank(p);

    add_token(strdup("(") , plast);
    add_token(strdup("quote") , plast);
    if(*p == '(')
        p = tok_list(p , &(*plast)->next , plast);
    else if(*p == '\'')
        p = add_quote(p , plast);
    else{
        char *s = p;
        add_token(ya_strndup(s , (p = tokstr(p)) - s) , plast);
    }
    add_token(strdup(")") , plast);
    return p;
}

char *salloc(char *str , int size){
    char *re = malloc(sizeof(char) * size);
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
    char *q = p , *str = salloc(NULL , SIZE);
    while(!even_backslash(p , (q = strchr(q + 1 , '"'))));
    if(q) add_token(ya_strndup(p , q - p + 1) , ptail) , p = q + 1;
    else{
        int len = SIZE , l = strlen(p) + 1;
        /* + 1 for \n */
        if(l > len){
            str = salloc(str , max(len , l) + 100);
            len = max(len , l) + 100;
        }
        sprintf(str , "%s\n" , p);
        while(1){
            if(str[l - 1] == '\n')
                stdin_printf(DESIRE_STR_PROMPT);
            if(l > len){
                str = salloc(str , l + 100);
                len = l + 100;
            }
            str[l] = read_char();
            if(str[l] == '"' && str[l - 1] != '\\'){
                str[l + 1] = 0;
                add_token(str , ptail);
                clear_buffer();
                buffer = salloc(NULL , 300);
                fgets(buffer , sizeof(char) * 300 , stream);
                buffer[strlen(buffer) - 1] = 0;
                return buffer;
            }
            l++;
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
    head = tail = new_token(strdup("(") , NULL); //must use strdup
    p += 1;
    while(1){
        p = get_non_blank(p);
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
                    add_token(strdup(")") , &tail);
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
    if(is_paren_l(*p))
        p = tok_list(p , &head , NULL);
    else if(*p)
        p = tok_atom(p , &head , NULL);
    if(p == NULL) free_token(head) , head = NULL;
    (*tok) = head;
    return p;
}
