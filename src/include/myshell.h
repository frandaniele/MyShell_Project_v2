#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

void help_menu();

int get_username(char* dst);

int get_hostname(char* dst);

int get_current_path(char* dst);

void print_cmdline_prompt(char* username, char* hostname, char* current_path);

void identificar_cmd(char* cmd);