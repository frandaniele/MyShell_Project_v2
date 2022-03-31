#include "include/myshell.h"
#include "include/utils.h"

int main(int argc, char **argv){
    const char* const short_options = "h"; 

    const struct option long_options[] = {  { "help", 0, NULL, 'h' },
                                            { NULL, 0, NULL, 0 } 
                                            };

    printf("holaaaaa\n");            
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

        instalar_signals(SIG_IGN);

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
    get_env_var(username, "USERNAME");
    get_hostname(hostname);
    get_env_var(current_path, "PWD");

    char *home = getenv("HOME");
    int offset = strlen(home);
    if(strncmp(home, current_path, offset) == 0){//si /home/user forma parte del PWD
        current_path[offset-1] = '~';// lo reemplazo por '~'
        current_path = current_path + offset-1;
    }

    printf("%s@%s:%s$ ", username, hostname, current_path);
    return;
}

void identificar_cmd(char* cmd){
    while(isspace(*cmd)) cmd++;

    if(strcmp(cmd, "quit") == 0 || strncmp(cmd, "quit ", 5) == 0 || strncmp("quit\t", cmd, 5) == 0){
        exit(0);
    }
    else if(strcmp(cmd, "clr") == 0 || strncmp("clr ", cmd, 4) == 0 || strncmp("clr\t", cmd, 4) == 0){
        printf("\033[3J\033[H\033[2J"); //https://unix.stackexchange.com/questions/124762/how-does-clear-command-work y cambio /e por /33
    }
    else if(strcmp(cmd, "cd") == 0 || strncmp("cd ", cmd, 3) == 0 || strncmp("cd\t", cmd, 3) == 0){
        cambiar_dir(cmd+3); //offset de 3 char para pasar solo argumento de llamada
    }
    else if(strcmp(cmd, "echo") == 0 || strncmp("echo ", cmd, 5) == 0 || strncmp("echo\t", cmd, 5) == 0){
        if(hay_redireccion(cmd+5)) redireccionar(cmd+5, 1);
        else eco(cmd+5); //offset de 5 char para pasar solo argumento de llamada
    }
    else if(hay_redireccion(cmd)){
        redireccionar(cmd, 0);
    }
    else if(strchr(cmd, '|')){
        tuberia(cmd);
    }
    else if(strcmp(cmd, "") == 0){}
    else{
        invocar(cmd);
    }
    return;
}

int invocar(char* program){
    const int MAX_ARGS = 4;
    char *arg_list[MAX_ARGS];
    int i = 1;
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

    if(strcmp(cmd, "")){//input = echo comentario|variable

        char* ptr = strtok(cmd, " ");//leo palabra a palabra
        while(ptr != NULL){
            char ch = '\0';//para caso de $envvar.!; etc

            while(isspace(*ptr)) ptr++;
            if(ptr[0] == '$'){// si es envvar
                int i = 0;
                while(isalpha((ptr+1)[i])){
                    i++;
                }
                ch = ptr[i+1];//si terminaba en algun signo o espacio
                reemplazar_char(ptr, ch);//lo reemplazo
                ptr = getenv(ptr+1);//obtengo envvar
            }      
            if(ptr == NULL) ptr = "";//chequeo por las dudas
            printf("%s", ptr);
            if(ch != '\0') printf("%c ", ch);//si tuve que agregar algun char arriba
            else printf(" ");//espacio para la siguiente palabra

            ptr = strtok(NULL, " ");
        }
    }
    printf("\n");//termine

    redireccionar_a_consola();
    return;
}

