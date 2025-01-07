#pragma once

#include <stddef.h>

typedef struct Lexer {
    char *source;
    char curr_char;
    size_t curr_pos;
} Lexer;

typedef enum TokenType {
    TOKEN_EOF = -1,
    TOKEN_NEWLINE = 0,
    TOKEN_NUMBER = 1,
    TOKEN_IDENT = 2,
    TOKEN_STRING = 3,
    // Keywords
    TOKEN_LABEL = 101,
    TOKEN_GOTO = 102,
    TOKEN_PRINT = 103,
    TOKEN_INPUT = 104,
    TOKEN_LET = 105,
    TOKEN_IF = 106,
    TOKEN_THEN = 107,
    TOKEN_ENDIF = 108,
    TOKEN_WHILE = 109,
    TOKEN_REPEAT = 110,
    TOKEN_ENDWHILE = 111,
    // Operators
    TOKEN_EQ = 201,
    TOKEN_PLUS = 202,
    TOKEN_MINUS = 203,
    TOKEN_ASTERISK = 204,
    TOKEN_SLASH = 205,
    TOKEN_EQEQ = 206,
    TOKEN_NOTEQ = 207,
    TOKEN_LT = 208,
    TOKEN_LTEQ = 209,
    TOKEN_GT = 210,
    TOKEN_GTEQ = 211,
} TokenType;

typedef struct Token {
    TokenType kind;
    char *text_start;
    size_t text_len;
} Token;

Lexer lexer_new(char *source);

Token lexer_get_token(Lexer *lexer);
