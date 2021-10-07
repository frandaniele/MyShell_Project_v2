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

    strcpy(dst, path);
}

int spawn(char* program, char** arg_list, int segundo_plano, int cant_args){
	pid_t child_pid;
    int child_status;

	/* Duplicate this process. */
	child_pid = fork();

    switch(child_pid){
        case -1:
            fprintf(stderr, "ERROR: fork");
            exit(1);
        case 0: ;
            /* pruebo path absoluto */
            ejecutar(arg_list[0], arg_list, cant_args, program);

            /* busco en paths estandar  */  
            char paths[5][128] = {  "/bin", 
                                    "/usr/bin",
                                    "/usr/local/bin",
                                    "/usr/games",
                                    "/usr/local/games"};
                                  
            for(int i = 0; i < 5; i++){
                strcat(paths[i],program);
                ejecutar(program, arg_list, cant_args, paths[i]);
            }

            /* busco en el path que me encuentro   */
            char path_actual[128];
            get_current_path(path_actual);
            strcat(path_actual,program);
            ejecutar(program, arg_list, cant_args, path_actual);
            
            /* returns only if an error occurs. */
            fprintf(stderr, "El programa %s no fue encontrado\n", program);
            exit(1);
        default:    ;
            static int job = 1;
            while(waitpid(-1, NULL, WNOHANG)>0) job--;
            if(segundo_plano){
                //Ejecuto en 2do plano
                printf("[%i] %i\n", job, child_pid);  
                job++;     
            }
            else{
                //Ejecuto en 1er plano
                waitpid(child_pid, &child_status, 0);
            }
    }
    return 0;
}

void ejecutar(char* program, char** arg_list, int cant_args, char* path){
    if(cant_args == 1){
        execl(path, program, (char*) NULL);
    }
    if(cant_args == 2){
        execl(path, program, arg_list[1], (char*) NULL);
    }
    if(cant_args == 3){
        execl(path, program, arg_list[1], arg_list[2], (char*) NULL);
    }	
    if(cant_args == 4){
        execl(path, program, arg_list[1], arg_list[2], arg_list[3], (char*) NULL);
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