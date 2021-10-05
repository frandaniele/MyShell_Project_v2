#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

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

/* Crea un proceso imagen y ejecuta program */
int spawn(char* program, char** arg_list);