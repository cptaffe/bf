
CFLAGS+= -g -DDEBUG -Ilex

SRC = main.c bf.c lex_funcs.c
OBJ = $(SRC:.c=.o)
HDR = bf.h tok.h lex_funcs.h
LIB = lex/lex.a

BIN = bf

all: $(BIN)

$(BIN): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LIB)

$(OBJ): $(HDR)

$(LIB):
	$(MAKE) -C $(dir $(LIB))

clean:
	rm $(BIN) $(OBJ)
	$(MAKE) clean -C $(dir $(LIB))
