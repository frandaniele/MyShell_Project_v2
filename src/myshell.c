#include "include/myshell.h"
#include "include/utils.h"

int main(int argc, char **argv){
    
    int next_option;
    int h = 0;
    
    const char* const short_options = "h"; 

    const struct option long_options[] = {
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, 0 } 
    };

    do
    {
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);
        switch (next_option)
        {
        case 'h': 
            h = 1;
            break;
        case '?': 
            break;
        case -1: 
            break;
        default: 
            printf("Unexpected error.\n");
            return 1;
        }
    }while (next_option != -1); 
    
    if(h){
        help_menu();
    }

    char username[32];
    char hostname[32];
    char path[256];

    print_cmdline_prompt(username, hostname, path);

    return 0;
}

void help_menu(){
    
    printf("-h --help       Despliega el menú de ayuda\n");

}

int get_username(char* dst){

    strcpy(dst,getenv("USERNAME"));

    if(dst == NULL){
        printf("Error al buscar el username.\n");
        exit(-1);
    }

    return 0;
}

int get_hostname(char* dst){
    read_text_file("/proc/sys/kernel/hostname", 32, dst);

    dst = strtok(dst, "\n");
    if(dst == NULL){
        printf("Error al buscar el hostname.\n");
        exit(-1);
    }

    return 0;
}

int get_current_path(char* dst){

    strcpy(dst,getenv("PWD"));

    if(dst == NULL){
        printf("Error al buscar el path actual.\n");
        exit(-1);
    }

    return 0;
}

void print_cmdline_prompt(char* username, char* hostname, char* current_path){
    get_username(username);
    get_hostname(hostname);
    get_current_path(current_path);
    printf("\n%s@%s:~%s$ ", username, hostname, current_path);
}