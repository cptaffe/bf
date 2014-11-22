
CFLAGS+= -DDEBUG

SRC = main.c lex.c bf.c
OBJ = $(SRC:.c=.o)
HDR = lex.h bf.h tok.h

BIN = bf

all: $(BIN)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

$(BIN): $(OBJ)

$(OBJ): $(HDR)

clean:
	rm $(BIN) $(OBJ)
