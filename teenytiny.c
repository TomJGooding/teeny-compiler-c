#include <stdio.h>
#include <stdlib.h>

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
        fprintf(stderr, "Error: Could not open file\n");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *source = malloc(file_size + 1);
    fread(source, file_size, 1, file);
    source[file_size] = '\0';

    fclose(file);

    Lexer lexer = lexer_new(source);
    Parser parser = parser_new(&lexer);

    parser_program(&parser);
    printf("Parsing completed\n");
}
