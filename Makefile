CC = gcc
CFLAGS = -Wall -Werror -pedantic
RM = rm -f

all:	main

main: 	src/myshell.c src/utils.c src/include/myshell.h src/include/utils.h
		$(CC) $(CFLAGS) $(OFLAGS) -o src/bin/myshell src/myshell.c src/utils.c

clean:
		$(RM) *.o src/bin/myshell