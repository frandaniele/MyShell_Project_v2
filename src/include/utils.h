#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/**
  @file utils.h
  @brief Archivo de headers de funciones de proposito general
    para los distintos laboratorios de Sistemas Operativos I.

  @author Francisco Daniele
  @date 9/2021

*/

/* Lee texto de un archivo y lo guarda en un buffer de tamaño size  */
int read_text_file(char *directory, int size, char *buffer);

/* Reemplaza ch de string por '\0' */
void reemplazar_char(char* string, char ch);

/* Muestra un menu de ayuda */
void help_menu(FILE* stream, int exit_code);

/* Obtienen los datos del usuario que está ejecutando la shell 
   y se los pasan a un string */
void get_username(char* dst);
void get_hostname(char* dst);
void get_current_path(char* dst);

/* Crea un proceso imagen y ejecuta program */
int spawn(char* program, char** arg_list, int segundo_plano);

/* Detecta el ampersand que determina la ejecucion en 2do plano */
int identificar_seg_plano(char* str);

/* Imprime informacion del proceso ejecutando en 2do plano  */
void segundo_plano_info(pid_t pid);