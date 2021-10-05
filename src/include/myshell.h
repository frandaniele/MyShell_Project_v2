#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h> 
#include <sys/wait.h>

/**
  @file myshell.h
  @brief Archivo de headers para la implementación de mi propia shell.

  @author Francisco Daniele
  @date 10/2021
*/

/* Imprime el command line prompt */
void print_cmdline_prompt(char* username, char* hostname, char* current_path);

/* Identifica los comandos recibidos y ejecuta la accion necesaria  */
void identificar_cmd(char* cmd);

/* Invoca (o trata) el input del usuario que no es un cmd interno  */
int invocar(char* program);

/* Simil a comando echo de Linux  */
void eco(char* cmd);

/* Cambia el directorio actual */
int cambiar_dir(char* dir);

/* Lee instrucciones secuenciales desde un batch file */
int leer_batchfile(char* file);