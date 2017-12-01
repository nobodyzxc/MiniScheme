#ifndef TOKEN_H
#define TOKEN_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "type.h"
typedef struct token_tag *Token;
typedef struct token_tag token_t;

struct token_tag{
    char *p;
    Token next;
};

char *tokenize(char * , Token *);
char *input(const char *prompt , bool lock);

extern FILE *stream;
bool is_blank(char p);
void print_token(Token tok);
#endif
