CC= gcc
CFLAGS= -ansi -pedantic-errors -Wall -fstack-protector-all -Werror

public15-testprogram.x: public15-testprogram.o
	$(CC) public15-testprogram.o -o public15-testprogram.x

public15-testprogram.o: public15-testprogram.c
	$(CC) $(CFLAGS) -c public15-testprogram.c
