#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

/**
  @file utils.h
  @brief Archivo de headers de funciones de proposito general
    para los distintos laboratorios de Sistemas Operativos I.

  @author Francisco Daniele
  @date 9/2021

*/

struct Node{
    pid_t pid;
    int n_job;
    struct Node *next;
};

/* Lee texto de un archivo y lo guarda en un buffer de tamaño size  */
int read_text_file(char *directory, int size, char *buffer);

/* Reemplaza ch de string por '\0' */
void reemplazar_char(char* string, char ch);

/* Muestra un menu de ayuda */
void help_menu(FILE* stream, int exit_code);

/* Obtienen los datos del usuario que está ejecutando la shell 
   y se los pasan a un string */
void get_env_var(char* dst, char* var);
void get_hostname(char* dst);

/* Crea un proceso imagen y ejecuta program */
int spawn(char* program, char** arg_list, int segundo_plano, int cant_args);

/* Detecta el ampersand que determina la ejecucion en 2do plano */
int identificar_seg_plano(char* str);

/* Ejecuta el programa  */
void ejecutar(char* program, char** arg_list, int cant_args, char* path);

void append(struct Node** head_ref, pid_t pid);

void deleteNode(struct Node** head_ref, pid_t pid);

int last_job(struct Node** head_ref);

int pid_job(struct Node** head_ref, pid_t pid);