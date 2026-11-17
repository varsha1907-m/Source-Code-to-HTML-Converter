
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

/* FSM states */
typedef enum
{
    FSM_START,
    FSM_IDENTIFIER,
    FSM_NUMBER,
    FSM_STRING,
    FSM_CHARACTER,
    FSM_LINE_COMMENT,
    FSM_BLOCK_COMMENT,
    FSM_PREPROCESSOR,
    FSM_HEADER,
    FSM_OPERATOR
} FSMState;

static Token token;
static int line_start = 1;
static int include_pending = 0;

/* Data type keywords */
static const char *data_keywords[] =
{
    "int", "char", "float", "double", "void",
    "short", "long", "signed", "unsigned",
    "const", "volatile", "static", "extern",
    "auto", "register", "struct", "union",
    "enum", "typedef", "_Bool", NULL
};

/* Other C keywords */
static const char *other_keywords[] =
{
    "if", "else", "for", "while", "do",
    "switch", "case", "default", "break",
    "continue", "return", "goto", "sizeof",
    "inline", "restrict", NULL
};

/* Add character to token */
static void add_char(int ch)
{
    int len = strlen(token.data);

    if(len < TOKEN_SIZE - 1)
    {
        token.data[len] = (char)ch;
        token.data[len + 1] = '\0';
    }
}

/* Finish token */
static Token *finish_token(TokenType type, int property)
{
    token.type = type;
    token.property = property;

    return &token;
}

/* Check identifier characters */
static int is_word_char(int ch)
{
    return isalnum((unsigned char)ch) || ch == '_';
}

/* Check C keywords */
static int check_keyword(const char *word)
{
    int i;

    for(i = 0; data_keywords[i] != NULL; i++)
    {
        if(strcmp(word, data_keywords[i]) == 0)
            return DATA_KEYWORD;
    }

    for(i = 0; other_keywords[i] != NULL; i++)
    {
        if(strcmp(word, other_keywords[i]) == 0)
            return OTHER_KEYWORD;
    }

    return 0;
}

/* Read identifier or keyword */
static Token *read_identifier(FILE *fp, int first)
{
    FSMState state = FSM_IDENTIFIER;
    char word[TOKEN_SIZE];
    int i = 0;
    int ch;

    add_char(first);
    word[i++] = (char)first;

    while(state == FSM_IDENTIFIER)
    {
        ch = fgetc(fp);

        if(ch != EOF && is_word_char(ch))
        {
            add_char(ch);

            if(i < TOKEN_SIZE - 1)
                word[i++] = (char)ch;
        }
        else
        {
            if(ch != EOF)
                ungetc(ch, fp);

            state = FSM_START;
        }
    }

    word[i] = '\0';

    return finish_token(TOKEN_KEYWORD, check_keyword(word));
}

/* Read numeric constant */
static Token *read_number(FILE *fp, int first)
{
    FSMState state = FSM_NUMBER;
    int ch;
    int previous = first;

    add_char(first);

    while(state == FSM_NUMBER)
    {
        ch = fgetc(fp);

        if(ch != EOF &&
           (isalnum((unsigned char)ch) || ch == '.' || ch == '_'))
        {
            add_char(ch);
            previous = ch;
        }
        else if(ch != EOF &&
                (ch == '+' || ch == '-') &&
                (previous == 'e' || previous == 'E' ||
                 previous == 'p' || previous == 'P'))
        {
            add_char(ch);
            previous = ch;
        }
        else
        {
            if(ch != EOF)
                ungetc(ch, fp);

            state = FSM_START;
        }
    }

    return finish_token(TOKEN_NUMBER, 0);
}

/* Read string or character literal */
static Token *read_quoted(FILE *fp, int quote, FSMState state)
{
    int ch;

    add_char(quote);

    while(state == FSM_STRING || state == FSM_CHARACTER)
    {
        ch = fgetc(fp);

        if(ch == EOF)
            break;

        add_char(ch);

        if(ch == '\\')
        {
            ch = fgetc(fp);

            if(ch == EOF)
                break;

            add_char(ch);
        }
        else if(ch == quote)
        {
            break;
        }
    }

    if(quote == '"')
        return finish_token(TOKEN_STRING, 0);

    return finish_token(TOKEN_CHARACTER, 0);
}

/* Read header file */
static Token *read_header(FILE *fp, int opening)
{
    FSMState state = FSM_HEADER;
    int closing = (opening == '<') ? '>' : '"';
    int ch;

    add_char(opening);

    while(state == FSM_HEADER)
    {
        ch = fgetc(fp);

        if(ch == EOF)
            break;

        add_char(ch);

        if(ch == closing)
            state = FSM_START;
    }

    return finish_token(TOKEN_HEADER,
                        opening == '<' ? SYSTEM_HEADER : USER_HEADER);
}

