#ifndef TOKEN_H
#define TOKEN_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "type.h"

#define SIZE 300

typedef struct token_tag *Token;
typedef struct token_tag token_t;

struct token_tag{
    char *p;
    Token next;
};

char *tokenize(char* , char * , Token *);
char *input(char* , const char *prompt , bool lock);

extern FILE *stream;
extern char glo_buffer[SIZE];
extern char *glo_bufptr;

bool is_blank(char p);
void print_token(Token tok);
void clear_buf(void);
char *get_non_blank(char* buffer , char *p);
#endif
