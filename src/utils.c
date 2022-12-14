#include "include/utils.h"

/* declarada aca, recomendado con funciones estaticas. envia sig a proceso en 1er plano actual*/
static void enviar_signal(int sig);

/* child_pid = proc actual en 1er plano; head -> lista de jobs en 2do plano */
pid_t child_pid;
static Node *head = NULL;

void append_nodo(Node** head_ref, pid_t pid){
    Node* new_node = (Node*)malloc(sizeof(Node));
 
    Node *last = *head_ref;  
    
    new_node->pid = pid;
    new_node->next = NULL;
 
    /* Lista vacia, new node = head */
    if(*head_ref == NULL){
        new_node->n_job = 1;
        *head_ref = new_node;
        return;
    } 
      
    /* Buscamos el ultimo nodo */
    while(last->next != NULL){
        last = last->next;
    }
  
    last->next = new_node;
    new_node->n_job = last->n_job + 1;
    
    return;   
}

int eliminar_nodo(Node** head_ref, pid_t pid){
    // guardamos head para iterar
    Node *temp = *head_ref;
    Node *prev;
 
    //si head contiene el pid del proceso a eliminar
    if(temp != NULL && temp->pid == pid){
        *head_ref = temp->next; //cambiamos head
        int job = temp->n_job;
        free(temp);
        return job;
    }
 
    // buscamos el pid del proceso a eliminar
    while(temp != NULL && temp->pid != pid){
        prev = temp;
        temp = temp->next;
    }
 
    //si el pid no estaba en la lista
    if(temp == NULL)    return 1;
 
    //deslinkeamos el nodo
    prev->next = temp->next;
    
    int job = temp->n_job;

    free(temp);

    return job;
}

int last_job(Node** head_ref){
    Node *last = *head_ref;

    while(last->next != NULL){
        last = last->next;
    }

    return last->n_job;
} 

int read_text_file(char *directory, int size, char *buffer){
    FILE *fptr;
    if((fptr = fopen(directory, "rb")) == NULL){
        fprintf(stderr, "Error! opening file\n");
        return 1;
    }

    int leer = 0;
    while((leer = fread(buffer, size, 1, fptr)) > 0);

    fclose(fptr); 
    return 0;
}

void reemplazar_char(char* string, char ch){
    char *reemplazado;

    if((reemplazado=strchr(string, ch)) != NULL){
        *reemplazado = '\0';
    }
    return;
}

