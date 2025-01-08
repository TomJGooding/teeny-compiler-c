#include "emit.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

#define EMITTER_BUF_INIT_CAPACITY 256

Emitter emitter_new() {
    Emitter emitter = {
        .header_capacity = EMITTER_BUF_INIT_CAPACITY,
        .body_capacity = EMITTER_BUF_INIT_CAPACITY
    };
    emitter.header_buf = malloc(emitter.body_capacity);
    emitter.body_buf = malloc(emitter.header_capacity);

    return emitter;
}

void emitter_header_resize(Emitter *emitter) {
    size_t new_capacity = emitter->header_capacity * 2;
    char *new_buf = realloc(emitter->header_buf, new_capacity);
    emitter->header_buf = new_buf;
    emitter->header_capacity = new_capacity;
}

void emitter_body_resize(Emitter *emitter) {
    size_t new_capacity = emitter->body_capacity * 2;
    char *new_buf = realloc(emitter->body_buf, new_capacity);
    emitter->body_buf = new_buf;
    emitter->body_capacity = new_capacity;
}

void emitter_header_emit_nstr(Emitter *emitter, char *code, size_t code_len) {
    if (emitter->header_len + code_len >= emitter->header_capacity) {
        emitter_header_resize(emitter);
    }
    memcpy(emitter->header_buf + emitter->header_len, code, code_len);
    emitter->header_len += code_len;
}

void emitter_header_emit_str(Emitter *emitter, char *code) {
    emitter_header_emit_nstr(emitter, code, strlen(code));
}

void emitter_header_emit_token_text(Emitter *emitter, Token token) {
    emitter_header_emit_nstr(emitter, token.text_start, token.text_len);
}

void emitter_emit_nstr(Emitter *emitter, char *code, size_t code_len) {
    if (emitter->body_len + code_len >= emitter->body_capacity) {
        emitter_body_resize(emitter);
    }
    memcpy(emitter->body_buf + emitter->body_len, code, code_len);
    emitter->body_len += code_len;
}

void emitter_emit_str(Emitter *emitter, char *code) {
    emitter_emit_nstr(emitter, code, strlen(code));
}

void emitter_emit_token_text(Emitter *emitter, Token token) {
    emitter_emit_nstr(emitter, token.text_start, token.text_len);
}

void emitter_write_file(Emitter *emitter, char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open emitter file\n");
        exit(EXIT_FAILURE);
    }

    fwrite(emitter->header_buf, emitter->header_len, 1, file);
    fwrite(emitter->body_buf, emitter->body_len, 1, file);

    fclose(file);
}
