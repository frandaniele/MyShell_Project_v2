#include "include/myshell.h"
#include "include/utils.h"

int main(int argc, char **argv){
    const char* const short_options = "h"; 

    const struct option long_options[] = {  { "help", 0, NULL, 'h' },
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
        char user_input[256];

        while(1) {
            print_cmdline_prompt(username, hostname, path);
            
            fgets(user_input, 256, stdin);

            reemplazar_char(user_input, '\n');
            identificar_cmd(user_input);
        }
    }

    return 0;
}

void print_cmdline_prompt(char* username, char* hostname, char* current_path){
    get_username(username);
    get_hostname(hostname);
    get_current_path(current_path);

    char *home = getenv("HOME");
    int offset = strlen(home);
    if(strncmp(home, current_path, offset) == 0){
        if(strcmp(home,current_path) == 0){
            current_path = "~";
        }
        else{
            current_path = current_path + offset;
        }
    }

    printf("%s@%s:%s$ ", username, hostname, current_path);
}

void identificar_cmd(char* cmd){
    while(isspace(*cmd)) cmd++;

    if(strcmp(cmd, "quit") == 0 || strncmp(cmd, "quit ", 5) == 0 || strncmp("quit\t", cmd, 5) == 0){
        printf("Hasta luego. Gracias por utilizar mi shell :p\n");
        exit(0);
    }
    else if(strcmp(cmd, "clr") == 0 || strncmp("clr ", cmd, 4) == 0 || strncmp("clr\t", cmd, 4) == 0){
        printf("\033[3J\033[H\033[2J"); //https://unix.stackexchange.com/questions/124762/how-does-clear-command-work y cambio /e por /33
    }
    else if(strcmp(cmd, "cd") == 0 || strncmp("cd ", cmd, 3) == 0 || strncmp("cd\t", cmd, 3) == 0){
        cambiar_dir(cmd+3); //offset de 3 char para pasar solo argumento de llamada
    }
    else if(strcmp(cmd, "echo") == 0 || strncmp("echo ", cmd, 5) == 0 || strncmp("echo\t", cmd, 5) == 0){
        eco(cmd+5); //offset de 5 char para pasar solo argumento de llamada
    }
    else if(strcmp(cmd, "") == 0){}
    else{
        invocar(cmd);
    }
}

int invocar(char* program){
    while(isspace(*program)) program++;

    const int MAX_ARGS = 4;

    int i = 1;
    char *arg_list[MAX_ARGS];

    char aux[256] = "";

    char *ptr = strtok(program, " ");
    if(ptr[0] != '/'){
        aux[0] = '/';
        strcat(aux, ptr);
        program = aux;
    }

    arg_list[0] = (char*)(strrchr(program, '/')+1);
    while(ptr != NULL && i < MAX_ARGS){
        ptr = strtok(NULL, " ");
        if(ptr != NULL){
            arg_list[i] = ptr;
            i++;
        }
    }

    int segundo_plano = identificar_seg_plano(arg_list[i-1]);
    spawn(program, arg_list, segundo_plano, i);

    return 0;
}

void eco(char* cmd){
    while(isspace(*cmd)) cmd++;
    
    if(strcmp(cmd, "")){
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
    }

    printf("\n");
}

int cambiar_dir(char* dir){
    while(isspace(*dir)) dir++;

    if(strcmp(dir,"") == 0) return 0;

    char* viejo = getenv("PWD");

    if(strncmp(dir,"-\t", 2) == 0 || strncmp(dir,"- ", 2) == 0 || strcmp(dir,"-") == 0){
        char* nuevo = getenv("OLDPWD");
        if(nuevo != NULL){
            if(setenv("PWD", nuevo, 1) == 0){
                setenv("OLDPWD", viejo, 1);
                printf("%s\n", nuevo);
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
                help_menu(stderr, 1);
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
        fprintf(stderr, "ERROR: no se pudo abrir %s\n", file);
        help_menu(stderr, 1);
    }

    char cmd[256];

    while(fgets(cmd, sizeof(cmd), fp) !=NULL)
    {
        reemplazar_char(cmd, '\n');
        identificar_cmd(cmd);
    }

    fclose(fp);

    return 0;
}