#include "parse.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

void parser_next_token(Parser *parser) {
    parser->curr_token = parser->peek_token;
    parser->peek_token = lexer_get_token(parser->lexer);
}

Parser parser_new(Lexer *lexer) {
    Parser parser = {.lexer = lexer};
    // Call this twice to initialize current and peek.
    parser_next_token(&parser);
    parser_next_token(&parser);

    return parser;
}

bool token_set_contains(TokenSet *set, Token token) {
    for (size_t i = 0; i < set->len; i++) {
        if (strncmp(
                set->tokens[i].text_start, token.text_start, token.text_len
            ) == 0) {
            return true;
        }
    }
    return false;
}

bool token_set_insert(TokenSet *set, Token token) {
    if (token_set_contains(set, token)) {
        return false;
    }

    if (set->len >= TOKEN_SET_CAPACITY) {
        fprintf(stderr, "Error: Maximum tokens capacity exceeded\n");
        exit(EXIT_FAILURE);
    }

    set->tokens[set->len++] = token;

    return true;
}

bool parser_check_token(Parser *parser, TokenType kind) {
    return kind == parser->curr_token.kind;
};

bool parser_check_peek(Parser *parser, TokenType kind) {
    return kind == parser->peek_token.kind;
};

void parser_match(Parser *parser, TokenType kind) {
    if (!parser_check_token(parser, kind)) {
        fprintf(stderr, "Error: ");
        // TODO: Look into creating a function to print the TokenType enum names
        fprintf(stderr, "Expected %d, got %d\n", kind, parser->curr_token.kind);
        exit(EXIT_FAILURE);
    }
    parser_next_token(parser);
}

void parser_nl(Parser *parser) {
    printf("NEWLINE\n");
    parser_match(parser, TOKEN_NEWLINE);
    while (parser_check_token(parser, TOKEN_NEWLINE)) {
        parser_next_token(parser);
    }
}

void parser_primary(Parser *parser) {
    printf(
        "PRIMARY (%.*s)\n",
        (int)parser->curr_token.text_len,
        parser->curr_token.text_start
    );

    if (parser_check_token(parser, TOKEN_NUMBER)) {
        parser_next_token(parser);
    } else if (parser_check_token(parser, TOKEN_IDENT)) {
        if (!token_set_contains(&parser->symbols, parser->curr_token)) {
            fprintf(
                stderr,
                "Error: Referencing variable before assignment: %.*s\n",
                (int)parser->curr_token.text_len,
                parser->curr_token.text_start
            );
            exit(EXIT_FAILURE);
        }
        parser_next_token(parser);
    } else {
        fprintf(
            stderr,
            "Error: Unexpected token at %.*s\n",
            (int)parser->curr_token.text_len,
            parser->curr_token.text_start
        );
        exit(EXIT_FAILURE);
    }
}

void parser_unary(Parser *parser) {
    printf("UNARY\n");
    if (parser_check_token(parser, TOKEN_PLUS) ||
        parser_check_token(parser, TOKEN_MINUS)) {
        parser_next_token(parser);
    }
    parser_primary(parser);
}

void parser_term(Parser *parser) {
    printf("TERM\n");

    parser_unary(parser);
    while (parser_check_token(parser, TOKEN_ASTERISK) ||
           parser_check_token(parser, TOKEN_SLASH)) {
        parser_next_token(parser);
        parser_unary(parser);
    }
}

void parser_expression(Parser *parser) {
    printf("EXPRESSION\n");

    parser_term(parser);
    while (parser_check_token(parser, TOKEN_PLUS) ||
           parser_check_token(parser, TOKEN_MINUS)) {
        parser_next_token(parser);
        parser_term(parser);
    }
}

bool parser_is_comparison_operator(Parser *parser) {
    return parser_check_token(parser, TOKEN_GT) ||
           parser_check_token(parser, TOKEN_GTEQ) ||
           parser_check_token(parser, TOKEN_LT) ||
           parser_check_token(parser, TOKEN_LTEQ) ||
           parser_check_token(parser, TOKEN_EQEQ) ||
           parser_check_token(parser, TOKEN_NOTEQ);
}

