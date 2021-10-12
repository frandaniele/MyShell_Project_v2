#include "include/utils.h"

void append(struct Node** head_ref, pid_t pid){
    /* 1. allocate node */
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
 
    struct Node *last = *head_ref;  /* used in step 5*/
  
    /* 2. put in the data  */
    new_node->pid = pid;

    /* 3. This new node is going to be the last node, so make next
          of it as NULL*/
    new_node->next = NULL;
 
    /* 4. If the Linked List is empty, then make the new node as head */
    if (*head_ref == NULL){
        new_node->n_job = 1;
        *head_ref = new_node;
        return;
    } 
      
    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;
  
    /* 6. Change the next of last node */
    last->next = new_node;
    new_node->n_job = last->n_job + 1;
    
    return;   
}

void deleteNode(struct Node** head_ref, pid_t pid){
    // Store head node
    struct Node *temp = *head_ref, *prev;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->pid == pid) {
        *head_ref = temp->next; // Changed head
        free(temp); // free old head
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->pid != pid) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL)
        return;
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp); // Free memory
}

int last_job(struct Node** head_ref){
    struct Node *last = *head_ref;

    while (last->next != NULL)
        last = last->next;

    return last->n_job;
} 

int pid_job(struct Node** head_ref, pid_t pid){
    struct Node *last = *head_ref;

    do{
        printf("pid %i last %i\n", pid, last->pid);
        if(last->pid == pid) return last->n_job;
        last = last->next;
    }while (last->next != NULL);
    return last->n_job;
}

int read_text_file(char *directory, int size, char *buffer){
    FILE *fptr;
    
    if ((fptr = fopen(directory, "rb")) == NULL)
    {
        fprintf(stderr, "Error! opening file\n");
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

void get_env_var(char* dst, char* var){
    char *ptr = getenv(var);
    if(ptr == NULL){
        fprintf(stderr, "ERROR: %s no encontrado.\n", var);
        help_menu(stderr, 1);
    }
    strcpy(dst, ptr);
}

void get_hostname(char* dst){
    read_text_file("/proc/sys/kernel/hostname", 32, dst);

    dst = strtok(dst, "\n");
    if(dst == NULL){
        fprintf(stderr, "Error al buscar el hostname.\n");
        help_menu(stderr, 1);
    }
}

int spawn(char* program, char** arg_list, int segundo_plano, int cant_args){
	pid_t child_pid;
    int child_status;
    static struct Node *head = NULL;

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
            get_env_var(path_actual, "PWD");
            strcat(path_actual,program);
            ejecutar(program, arg_list, cant_args, path_actual);
            
            /* returns only if an error occurs. */
            fprintf(stderr, "El programa %s no fue encontrado\n", program);
            exit(1);
        default:    ;
            pid_t zombie_pid;             
            while((zombie_pid = waitpid(-1, &child_status, WNOHANG))>0){
                deleteNode(&head, zombie_pid);
                printf("[%i]\t%i\t", pid_job(&head, zombie_pid), zombie_pid);
                if(WIFEXITED(child_status)){
                    if(WEXITSTATUS(child_status) == 0)  printf("Done\n");
                    else printf("terminated with error code\t%i\n", WEXITSTATUS(child_status));
                }
                if(WIFSIGNALED(child_status)){
                    printf("exited via signal\t%i\n",WTERMSIG(child_status));
                }
            }

            if(segundo_plano){
                //Ejecuto en 2do plano
                append(&head,child_pid);
                printf("[%i] %i\n", last_job(&head), child_pid);  
            }
            else{
                //Ejecuto en 1er plano
                waitpid(child_pid, NULL, 0);
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