
CFLAGS+= -Wall -g -Ilex --std=c11
LNKFLAGS=-lpthread -lc

SRC = main.c bf.c lex_funcs.c tok.c stack.c lex_threadable.c jit.c link.c comp.c
OBJ = $(SRC:.c=.o)
HDR = bf.h tok.h lex_funcs.h tok.h stack.h lex_threadable.h jit.h link.h jit_arch.h jit_emit.h comp.h
LIB = lex/lex.a

LOG = log.txt # testing log

BIN = bf

all: $(BIN) test

$(BIN): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) $(LNKFLAGS) -o $(BIN) $(OBJ) $(LIB)

# accurate listing of dependencies. This can be avoided by uncommenting
# the following line, but this way it will only remake affected files.
# $(OBJ): $(HDR)

main.o: comp.h
comp.o: lex_funcs.h bf.h
comp.h: lex/lex.h jit.h
bf.o: bf.h
tok.o: tok.h
lex_funcs.o: lex_funcs.h
tok.o: tok.h
stack.o: stack.h
lex_threadable.o: lex_threadable.h
jit.o: jit.h jit_emit.h
link.o: link.h
jit.h bf.h: stack.h
jit.h: tok.h
lex_funcs.h: bf.h lex/lex.h
jit_emit.h: jit_arch.h jit.h

$(LIB):
	$(MAKE) -C $(dir $(LIB))

clean:
	rm $(BIN) $(OBJ)
	$(MAKE) clean -C $(dir $(LIB))

test:
	./test.sh ./$(BIN) $(LOG)
