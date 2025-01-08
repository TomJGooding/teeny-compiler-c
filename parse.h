#pragma once

#include "emit.h"
#include "lex.h"

#define TOKEN_SET_CAPACITY 256

// In reality this should be a dynamic hash table!
typedef struct TokenSet {
    Token tokens[TOKEN_SET_CAPACITY];
    size_t len;
} TokenSet;

typedef struct Parser {
    Lexer *lexer;
    Emitter *emitter;

    TokenSet symbols;
    TokenSet labels_declared;
    TokenSet labels_gotoed;

    Token curr_token;
    Token peek_token;
} Parser;

Parser parser_new(Lexer *lexer, Emitter *emitter);

void parser_program(Parser *parser);
