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

void reemplazar_char(char* string, char ch){
    char *reemplazado;

    if((reemplazado=strchr(string, ch)) != NULL){
        *reemplazado = '\0';
    }
}

void help_menu(FILE* stream, int exit_code){
    fprintf(stream, 
            "-h --help          Despliega el menú de ayuda.\n"
            "FILENAME           Ejecuta comandos desde batch file.\n"
            "no args            Espera por inputs del usuario.\n");
    exit(exit_code);
}

void get_username(char* dst){
    char *username = getenv("USERNAME");
    if(username == NULL){
        fprintf(stderr, "ERROR: username no encontrado.\n");
        help_menu(stderr, 1);
    }
    strcpy(dst, username);
}

void get_hostname(char* dst){

    read_text_file("/proc/sys/kernel/hostname", 32, dst);

    dst = strtok(dst, "\n");
    if(dst == NULL){
        printf("Error al buscar el hostname.\n");
        help_menu(stderr, 1);
    }
}

void get_current_path(char* dst){

    char *path = getenv("PWD");

    if(path == NULL){
        fprintf(stderr, "Error al buscar el path actual.\n");
        help_menu(stderr, 1);
    }

    char *home = getenv("HOME");
    int offset = strlen(home);
    if(strncmp(home, path, offset) == 0){
        path = path + offset;
    }

    strcpy(dst, path);
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
		    execl(paths[i], program, (char*) NULL);
        }
		/* returns only if an error occurs. */
		fprintf (stderr, "El programa %s no fue encontrado\n", program);
        exit(1);
	}
}

/*  Esta funcion devuelve 1 si encuentra '&' y lo reemplaza por '\0'. 
    Caso contrario devuelve 0.  */
int identificar_seg_plano(char* str){
    if(strchr(str, '&') != NULL){
        reemplazar_char(str, '&');
        return 1;
    }
    return 0;
}