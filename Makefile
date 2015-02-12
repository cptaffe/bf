
CFLAGS+= -Wall -g -Ilex --std=c11 -D_GNU_SOURCE
LNKFLAGS=-pthread -lc

SRC = main.c bf.c lex_funcs.c tok.c stack.c lex_threadable.c jit.c link.c comp.c
OBJ = $(SRC:.c=.o)
HDR = bf.h tok.h lex_funcs.h tok.h stack.h lex_threadable.h jit.h link.h jit_arch.h jit_emit.h comp.h
LIB = lex/lex.a

# testing log
LOG = log.txt

BIN = bf

all: $(BIN) test

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(BIN): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) $(LNKFLAGS) -o $(BIN) $(OBJ) $(LIB)

# accurate listing of dependencies. This can be avoided by uncommenting
# the following line, but this way it will only remake affected files.
$(OBJ): $(HDR)

$(LIB):
	$(MAKE) -C $(dir $(LIB))

clean:
	$(RM) $(BIN) $(OBJ)
	$(MAKE) clean -C $(dir $(LIB))

test:
	./test.sh ./$(BIN) $(LOG)
