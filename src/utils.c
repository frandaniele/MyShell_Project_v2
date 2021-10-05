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

    if((reemplazado=strchr(string, ch)) != NULL){
        *reemplazado = '\0';
    }
}

int spawn(char* program, char** arg_list){
	pid_t child_pid;

	/* Duplicate this process. */
	child_pid = fork();

	if (child_pid != 0){
		return child_pid;
	}
	else{
        char paths[5][64] = {   "/bin", 
                                "/usr/bin",
                                "/usr/local/bin",
                                "/usr/games",
                                "/usr/local/games"};
                                
        for(int i = 0; i < 5; i++){
            strcat(paths[i],program);
            printf("lo llamo con %s\n", paths[i]);
		    execl(paths[i], program, (char*) NULL);
        }
		/* returns only if an error occurs. */
		fprintf (stderr, "El programa %s no fue encontrado\n", program);
		abort();
	}
}