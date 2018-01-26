#ifndef TOKEN_H
#define TOKEN_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "type.h"

#define SIZE 300

#define DESIRE_STR_PROMPT "___ "
#define DESIRE_PAR_PROMPT "... "
#define DESIRE_CMT_PROMPT "#.. "
#define DESIRE_QUO_PROMPT "'.. "

#define mulcmt_beg "#|"
#define mulcmt_end "|#"

typedef struct token_tag *Token;
typedef struct token_tag token_t;

struct token_tag{
    char *p;
    Token next;
};

char *tokenize(char * , Token *);

bool is_blank(char p);
void print_token(Token tok);
void clear_buffer(void);

extern char *(*tok_raw_input)(char *p);
extern char *(*tok_non_blank)(char *p , char *prompt);
#endif
