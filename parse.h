#pragma once

#include "lex.h"

typedef struct Parser {
    Lexer *lexer;
    Token curr_token;
    Token peek_token;
} Parser;

Parser parser_new(Lexer *lexer);
void parser_program(Parser *parser);
