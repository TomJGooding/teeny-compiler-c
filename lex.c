#include "lex.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lexer_next_char(Lexer *lexer) {
    lexer->curr_pos++;
    if (lexer->curr_pos >= strlen(lexer->source)) {
        lexer->curr_char = '\0';
    } else {
        lexer->curr_char = lexer->source[lexer->curr_pos];
    }
}

Lexer lexer_new(char *source) {
    Lexer lexer = {.source = source, .curr_pos = -1};
    lexer_next_char(&lexer);

    return lexer;
}

char lexer_peek(Lexer *lexer) {
    if (lexer->curr_pos + 1 >= strlen(lexer->source)) {
        return '\0';
    }
    return lexer->source[lexer->curr_pos + 1];
}

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

void lexer_skip_whitespace(Lexer *lexer) {
    while (is_whitespace(lexer->curr_char)) {
        lexer_next_char(lexer);
    }
}

void lexer_skip_comment(Lexer *lexer) {
    if (lexer->curr_char == '#') {
        while (lexer->curr_char != '\n' && lexer->curr_char != '\0') {
            lexer_next_char(lexer);
        }
    }
}

const char *token_keywords[] = {
    "LABEL",
    "GOTO",
    "PRINT",
    "INPUT",
    "LET",
    "IF",
    "THEN",
    "ENDIF",
    "WHILE",
    "REPEAT",
    "ENDWHILE"
};

TokenType check_if_keyword(char *text_start, size_t text_len) {
    size_t keywords_count = sizeof(token_keywords) / sizeof(token_keywords[0]);
    for (size_t i = 0; i < keywords_count; i++) {
        if (strncmp(text_start, token_keywords[i], text_len) == 0) {
            return (TokenType)(TOKEN_LABEL + i);
        }
    }
    return TOKEN_IDENT;
}

bool is_illegal_string_char(char c) {
    return c == '\r' || c == '\n' || c == '\t' || c == '\\' || c == '\%';
}

Token lexer_get_token(Lexer *lexer) {
    lexer_skip_whitespace(lexer);
    lexer_skip_comment(lexer);

    Token token = {
        .kind = TOKEN_EOF,
        .text_start = lexer->source + lexer->curr_pos,
        .text_len = 1
    };
    if (lexer->curr_char == '+') {
        token.kind = TOKEN_PLUS;

    } else if (lexer->curr_char == '-') {
        token.kind = TOKEN_MINUS;

    } else if (lexer->curr_char == '*') {
        token.kind = TOKEN_ASTERISK;

    } else if (lexer->curr_char == '/') {
        token.kind = TOKEN_SLASH;

    } else if (lexer->curr_char == '=') {
        if (lexer_peek(lexer) == '=') {
            lexer_next_char(lexer);
            token.text_len = 2;
            token.kind = TOKEN_EQEQ;
        } else {
            token.kind = TOKEN_EQ;
        }

    } else if (lexer->curr_char == '>') {
        if (lexer_peek(lexer) == '=') {
            lexer_next_char(lexer);
            token.text_len = 2;
            token.kind = TOKEN_GTEQ;
        } else {
            token.kind = TOKEN_GT;
        }

    } else if (lexer->curr_char == '<') {
        if (lexer_peek(lexer) == '=') {
            lexer_next_char(lexer);
            token.text_len = 2;
            token.kind = TOKEN_LTEQ;
        } else {
            token.kind = TOKEN_LT;
        }

    } else if (lexer->curr_char == '!') {
        if (lexer_peek(lexer) == '=') {
            lexer_next_char(lexer);
            token.text_len = 2;
            token.kind = TOKEN_NOTEQ;
        } else {
            fprintf(stderr, "Lexing error: ");
            fprintf(stderr, "Expected !=, got !%c\n", lexer_peek(lexer));
            exit(EXIT_FAILURE);
        }

    } else if (lexer->curr_char == '\"') {
        lexer_next_char(lexer);
        size_t start_pos = lexer->curr_pos;
        while (lexer->curr_char != '\"') {
            if (is_illegal_string_char(lexer->curr_char)) {
                fprintf(stderr, "Lexing error: ");
                fprintf(stderr, "Illegal character in string\n");
                exit(EXIT_FAILURE);
            }

            lexer_next_char(lexer);
        }
        token.text_start = lexer->source + start_pos;
        token.text_len = lexer->curr_pos - start_pos;
        token.kind = TOKEN_STRING;

    } else if (isdigit(lexer->curr_char)) {
        size_t start_pos = lexer->curr_pos;
        while (isdigit(lexer_peek(lexer))) {
            lexer_next_char(lexer);
        }
        if (lexer_peek(lexer) == '.') {
            lexer_next_char(lexer);
            if (!isdigit(lexer_peek(lexer))) {
                fprintf(stderr, "Lexing error: ");
                fprintf(stderr, "Illegal character in number\n");
                exit(EXIT_FAILURE);
            }
            while (isdigit(lexer_peek(lexer))) {
                lexer_next_char(lexer);
            }
        }
        token.text_len = lexer->curr_pos + 1 - start_pos;
        token.kind = TOKEN_NUMBER;

    } else if (isalpha(lexer->curr_char)) {
        size_t start_pos = lexer->curr_pos;
        while (isalnum(lexer_peek(lexer))) {
            lexer_next_char(lexer);
        }
        token.text_len = lexer->curr_pos + 1 - start_pos;
        token.kind = check_if_keyword(token.text_start, token.text_len);

    } else if (lexer->curr_char == '\n') {
        token.kind = TOKEN_NEWLINE;

    } else if (lexer->curr_char == '\0') {
        token.kind = TOKEN_EOF;

    } else {
        fprintf(stderr, "Lexing error: ");
        fprintf(stderr, "Unknown token: %c\n", lexer->curr_char);
        exit(EXIT_FAILURE);
    }

    lexer_next_char(lexer);

    return token;
}