/* https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way    */
char* trimwhitespace(char *str){
    if(str == NULL) return NULL;

    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

void help_menu(FILE* stream, int exit_code){
    fprintf(stream, 
            "-h --help          Despliega el men?? de ayuda.\n"
            "FILENAME           Ejecuta comandos desde batch file.\n"
            "no args            Espera por inputs del usuario.\n");
    exit(exit_code);
}

void get_env_var(char* dst, char* var){
    char *ptr = getenv(var);
    if(ptr == NULL){
        fprintf(stderr, "ERROR: %s no encontrado.\n", var);
        help_menu(stderr, 1);
    }
    strcpy(dst, ptr);
    return;
}

void get_hostname(char* dst){
    if(read_text_file("/proc/sys/kernel/hostname", 32, dst)) return;

    dst = strtok(dst, "\n");
    if(dst == NULL){
        fprintf(stderr, "Error al buscar el hostname.\n");
        help_menu(stderr, 1);
    }
    return;
}

void limpiar_zombies(){
    int child_status;
    pid_t zombie_pid;             
    while((zombie_pid = waitpid(-1, &child_status, WNOHANG))>0){//WNOHANG -> si no hay zombie no espero                
        printf("[%i]\t%i\t", eliminar_nodo(&head, zombie_pid), zombie_pid);// [job] pid exit_condition
        if(WIFEXITED(child_status)){
            if(WEXITSTATUS(child_status) == 0)      printf("Done\n");
            else    printf("terminated with error code\t%i\n", WEXITSTATUS(child_status));
        }
        if(WIFSIGNALED(child_status)){
            printf("exited via signal\t%i\n", WTERMSIG(child_status));
        }
    }

    return;
}

char** obtener_args(char* src){
    const int MAX_ARGS = 10;
    int i = 0;
    
    char **arg_list = malloc(sizeof(char*)*MAX_ARGS);

    char *ptr = strtok(src, " ");
    while(ptr != NULL && i < MAX_ARGS-1){//nombre + 8 args max
        if(strcmp(ptr, "") != 0 && strcmp(ptr, " ") != 0 && strcmp(ptr, "\t") != 0){
            arg_list[i] = ptr;
            i++;
        }
        ptr = strtok(NULL, " ");
    }
    arg_list[i] = NULL;//null terminated list

    return arg_list;
}

int spawn(char* program, char** arg_list, int segundo_plano, int cant_args){

    /* Duplicate this process. */
	child_pid = fork();

    switch(child_pid){
        case -1:
            perror("ERROR: fork");
            return 1;
        case 0: 
            if(!segundo_plano) {
                instalar_signals(SIG_DFL);
            }

            /* pruebo path absoluto */
            ejecutar(arg_list[0], arg_list, cant_args, program);

            /* pruebo los paths en env var $PATH    */
            char *paths = getenv("PATH");
            char *ptr = strtok(paths, ":");
            char aux[256];

            while(ptr != NULL){
                strcpy(aux,ptr);
                strcat(aux,program);
                ejecutar(program, arg_list, cant_args, aux);
                ptr = strtok(NULL, ":");
            }

            /* busco en el path que me encuentro   */
            get_env_var(aux, "PWD");
            strcat(aux,program);
            ejecutar(program, arg_list, cant_args, aux);
            
            /* returns only if an error occurs. */
            fprintf(stderr, "ERROR con programa %s: %s\n", program, strerror(errno));
            exit(1);
        default:    ;        
            limpiar_zombies();

            if(segundo_plano){
                //Ejecuto en 2do plano
                append_nodo(&head,child_pid);
                printf("[%i] %i\n", last_job(&head), child_pid);  
            }
            else{   //Ejecuto en 1er plano
                int child_status = 0;    
                esperar_proceso(child_pid, child_status);
            }
    }
    return 0;
}

int spawn_pipe(char*** processes, int n_processes){
    int child_status = 0;

    pid_t pid = fork();
    switch(pid){
        case -1:
            perror("Fork error");
            return 1;
        case 0: ;
            instalar_signals(SIG_DFL);

            int n_processes_left = n_processes;

            int fds[2];
            int fdin_viejo = 0;

            while(n_processes > 0){
                if(pipe(fds) != 0){
                    perror("ERROR pipe");
                    return 1;
                }
                pid_t gchild_pid = fork();

                switch(gchild_pid){
                    case -1:
                        perror("Fork error");
                        return 1;
                    case 0: ;
                    //"nieto" recibe input del prog anterior, escribe en su write end y ejecuta el programa
                        int proc_to_exec = n_processes-n_processes_left;
                        dup2(fdin_viejo, STDIN_FILENO);
                        if((n_processes_left-1) != 0) dup2(fds[1], STDOUT_FILENO);
                        close(fds[0]);
                        execvp(processes[proc_to_exec][0], processes[proc_to_exec]);

                        perror("EXEC error");
                        close(fds[1]);
                        exit(1);
                    default: ; 
                    //child de myshell, espera que cada programa se ejecute y guarda su read end del pipe para el prox prog
                        if(waitpid(gchild_pid, NULL, WUNTRACED) == -1){//wuntraced: vuelvo tambien si fue stopped
                            perror("Waitpid");
                            exit(1);
                        }           
                        close(fds[1]);
                        fdin_viejo = fds[0];
                        n_processes_left--;
                        if(n_processes_left == 0) exit(0);
                }
            }
        default:
        //myshell: espera que terminen los pipes
            esperar_proceso(pid, child_status);
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
    return;
}

void esperar_proceso(pid_t pid, int status){
    instalar_signals(enviar_signal);
            
    if(waitpid(pid, &status, WUNTRACED) == -1){//wuntraced: vuelvo tambien si fue stopped
        perror("Waitpid");
        instalar_signals(SIG_IGN);
        exit(1);
    }

    instalar_signals(SIG_IGN);
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

int hay_redireccion(char* cmd){
    return (strchr(cmd, '<') || strchr(cmd, '>'));
}

//ej: ps -e | grep 0 -> programs[0] = ps -e; programs[1] = grep 0; return 2;
int obtener_io(char* cmd, char** programs, char* ch){
    cmd = trimwhitespace(cmd);
    if(strcmp(cmd, ch)==0){// caso de cmd = '<', '>', '|', sin programas    
        return -1;
    } 

    char *ptr = strtok(cmd, ch);
    int i = 0;
    while(ptr != NULL && i<sizeof(programs)){
        programs[i] = malloc(strlen(ptr));
        strcpy(programs[i], trimwhitespace(ptr));
        i++;
        ptr = strtok(NULL, ch);
    } 

    if(i==0)    return -1; //no se aloco 
    else        return i; //n de elementos alocados
}

int reemplazar_stdout(char* file, int append){
    char mode[3];
    
    if(append) strcpy(mode, "a+");
    else strcpy(mode, "w");

    if(freopen(file, mode, stdout) == NULL){//associates a new filename with the given open stream and at the same time closes the old file in the stream
        perror("ERROR en redireccion de salida");
        redireccionar_a_consola();
        return 1;
    }

    return 0;
}

void redireccionar_a_consola(){
    if(freopen("/dev/tty", "w", stdout) == NULL){ //asocia a stdout un new filename (dev/tty) y cierra el file viejo en el stream
        perror("ERROR al redireccionar la salida a la consola");
        exit(1);
    }

    return;
}

void instalar_signals(__sighandler_t s){
    signal(SIGINT, s);
    signal(SIGTSTP, s);
    signal(SIGQUIT, s);

    return;
}

static void enviar_signal(int sig){
    kill(child_pid, sig);

    return;
}