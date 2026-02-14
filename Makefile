CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
SRC = lexical.c main.c
OUT = lexer

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
