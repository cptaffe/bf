
CFLAGS+= -g -DDEBUG -Ilex

SRC = main.c bf.c lex_funcs.c tok.c stack.c
OBJ = $(SRC:.c=.o)
HDR = bf.h tok.h lex_funcs.h
LIB = lex/lex.a

LOG = log.txt # testing log

BIN = bf

all: $(BIN) test

$(BIN): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LIB)

$(OBJ): $(HDR)

$(LIB):
	$(MAKE) -C $(dir $(LIB))

clean:
	rm $(BIN) $(OBJ)
	$(MAKE) clean -C $(dir $(LIB))

test:
	./test.sh ./$(BIN) $(LOG)
