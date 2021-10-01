#include "include/utils.h"

int read_text_file(char *directory, int size, char *buffer)
{
    FILE *fptr;
    
    if ((fptr = fopen(directory, "rb")) == NULL)
    {
        printf("Error! opening file\n");
        // Program exits if the file pointer returns NULL.
        exit(-1);
    }

    int leer = 0;

    while ((leer = fread(buffer, size, 1, fptr)) > 0);

    fclose(fptr); 

    return 0;
}

char* busca_palabras(char* str, char* separator, int n)
{
    char *ptr = strtok(str, separator);
    for(int i=0; i<n; i++) 
    {
        ptr = strtok(NULL, separator);
    }
    return ptr;
}

void reemplazar_char(char* string, char ch){
    char *reemplazado;

    if ((reemplazado=strchr(string, ch)) != NULL){
        *reemplazado = '\0';
    }
}