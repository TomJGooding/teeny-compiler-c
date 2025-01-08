P=teenytiny
OBJECTS=lex.o parse.o emit.o
CFLAGS=-Wall -Wextra
LDLIBS=

$(P): $(OBJECTS)