/* Read preprocessor directive */
static Token *read_preprocessor(FILE *fp)
{
    FSMState state = FSM_PREPROCESSOR;
    char word[TOKEN_SIZE];
    int ch;
    int i = 0;

    add_char('#');

    while(state == FSM_PREPROCESSOR)
    {
        ch = fgetc(fp);

        if(ch == EOF)
            break;

        if(isspace((unsigned char)ch))
        {
            if(ch == '\n')
            {
                ungetc(ch, fp);
                break;
            }

            add_char(ch);
            continue;
        }

        if(isalpha((unsigned char)ch))
        {
            do
            {
                add_char(ch);

                if(i < TOKEN_SIZE - 1)
                    word[i++] = (char)ch;

                ch = fgetc(fp);
            }
            while(ch != EOF && is_word_char(ch));

            if(ch != EOF)
                ungetc(ch, fp);
        }

        break;
    }

    word[i] = '\0';

    include_pending = (strcmp(word, "include") == 0);

    return finish_token(TOKEN_PREPROCESSOR, 0);
}

/* Read comments */
static Token *read_comment(FILE *fp, int second)
{
    FSMState state;
    int ch;
    int previous = 0;

    add_char('/');
    add_char(second);

    if(second == '/')
        state = FSM_LINE_COMMENT;
    else
        state = FSM_BLOCK_COMMENT;

    while(state == FSM_LINE_COMMENT || state == FSM_BLOCK_COMMENT)
    {
        ch = fgetc(fp);

        if(ch == EOF)
            break;

        add_char(ch);

        if(ch == '\n')
            line_start = 1;

        if(state == FSM_LINE_COMMENT && ch == '\n')
        {
            state = FSM_START;
        }
        else if(state == FSM_BLOCK_COMMENT &&
                previous == '*' && ch == '/')
        {
            state = FSM_START;
        }

        previous = ch;
    }

    return finish_token(TOKEN_COMMENT, 0);
}

/* Read operators */
static Token *read_operator(FILE *fp, int first)
{
    int next;
    int third;

    add_char(first);

    next = fgetc(fp);

    if(next == EOF)
        return finish_token(TOKEN_OPERATOR, 0);

    /* Three-character shift operators */
    if((first == '<' && next == '<') ||
       (first == '>' && next == '>'))
    {
        third = fgetc(fp);

        if(third == '=')
        {
            add_char(next);
            add_char(third);
        }
        else
        {
            if(third != EOF)
                ungetc(third, fp);

            add_char(next);
        }

        return finish_token(TOKEN_OPERATOR, 0);
    }

    /* Two-character operators */
    if((first == '+' && (next == '+' || next == '=')) ||
       (first == '-' && (next == '-' || next == '=' || next == '>')) ||
       (first == '*' && next == '=') ||
       (first == '/' && next == '=') ||
       (first == '%' && next == '=') ||
       (first == '=' && next == '=') ||
       (first == '!' && next == '=') ||
       (first == '<' && next == '=') ||
       (first == '>' && next == '=') ||
       (first == '&' && (next == '&' || next == '=')) ||
       (first == '|' && (next == '|' || next == '=')) ||
       (first == '^' && next == '='))
    {
        add_char(next);
    }
    else
    {
        ungetc(next, fp);
    }

    return finish_token(TOKEN_OPERATOR, 0);
}

/* Main FSM function */
Token *get_next_token(FILE *fp)
{
    FSMState state = FSM_START;
    int ch;
    int next;

    token.data[0] = '\0';
    token.property = 0;

    ch = fgetc(fp);

    if(ch == EOF)
        return finish_token(TOKEN_EOF, 0);

    /* Identify header file */
    if(include_pending && (ch == '<' || ch == '"'))
    {
        include_pending = 0;
        line_start = 0;

        return read_header(fp, ch);
    }

    /* Identify preprocessor directive */
    if(ch == '#' && line_start)
    {
        line_start = 0;
        return read_preprocessor(fp);
    }

    /* Identify whitespace */
    if(isspace((unsigned char)ch))
    {
        add_char(ch);

        if(ch == '\n')
            line_start = 1;

        return finish_token(TOKEN_NORMAL, 0);
    }

    line_start = 0;

    /* Identify starting FSM state */
    if(isalpha((unsigned char)ch) || ch == '_')
        state = FSM_IDENTIFIER;

    else if(isdigit((unsigned char)ch))
        state = FSM_NUMBER;

    else if(ch == '"')
        state = FSM_STRING;

    else if(ch == '\'')
        state = FSM_CHARACTER;

    else if(ch == '/')
    {
        next = fgetc(fp);

        if(next == '/' || next == '*')
            return read_comment(fp, next);

        if(next != EOF)
            ungetc(next, fp);

        state = FSM_OPERATOR;
    }
    else if(strchr("+-*%=<>!&|^~", ch) != NULL)
        state = FSM_OPERATOR;

    /* FSM transitions */
    switch(state)
    {
        case FSM_IDENTIFIER:
            return read_identifier(fp, ch);

        case FSM_NUMBER:
            return read_number(fp, ch);

        case FSM_STRING:
            return read_quoted(fp, ch, FSM_STRING);

        case FSM_CHARACTER:
            return read_quoted(fp, ch, FSM_CHARACTER);

        case FSM_OPERATOR:
            return read_operator(fp, ch);

        default:
            add_char(ch);
            return finish_token(TOKEN_NORMAL, 0);
    }
}
