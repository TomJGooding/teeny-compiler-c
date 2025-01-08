#pragma once

#include <stddef.h>

#include "lex.h"

typedef struct Emitter {
    char *header_buf;
    size_t header_len;
    size_t header_capacity;

    char *body_buf;
    size_t body_len;
    size_t body_capacity;
} Emitter;

Emitter emitter_new();

void emitter_header_emit_str(Emitter *emitter, char *code);

void emitter_header_emit_token_text(Emitter *emitter, Token token);

void emitter_emit_str(Emitter *emitter, char *code);

void emitter_emit_token_text(Emitter *emitter, Token token);

void emitter_write_file(Emitter *emitter, char *filepath);
