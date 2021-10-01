CC = gcc
CFLAGS = -Wall -Werror -pedantic
RM = rm -f

all:	main

main: 	src/myshell.c src/include/myshell.h
		$(CC) $(CFLAGS) $(OFLAGS) -o src/bin/myshell src/myshell.c

clean:
		$(RM) *.o src/bin/myshell