int cambiar_dir(char* dir){
    while(isspace(*dir)) dir++;

    if(strcmp(dir,"") == 0) return 0; //input = cd

    char* viejo = getenv("PWD");//para setear OLDPWD

    if(strncmp(dir,"-\t", 2) == 0 || strncmp(dir,"- ", 2) == 0 || strcmp(dir,"-") == 0){ //input = cd -
        dir = getenv("OLDPWD");
        if(dir==NULL){
            fprintf(stderr, "ERROR: Oldpwd not set.\n");
            return 1;
        }
    }
    //input = cd directorio
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

void tuberia(char* cmd){
    const int MAX_PIPE = 10;
    char* buffer[MAX_PIPE]; 

    int to_free;
    if((to_free = obtener_io(cmd, buffer, "|"))<2){ // me da los programas con sus arg a ejecutar
        fprintf(stderr, "Incomplete pipe\n");
        return;
    }

    char **programs[to_free];
    for(int j=0; j<to_free; j++){
        char** arg_list = obtener_args(buffer[j]);    // me arma la lista del prog para pasarle a execvp
        programs[j] = arg_list;//voy poniendo la lista de c/prog 
    }
    
    spawn_pipe(programs, to_free);

    for(int i = 0; i < to_free; i++){
        if(programs[i]) free(programs[i]); //libero lo alocado en obtener_args
        if(buffer[i]) free(buffer[i]); //libero lo alocado en obtener_io        
        i++;
    }
    
    return;
}

void redireccionar(char* cmd, int flag_eco){
    int append = 0;

    if(strchr(cmd, '<') && strchr(cmd, '>')){//hay redireccion doble
        if(strncmp((strchr(cmd, '>')+1),">", 1) == 0){//input = a < b >> c
            if(strncmp((strchr(cmd, '>')+1),">>", 2) == 0){//input = a < b >>> c
                fprintf(stderr, "Error de sintaxis\n");
                return;
            }
            append = 1;
        }
        if(flag_eco){//si lo llame desde echo 
            char *file = strtok(cmd, "<");//recibo "< input > output"
            if(append) file = strtok(file, ">>");//chequeo si fue con > o >>
            else file = strtok(file, ">");
            char txt[1024];
            if(read_text_file(trimwhitespace(file), 1024, txt)) return;//archivo input de echo
            if(append) file = strtok(NULL, ">>");
            else file = strtok(NULL, ">");
            if(reemplazar_stdout(trimwhitespace(file), append)){//output de echo
                fprintf(stderr, "No se pudo redireccionar\n");
                return;
            }
            if(strlen(txt) > 0) eco(txt);//chequeo que haya leido algo y ejecuto
            else{
                fprintf(stderr, "ERROR: archivo vacio\n");
                redireccionar_a_consola();
            }      
        }
        else redireccion_doble(cmd, append);//no fue echo
    }
    else if(strchr(cmd, '<')){//redireccion de entrada
        if(flag_eco){//desde echo
            char *file = strtok(cmd, "<");
            char txt[1024];
            if(read_text_file(trimwhitespace(file), 1024, txt)) return;//leo input

            if(strlen(txt) > 0) eco(txt);//imprimo lo leido
        }
        else redireccion_entrada(cmd);//no fue echo
    }
    else if(strchr(cmd, '>')){//redireccion de salida
        if(strncmp((strchr(cmd, '>')+1),">", 1) == 0){//chequeo si fue con > o >>
            if(strncmp((strchr(cmd, '>')+1),">>", 2) == 0){// >>>
                fprintf(stderr, "Error de sintaxis\n");
                return;
            }
            append = 1;
        }
        if(flag_eco){//desde echo
            char *file = strtok(cmd, ">");
            char *msj = file;//echo msj > file
            file = strtok(NULL, ">");
            if(reemplazar_stdout(trimwhitespace(file), append)){
                fprintf(stderr, "No se pudo redireccionar\n");
                return;
            }
            eco(msj);
        }
        else redireccion_salida(cmd, append);//no fue echo
    }
    else{
        fprintf(stderr, "Error inesperado redireccionando\n");
        return;
    }

    return;
}

void redireccion_entrada(char* cmd){
    char* buffer[2];
    int to_free;
    
    if((to_free = obtener_io(cmd, buffer, "<"))<2){ // debe haber 2 elementos alocados
        fprintf(stderr, "ERROR: file is empty\n");
        return;
    }  
    
    if(add_inputfile(buffer[0], buffer[1])){//buffer[0] < buffer[1]
        fprintf(stderr, "ERROR: no se pudo invocar\n");
        return;
    }

    for(int i = 0; i < to_free; i++){
        free(buffer[i]);        
    } 

    return;
}

void redireccion_salida(char* cmd, int append){
    char* buffer[2];
    int to_free;

    if((to_free = obtener_io(cmd, buffer, ">"))<2){ // debe haber 2 elementos alocados
        fprintf(stderr, "ERROR: file is empty\n");
        return;
    }  

    if(reemplazar_stdout(buffer[1], append)){//buffer[0] > buffer[1]
        fprintf(stderr, "No se pudo redireccionar\n");
        return;
    }

    invocar(buffer[0]);

    redireccionar_a_consola();

    for(int i = 0; i < to_free; i++){
        free(buffer[i]);        
    } 

    return;
}

void redireccion_doble(char* cmd, int append){
    char* buffer1[2], *buffer2[2];
    char *input, *output, *program;
    int to_free1, to_free2;
    
    if((to_free1 = obtener_io(cmd, buffer1, ">"))<2){ // (a < b) > (c) -> debo obtener 2
        fprintf(stderr, "ERROR: file is empty\n");
        return;
    }  
    if(strchr(buffer1[0], '<')){//input: a < b > c
        if((to_free2 = obtener_io(buffer1[0], buffer2, "<"))<2)  return; //buffer1[0] = a<b; buffer1[1] = c  
        input = buffer2[1];//b
        output = buffer1[1];//c
        program = buffer2[0];//a
    }
    else{//input: a > b < c
        if((to_free2 = obtener_io(buffer1[1], buffer2, "<"))<2){//buffer1[0] = a; buffer1[1] = b<c
            fprintf(stderr, "ERROR: file is empty\n");//falto algun argumento
            return;
        }  
        input = buffer2[1];//c
        output = buffer2[0];//b
        program = buffer1[0];//a
    }

    if(reemplazar_stdout(output, append)){
        fprintf(stderr, "ERROR: No se pudo redireccionar\n");
        return;
    }

    if(add_inputfile(program, input)){
        fprintf(stderr, "ERROR: no se pudo invocar\n");
        return;
    }

    redireccionar_a_consola();

    for(int i = 0; i < to_free1; i++){
        free(buffer1[i]);        
    } 

    for(int i = 0; i < to_free2; i++){
        free(buffer2[i]);        
    } 

    return;
}

int add_inputfile(char* program, char* input){
    if(strlen(program) == 0 || strlen(input) == 0) return 1;//alguna entrada invalida

    char *aux = (char*) malloc(strlen(input) + strlen(program) + 1);//voy a invocar aux con input
    if(aux == NULL)  return 1;//fallo malloc

    strcpy(aux, program);
    strcat(aux, " ");
    strcat(aux, input);

    invocar(aux);

    free(aux);

    return 0;
}