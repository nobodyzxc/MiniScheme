#ifndef TOKEN_H
#define TOKEN_H
typedef struct token_tag *token;
typedef struct token_tag token_t;

struct token_tag{
    char *p;
    token next;
};

char *parse_list(char *p , token *phead);
void tokenize(FILE *);
#endif
