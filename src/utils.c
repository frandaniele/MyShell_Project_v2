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

//int spawn(char* program, char** arg_list){
//    const int N_PATHS = 5;
//	pid_t child_pid;
//
//    char paths[N_PATHS][32] = { "/bin", 
//                                "/usr/bin",
//                                "/usr/local/bin",
//                                "/usr/games",
//                                "/usr/local/games"};
//
//	/* Duplicate this process. */
//	child_pid = fork();
//
//	if (child_pid != 0){
//		return child_pid;
//	}
//	else{
//        for(int i = 0; i < N_PATHS; i++){
//		    execl(program, arg_list, (char*) NULL);
//        }
//		
//		/* returns only if an error occurs. */
//		fprintf (stderr, "an error occurred in execvp\n");
//		exit(1);
//	}
//}