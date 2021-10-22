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
            
            //posibilidad de limpiar zombies aca
            //while(waitpid(-1, NULL, WNOHANG)>0) job--;
        }
    }

    return 0;
}

void print_cmdline_prompt(char* username, char* hostname, char* current_path){
    get_env_var(username, "USERNAME");
    get_hostname(hostname);
    get_env_var(current_path, "PWD");

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
    return;
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
    else if(hay_redireccion(cmd)){
        redireccionar(cmd);
    }
    else if(strcmp(cmd, "") == 0){}
    else{
        invocar(cmd);
    }
    return;
}

int invocar(char* program){
    const int MAX_ARGS = 4;
    int i = 1;
    char *arg_list[MAX_ARGS];
    char aux[256] = "";

    char *ptr = strtok(program, " ");
    if(ptr[0] != '/'){//chequeo si puse el path con '/' al principio, sino lo agrego
        aux[0] = '/';
        strcat(aux, ptr);
        program = aux;
    }

    arg_list[0] = (char*)(strrchr(program, '/')+1); //argumento que contiene el nombre del programa solo
    while(ptr != NULL && i < MAX_ARGS){//guardo los demas argumentos, chequeo que no sean vacio
        ptr = strtok(NULL, " ");
        if(ptr != NULL && strcmp(ptr, "") != 0 && strcmp(ptr, " ") != 0 && strcmp(ptr, "\t") != 0){
            arg_list[i] = ptr;
            i++;
        }
    }

    int segundo_plano = identificar_seg_plano(arg_list[i-1]);
    if(strcmp(arg_list[i-1],"") == 0) i--;//si llame con & separado lo identifico
    spawn(program, arg_list, segundo_plano, i);//llamo funcion para ejecutar programa

    return 0;
}

void eco(char* cmd){
    while(isspace(*cmd)) cmd++;
    
    if(strchr(cmd, '>')){
        int append = 1;
        if(strncmp((strchr(cmd, '>')+1),">", 1) == 0){
            if(strncmp((strchr(cmd, '>')+1),">>", 2) == 0){
                fprintf(stderr, "Error de sintaxis\n");
                return;
            }
        }
        else{
            append = 0;
        }
        char *file = strtok(cmd, ">");
        file = strtok(NULL, ">");
        if(reemplazar_stdout(trimwhitespace(file), append)){
            fprintf(stderr, "No se pudo redireccionar\n");
            return;
        }
    } 

    if(strcmp(cmd, "")){//input = echo comentario|variable
        int i;

        char* ptr = strtok(cmd, " ");//leo palabra a palabra
        if(ptr != NULL){
            char ch = '\0';
            i = 0;
            while(isspace(*ptr)) i++;//elimino espacios
            if(ptr[i] == '$'){//chequeo si es env var
                if(ispunct(ptr[i+strlen(ptr)-1]) || isspace(ptr[i+strlen(ptr)-1])){
                    ch = ptr[i+strlen(ptr)-1];
                    reemplazar_char(ptr, ch);
                }   
                ptr = getenv(ptr+1);
            }
            if(ptr == NULL) ptr = "";
            printf("%s", ptr);
            if(ch != '\0') printf("%c ", ch);
            else printf(" ");
        }

        while(ptr != NULL){
            ptr = strtok(NULL, " ");
            if(ptr != NULL){
                i = 0;
                char ch = '\0';
                while(isspace(*ptr)) i++;
                if(ptr[i] == '$'){
                    if(ispunct(ptr[i+strlen(ptr)-1]) || isspace(ptr[i+strlen(ptr)-1])){
                        ch = ptr[i+strlen(ptr)-1];
                        reemplazar_char(ptr, ch);
                    }   
                    ptr = getenv(ptr+1);
                }      
                if(ptr == NULL) ptr = "";
                printf("%s", ptr);
                if(ch != '\0') printf("%c ", ch);
                else printf(" ");
            }
            else break;
        }
    }

    printf("\n");

    if(freopen("/dev/tty", "w", stdout) == NULL){
        perror("ERROR al redireccionar la salida a la consola");
        exit(1);
    }

    return;
}

