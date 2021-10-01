#include "include/myshell.h"

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

    return 0;
}

void help_menu(){
    
    printf("-h --help       Despliega el menú de ayuda\n");

}