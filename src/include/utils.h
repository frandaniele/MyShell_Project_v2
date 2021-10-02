#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
  @file utils.h
  @brief Archivo de headers de funciones de proposito general
    para los distintos laboratorios de Sistemas Operativos I.

  @author Francisco Daniele
  @date 9/2021

*/

/* Lee texto de un archivo y lo guarda en un buffer de tamaño size  */
int read_text_file(char *directory, int size, char *buffer);

/* Itera con la funcion strtok n veces dividiendo str por separator para encontrar alguna palabra */
char* busca_palabras(char* str, char* separator, int n);

/* Reemplaza ch de string por '\0' */
void reemplazar_char(char* string, char ch);