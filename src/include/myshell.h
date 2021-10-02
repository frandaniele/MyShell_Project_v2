#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include<unistd.h> 

/**
  @file myshell.h
  @brief Archivo de headers para la implementación de mi propia shell.

  @author Francisco Daniele
  @date 10/2021
*/

/* Muestra un menu de ayuda */
void help_menu();

/* Obtienen los datos del usuario que está ejecutando la shell 
   y se los pasan a un string */
int get_username(char* dst);
int get_hostname(char* dst);
int get_current_path(char* dst);

/* Imprime el command line prompt */
void print_cmdline_prompt(char* username, char* hostname, char* current_path);

/* Identifica los comandos recibidos y ejecuta la accion necesaria  */
void identificar_cmd(char* cmd);

/* Invoca (o trata) el input del usuario que no es un cmd interno  */
void invocar(char* program);

/* Simil a comando echo de Linux  */
void eco(char* cmd);

/* Cambia el directorio actual */
void cambiar_dir(char* dir);