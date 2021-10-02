#include "include/myshell.h"
#include "include/utils.h"

int main(int argc, char **argv){
    
    int next_option;
    int h = 0;
    
    const char* const short_options = "h"; 

    const struct option long_options[] = {
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, 0 } 
    };

    do
    {
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);
        switch (next_option)
        {
        case 'h': 
            h = 1;
            break;
        case '?': 
            break;
        case -1: 
            break;
        default: 
            printf("Unexpected error.\n");
            return 1;
        }
    }while (next_option != -1); 
    
    if(h){
        help_menu();
    }

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

    return 0;
}

void help_menu(){
    
    printf("-h --help       Despliega el menú de ayuda\n");
}

int get_username(char* dst){

    strcpy(dst,getenv("USERNAME"));

    if(dst == NULL){
        printf("Error al buscar el username.\n");
        exit(-1);
    }

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
    int offset = strlen(getenv("HOME"));
    path = path + offset;

    if(path == NULL){
        printf("Error al buscar el path actual.\n");
        exit(-1);
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

void invocar(char* program){

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
        char* ptr = strtok(NULL, " ");
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

void cambiar_dir(char* dir){
    char* viejo = getenv("PWD");

    if(strncmp(dir,"- ", 2) == 0 || strcmp(dir,"-") == 0){
        char* nuevo = getenv("OLDPWD");
        if(nuevo != NULL){
            if(setenv("PWD", viejo, 1) == 0){
                setenv("OLDPWD", viejo, 1);
            }
            else{
                fprintf(stderr, "ERROR: No se pudo cambiar la variable PWD.\n");
            }
        }
        else{
            fprintf(stderr, "ERROR: Oldpwd not set.\n");
        }
    }
    else{
        if(chdir(dir) != 0){
            fprintf(stderr, "ERROR: El directorio no está presente o no existe.\n");
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
            }
        }
    }
}