void parser_comparison(Parser *parser) {
    printf("COMPARISON\n");

    parser_expression(parser);
    if (parser_is_comparison_operator(parser)) {
        parser_next_token(parser);
        parser_expression(parser);
    } else {
        fprintf(
            stderr,
            "Error: Expected comparison operator at: %.*s\n",
            (int)parser->curr_token.text_len,
            parser->curr_token.text_start
        );
        exit(EXIT_FAILURE);
    }

    while (parser_is_comparison_operator(parser)) {
        parser_next_token(parser);
        parser_expression(parser);
    }
}

void parser_statement(Parser *parser) {
    if (parser_check_token(parser, TOKEN_PRINT)) {
        printf("STATEMENT-PRINT\n");
        parser_next_token(parser);

        if (parser_check_token(parser, TOKEN_STRING)) {
            parser_next_token(parser);
        } else {
            parser_expression(parser);
        }

    } else if (parser_check_token(parser, TOKEN_IF)) {
        printf("STATEMENT-IF\n");
        parser_next_token(parser);
        parser_comparison(parser);

        parser_match(parser, TOKEN_THEN);
        parser_nl(parser);

        while (!parser_check_token(parser, TOKEN_ENDIF)) {
            parser_statement(parser);
        }
        parser_match(parser, TOKEN_ENDIF);

    } else if (parser_check_token(parser, TOKEN_WHILE)) {
        printf("STATEMENT-WHILE\n");
        parser_next_token(parser);
        parser_comparison(parser);

        parser_match(parser, TOKEN_REPEAT);
        parser_nl(parser);

        while (!parser_check_token(parser, TOKEN_ENDWHILE)) {
            parser_statement(parser);
        }
        parser_match(parser, TOKEN_ENDWHILE);

    } else if (parser_check_token(parser, TOKEN_LABEL)) {
        printf("STATEMENT-LABEL\n");
        parser_next_token(parser);

        if (!token_set_insert(&parser->labels_declared, parser->curr_token)) {
            fprintf(
                stderr,
                "Error: Label already exists: %.*s\n",
                (int)parser->curr_token.text_len,
                parser->curr_token.text_start
            );
            exit(EXIT_FAILURE);
        }

        parser_match(parser, TOKEN_IDENT);

    } else if (parser_check_token(parser, TOKEN_GOTO)) {
        printf("STATEMENT-GOTO\n");
        parser_next_token(parser);
        token_set_insert(&parser->labels_gotoed, parser->curr_token);
        parser_match(parser, TOKEN_IDENT);

    } else if (parser_check_token(parser, TOKEN_LET)) {
        printf("STATEMENT-LET\n");
        parser_next_token(parser);
        token_set_insert(&parser->symbols, parser->curr_token);

        parser_match(parser, TOKEN_IDENT);
        parser_match(parser, TOKEN_EQ);

        parser_expression(parser);

    } else if (parser_check_token(parser, TOKEN_INPUT)) {
        printf("STATEMENT-INPUT\n");
        parser_next_token(parser);
        token_set_insert(&parser->symbols, parser->curr_token);
        parser_match(parser, TOKEN_IDENT);

    } else {
        // TODO: Look into creating a function to print the TokenType enum names
        fprintf(
            stderr,
            "Error: Invalid statement at: %.*s\n",
            (int)parser->curr_token.text_len,
            parser->curr_token.text_start
        );
        exit(EXIT_FAILURE);
    }

    parser_nl(parser);
}

void parser_program(Parser *parser) {
    printf("PROGRAM\n");

    while (parser_check_token(parser, TOKEN_NEWLINE)) {
        parser_next_token(parser);
    }

    while (!parser_check_token(parser, TOKEN_EOF)) {
        parser_statement(parser);
    }

    for (size_t i = 0; i < parser->labels_gotoed.len; i++) {
        Token gotoed_token = parser->labels_gotoed.tokens[i];
        if (!token_set_contains(&parser->labels_declared, gotoed_token)) {
            fprintf(
                stderr,
                "Error: Attempting to GOTO to undeclared label: %.*s\n",
                (int)gotoed_token.text_len,
                gotoed_token.text_start
            );
            exit(EXIT_FAILURE);
        }
    }
}
