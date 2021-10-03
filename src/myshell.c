#include "include/myshell.h"
#include "include/utils.h"

int main(int argc, char **argv){
    const char* const short_options = "h"; 

    const struct option long_options[] = {
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, 0 } 
    };
    
    if(argc == 2){
        int next_option;
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);
        if(next_option == 'h'){
            help_menu(stdout, 0);
        }
        else{
            leer_batchfile(argv[1]);
        }
    }
    else{
        char username[32];
        char hostname[32];
        char path[256];
        char user_input[50];

        while(1) {
            print_cmdline_prompt(username, hostname, path);
            
            fgets(user_input, 50, stdin);

            reemplazar_char(user_input, '\n');
            identificar_cmd(user_input);
        }
    }

    return 0;
}

void help_menu(FILE* stream, int exit_code){
    fprintf(stream, 
            "-h --help          Despliega el menú de ayuda.\n"
            "FILENAME           Ejecuta comandos desde batch file.\n"
            "no args            Espera por inputs del usuario.\n");
    exit(exit_code);
}

int get_username(char* dst){
    char *username = getenv("USERNAME");
    if(username == NULL){
        fprintf(stderr, "ERROR: username no encontrado.\n");
        exit(-1);
    }
    strcpy(dst, username);

    return 0;
}

int get_hostname(char* dst){

    read_text_file("/proc/sys/kernel/hostname", 32, dst);

    dst = strtok(dst, "\n");
    if(dst == NULL){
        printf("Error al buscar el hostname.\n");
        exit(-1);
    }

    return 0;
}

int get_current_path(char* dst){

    char *path = getenv("PWD");

    if(path == NULL){
        printf("Error al buscar el path actual.\n");
        exit(-1);
    }

    char *home = getenv("HOME");
    int offset = strlen(home);
    if(strncmp(home, path, offset) == 0){
        path = path + offset;
    }

    strcpy(dst, path);

    return 0;
}

void print_cmdline_prompt(char* username, char* hostname, char* current_path){
    get_username(username);
    get_hostname(hostname);
    get_current_path(current_path);

    printf("%s@%s:~%s$ ", username, hostname, current_path);
}

void identificar_cmd(char* cmd){
    while(isspace(*cmd)) cmd++;

    if(strcmp(cmd, "quit") == 0){
        exit(0);
    }
    else if(strcmp("clr", cmd) == 0){
        printf("\033[1;1H\033[2J"); //https://www.geeksforgeeks.org/clear-console-c-language/ y cambio /e por /33
    }
    else if(strncmp("cd ", cmd, 3) == 0 || strncmp("cd\t", cmd, 3) == 0){
        cambiar_dir(cmd+3); //offset de 3 char para pasar solo argumento de llamada
    }
    else if(strncmp("echo ", cmd, 5) == 0 || strncmp("echo\t", cmd, 5) == 0){
        eco(cmd+5); //offset de 5 char para pasar solo argumento de llamada
    }
    else{
        invocar(cmd);
    }
}

int invocar(char* program){
    pid_t pid = fork();

    switch(pid){
        case -1:
            fprintf(stderr, "ERROR: fork");
            exit(1);
        case 0:
            /* child */
        default:
            /* parent */
    }

    return 0;
}

void eco(char* cmd){
    int i;

    char* ptr = strtok(cmd, " ");
    if(ptr != NULL){
        i = 0;
        while(isspace(*ptr)) i++;
        if(ptr[i] == '$'){
            ptr = getenv(ptr+1);
        }
        printf("%s ", ptr);
    }

    while(ptr != NULL){
        ptr = strtok(NULL, " ");
        if(ptr != NULL){
            i = 0;
            while(isspace(*ptr)) i++;
            if(ptr[i] == '$'){
                ptr = getenv(ptr+1);
            }      
            printf("%s ", ptr);
        }
        else break;
    }

    printf("\n");
}

int cambiar_dir(char* dir){
    char* viejo = getenv("PWD");

    if(strncmp(dir,"- ", 2) == 0 || strcmp(dir,"-") == 0){
        char* nuevo = getenv("OLDPWD");
        if(nuevo != NULL){
            if(setenv("PWD", nuevo, 1) == 0){
                setenv("OLDPWD", viejo, 1);
            }
            else{
                fprintf(stderr, "ERROR: No se pudo cambiar la variable PWD.\n");
                return 1;
            }
        }
        else{
            fprintf(stderr, "ERROR: Oldpwd not set.\n");
            return 1;
        }
    }
    else{
        if(chdir(dir) != 0){
            fprintf(stderr, "ERROR: El directorio no está presente o no existe.\n");
            return 1;
        }
        else{
            char buf[256];
            if(getcwd(buf, 256) == NULL){
                fprintf(stderr, "ERROR: No se pudo obtener el path actual.\n");
                exit(-1);
            }
            if(setenv("PWD", buf, 1) == 0){
                setenv("OLDPWD", viejo, 1);
            }
            else{
                fprintf(stderr, "ERROR: No se pudo cambiar la variable PWD.\n");
                return 1;
            }
        }
    }

    return 0;
}

int leer_batchfile(char* file){
    FILE *fp;
    fp = fopen(file,"r");
    if(fp == NULL){
        fprintf(stderr, "ERROR: no se pudo abrir %s", file);
        help_menu(stderr, 1);
    }

    char cmd[100];

    while(fgets(cmd, sizeof(cmd), fp) !=NULL)
    {
        reemplazar_char(cmd, '\n');
        identificar_cmd(cmd);
    }

    fclose(fp);

    return 0;
}