int cambiar_dir(char* dir){
    while(isspace(*dir)) dir++;

    if(strcmp(dir,"") == 0) return 0; //input = cd

    char* viejo = getenv("PWD");

    if(strncmp(dir,"-\t", 2) == 0 || strncmp(dir,"- ", 2) == 0 || strcmp(dir,"-") == 0){ //input = cd -
        char* nuevo = getenv("OLDPWD");
        if(nuevo != NULL){  //oldpwd set
            if(setenv("PWD", nuevo, 1) == 0){  //pwd = oldpwd
                setenv("OLDPWD", viejo, 1); //oldpwd = pwd anterior
                printf("%s\n", nuevo);
            }
            else{ //en caso de que setenv de error
                fprintf(stderr, "ERROR: No se pudo cambiar la variable PWD.\n");
                return 1;
            }
        }
        else{ //oldpwd not set
            fprintf(stderr, "ERROR: Oldpwd not set.\n");
            return 1;
        }
    }
    else{ //input = cd directorio
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
            if(setenv("PWD", buf, 1) == 0){ //se pudo cambiar, cambio pwd y oldpwd
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
    fp = fopen(file,"r"); //abro el batch file
    if(fp == NULL){
        fprintf(stderr, "ERROR: no se pudo abrir %s\n", file);
        help_menu(stderr, 1);
    }

    char cmd[256];

    while(fgets(cmd, sizeof(cmd), fp) !=NULL)//leo los comandos linea a linea y ejecuto
    {
        reemplazar_char(cmd, '\n');
        identificar_cmd(cmd);
    }

    fclose(fp);

    return 0;
}

void tuberia(){
    return;
}

void redireccionar(char* cmd){
    if(strchr(cmd, '<') && strchr(cmd, '>')){
        if(strncmp((strchr(cmd, '>')+1),">", 1) == 0){
            if(strncmp((strchr(cmd, '>')+1),">>", 2) == 0){
                fprintf(stderr, "Error de sintaxis\n");
                return;
            }
            redireccion_doble(cmd, 1);
        }
        else{
            redireccion_doble(cmd, 0);
        }
    }
    else if(strchr(cmd, '<')){
        redireccion_entrada(cmd);
    }
    else if(strchr(cmd, '>')){
        if(strncmp((strchr(cmd, '>')+1),">", 1) == 0){
            if(strncmp((strchr(cmd, '>')+1),">>", 2) == 0){
                fprintf(stderr, "Error de sintaxis\n");
                return;
            }
            redireccion_salida(cmd, 1);
        }
        else{
            redireccion_salida(cmd, 0);
        }
    }
    else{
        fprintf(stderr, "Error inesperado redireccionando\n");
    }
    return;
}

void redireccion_entrada(char* cmd){
    char* buffer[2];
    
    if(obtener_io(cmd, buffer, "<"))  return;

    if(add_inputfile(buffer[0], buffer[1])){
        fprintf(stderr, "ERROR: no se pudo invocar\n");
        return;
    }

    return;
}

void redireccion_salida(char* cmd, int append){
    char* buffer[2];

    if(obtener_io(cmd, buffer, ">"))  return;

    if(reemplazar_stdout(buffer[1], append)){
        fprintf(stderr, "No se pudo redireccionar\n");
        return;
    }

    invocar(buffer[0]);

    if(freopen("/dev/tty", "w", stdout) == NULL){
        perror("ERROR al redireccionar la salida a la consola");
        exit(1);
    }

    return;
}

void redireccion_doble(char* cmd, int append){
    char* buffer1[2], *buffer2[2];
    char *input, *output, *program;
    
    if(obtener_io(cmd, buffer1, ">"))  return;
    if(strchr(buffer1[0], '<')){
        if(obtener_io(buffer1[0], buffer2, "<"))  return;
        input = buffer2[1];
        output = buffer1[1];
        program = buffer2[0];
    }
    else{
        if(obtener_io(buffer1[1], buffer2, "<"))  return;
        input = buffer2[1];
        output = buffer2[0];
        program = buffer1[0];
    }

    if(reemplazar_stdout(output, append)){
        fprintf(stderr, "ERROR: No se pudo redireccionar\n");
        return;
    }

    if(add_inputfile(program, input)){
        fprintf(stderr, "ERROR: no se pudo invocar\n");
        return;
    }

    if(freopen("/dev/tty", "w", stdout) == NULL){
        perror("ERROR al redireccionar la salida a la consola");
        exit(1);
    }

    return;
}

void signaling(){
    return;
}

int add_inputfile(char* program, char* input){
    char *aux = (char*) malloc(strlen(input) + strlen(program) + 1);
    if((aux == NULL) || strlen(program) == 0 || strlen(input) == 0)   
        return 1;

    strcpy(aux, program);
    strcat(aux, " ");
    strcat(aux, input);

    invocar(aux);

    free(aux);

    return 0;
}