void token_print(Token *token) {
    printf("%.*s: ", (int)token->text_len, token->text_start);
    switch (token->kind) {
        case TOKEN_EOF:
            printf("TOKEN_EOF\n");
            break;
        case TOKEN_NEWLINE:
            printf("TOKEN_NEWLINE\n");
            break;
        case TOKEN_NUMBER:
            printf("TOKEN_NUMBER\n");
            break;
        case TOKEN_IDENT:
            printf("TOKEN_IDENT\n");
            break;
        case TOKEN_STRING:
            printf("TOKEN_STRING");
            break;
        case TOKEN_LABEL:
            printf("TOKEN_LABEL\n");
            break;
        case TOKEN_GOTO:
            printf("TOKEN_GOTO\n");
            break;
        case TOKEN_PRINT:
            printf("TOKEN_PRINT\n");
            break;
        case TOKEN_INPUT:
            printf("TOKEN_INPUT\n");
            break;
        case TOKEN_LET:
            printf("TOKEN_LET\n");
            break;
        case TOKEN_IF:
            printf("TOKEN_IF\n");
            break;
        case TOKEN_THEN:
            printf("TOKEN_THEN\n");
            break;
        case TOKEN_ENDIF:
            printf("TOKEN_ENDIF\n");
            break;
        case TOKEN_WHILE:
            printf("TOKEN_WHILE\n");
            break;
        case TOKEN_REPEAT:
            printf("TOKEN_REPEAT\n");
            break;
        case TOKEN_ENDWHILE:
            printf("TOKEN_ENDWHILE\n");
            break;
        case TOKEN_EQ:
            printf("TOKEN_EQ\n");
            break;
        case TOKEN_PLUS:
            printf("TOKEN_PLUS\n");
            break;
        case TOKEN_MINUS:
            printf("TOKEN_MINUS\n");
            break;
        case TOKEN_ASTERISK:
            printf("TOKEN_ASTERISK\n");
            break;
        case TOKEN_SLASH:
            printf("TOKEN_SLASH\n");
            break;
        case TOKEN_EQEQ:
            printf("TOKEN_EQEQ\n");
            break;
        case TOKEN_NOTEQ:
            printf("TOKEN_NOTEQ\n");
            break;
        case TOKEN_LT:
            printf("TOKEN_LT\n");
            break;
        case TOKEN_LTEQ:
            printf("TOKEN_LTEQ\n");
            break;
        case TOKEN_GT:
            printf("TOKEN_GT\n");
            break;
        case TOKEN_GTEQ:
            printf("TOKEN_GTEQ\n");
            break;
    };
}

// int main() {
//     char *source = "+-123 9.8654*/";
//     Lexer lexer = lexer_new(source);
//
//     Token token = lexer_get_token(&lexer);
//     while (token.kind != TOKEN_EOF) {
//         token_print(&token);
//         token = lexer_get_token(&lexer);
//     }
// }
