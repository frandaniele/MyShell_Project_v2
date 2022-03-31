#include <stdio.h>
#include <unistd.h>

int main(int argc, char **arg){

    printf("Soy un archivo para probar la invocacion de programas.\n");
    printf("Ojalá tener exito. Voy a esperar un ratito.\n");
    sleep(10);
    printf("Me canse de esperar. Chau\n");
    printf("bye bye");

    return 0;
}