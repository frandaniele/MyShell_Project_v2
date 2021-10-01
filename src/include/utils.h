#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_text_file(char *directory, int size, char *buffer);

char* busca_palabras(char* str, char* separator, int n);

void reemplazar_char(char* string, char ch);