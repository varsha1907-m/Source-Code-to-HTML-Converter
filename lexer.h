
#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define TOKEN_SIZE 1024

typedef enum
{
    TOKEN_EOF,
    TOKEN_KEYWORD,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_CHARACTER,
    TOKEN_HEADER,
    TOKEN_PREPROCESSOR,
    TOKEN_COMMENT,
    TOKEN_OPERATOR,
    TOKEN_NORMAL
} TokenType;

typedef struct
{
    TokenType type;
    int property;
    char data[TOKEN_SIZE];
} Token;

#define DATA_KEYWORD  1
#define OTHER_KEYWORD 2
#define SYSTEM_HEADER 3
#define USER_HEADER   4

Token *get_next_token(FILE *fp);

#endif
