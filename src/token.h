#ifndef TOKEN_H
#define TOKEN_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "type.h"

#define SIZE 300

#define DESIRE_STR_PROMPT "___ "
#define DESIRE_PAR_PROMPT "... "

typedef struct token_tag *Token;
typedef struct token_tag token_t;

struct token_tag{
    char *p;
    Token next;
};

char *tokenize(char * , Token *);
char *input(const char *prompt , bool lock);

extern FILE *stream;
extern char *ctx_p;

bool is_blank(char p);
void print_token(Token tok);
void clear_buffer(void);
char *get_non_blank(char *p);
#endif
