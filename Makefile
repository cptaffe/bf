
SRC = main.c lex.c
OBJ = $(SRC:.c=.o)
HDR = lex.h

BIN = bf

all: $(BIN)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

$(BIN): $(OBJ)

$(OBJ): $(HDR)
