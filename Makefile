
CFLAGS+= -Wall -g -Ilex --std=c11
LNKFLAGS=-lpthread

SRC = main.c bf.c lex_funcs.c tok.c stack.c astree.c parse.c lex_threadable.c jit.c link.c
OBJ = $(SRC:.c=.o)
HDR = bf.h tok.h lex_funcs.h tok.h stack.h astree.h parse.h lex_threadable.h jit.h link.h jit_arch.h
LIB = lex/lex.a

LOG = log.txt # testing log

BIN = bf

all: $(BIN) test

$(BIN): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) $(LNKFLAGS) -o $(BIN) $(OBJ) $(LIB)

# accurate listing of dependencies. This can be avoided by uncommenting
# the following line, but this way it will only remake affected files.
# $(OBJ): $(HDR)

main.o: lex/lex.h parse.h jit.h lex_funcs.h bf.h tok.h
bf.o: bf.h
tok.o: tok.h
lex_funcs.o: lex_funcs.h
tok.o: tok.h
stack.o: stack.h
astree.o: astree.h
parse.o: parse.h bf.h
lex_threadable.o: lex_threadable.h
jit.o: jit.h jit_arch.h astree.h
link.o: link.h
jit.h bf.h: stack.h
lex_funcs.h: bf.h lex/lex.h
parse.h: astree.h stack.h tok.h

$(LIB):
	$(MAKE) -C $(dir $(LIB))

clean:
	rm $(BIN) $(OBJ)
	$(MAKE) clean -C $(dir $(LIB))

test:
	./test.sh ./$(BIN) $(LOG)
