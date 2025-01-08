#include <stdio.h>
#include <stdlib.h>

#include "emit.h"
#include "lex.h"
#include "parse.h"

int main(int argc, char **argv) {
    printf("Teeny Tiny Compiler\n");

    if (argc < 2) {
        fprintf(stderr, "Error: Compiler needs source file as argument\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open source file\n");
        exit(EXIT_FAILURE);
    }

    // There's *many* possible errors that should really be handled below,
    // but leaving as-is for now as this is only a learning exercise!
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *source = malloc(file_size + 1);
    fread(source, file_size, 1, file);
    source[file_size] = '\0';

    fclose(file);

    Lexer lexer = lexer_new(source);
    Emitter emitter = emitter_new();
    Parser parser = parser_new(&lexer, &emitter);

    parser_program(&parser);
    emitter_write_file(&emitter, "out.c");

    printf("Compiling completed\n");
}
