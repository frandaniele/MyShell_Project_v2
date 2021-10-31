#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <signal.h>

/**
  @file utils.h
  @brief Archivo de headers de funciones de proposito general
    para los distintos laboratorios de Sistemas Operativos I.

  @author Francisco Daniele
  @date 9/2021

*/

/*  Estructura para lista enlazada de procesos en background 
    Tiene un pid y un numero de job */
typedef struct Node{
    pid_t pid;
    int n_job;
    struct Node *next;
} Node;

/* Lee texto de un archivo y lo guarda en un buffer de tamaño size  */
int read_text_file(char *directory, int size, char *buffer);

/* Reemplaza ch de string por '\0' */
void reemplazar_char(char* string, char ch);

/* Elimina los espacios en blanco de un string */
char* trimwhitespace(char *str);

/* Muestra un menu de ayuda */
void help_menu(FILE* stream, int exit_code);

/* Obtienen los datos del usuario que está ejecutando la shell 
   y se los pasan a un string */
void get_env_var(char* dst, char* var);
void get_hostname(char* dst);

/* Crea una lista null terminated con el nombre de un proceso 
  y los argumentos con que será llamado */
char** obtener_args(char* src);

/* Se encarga de limpiar procesos zombies si los hay */
void limpiar_zombies();

/* Crea un proceso imagen y ejecuta program */
int spawn(char* program, char** arg_list, int segundo_plano, int cant_args);

/* Crea tantas pipes como n_processes e invoca a los mismos */
int spawn_pipe(char*** processes, int n_processes);

/* Detecta el ampersand que determina la ejecucion en 2do plano */
int identificar_seg_plano(char* str);

/* se encarga de instalar los signal handlers necesarios
  y espera por el proceso con pid que está en primer plano */
void esperar_proceso(pid_t pid, int status);

/* Ejecuta el programa  */
void ejecutar(char* program, char** arg_list, int cant_args, char* path);

/*  Agrega nodos con un pid y n_job secuencial a una lista enlazada */
void append_nodo(Node** head_ref, pid_t pid);

/*  Elimina un nodo segun su pid*/
int eliminar_nodo(Node** head_ref, pid_t pid);

/*  Devuelve el n_job del ultimo nodo en la lista */
int last_job(Node** head_ref);

/*  Detecta si se debe redireccionar  */
int hay_redireccion(char* command);

/* Divide cmd segun ch y guarda cada string en programs */
int obtener_io(char* cmd, char** programs, char* ch);

/* reemplaza el stdout por file en el modo necesario */
int reemplazar_stdout(char* file, int append);

/* redirecciona output a la consola */
void redireccionar_a_consola();

/* instala a SIG_QUIT|TSTP|INT el handler s */
void instalar_signals(__sighandler_t s);