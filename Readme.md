# Laboratorio N4 - Francisco Daniele
## My Shell
### SOI - FCEFyN - UNC
---
## Desarrollo
Para el desarrollo de este laboratorio utilice conceptos aprendidos en clases, ademas de ayudarme con información en la  web. Para empezar organice el esquema de archivos, cree el Makefile y los archivos base.
El esquema de archivos es el siguiente:
-   /: .gitignore, Makefile, batch files, Readme.
-   /src: 
    -   myshell.c: archivo principal de la shell
    -   utils.c: archivo con funciones auxiliares para el desarrollo del laboratorio
    -   app.c: archivo auxiliar para test de invocacion de programas
-   /src/include: archivos de headers 
-   /src/bin: binarios generados por el makefile 
    -   myshell: binario de la shell, union de myshell.c y utils.c
    -   app: binario del programa auxiliar.
## Actividades
#### 1. Command line prompt
Para la implementacion del prompt cree una funcion llamada _print_cmdline_prompt_ que recibe de parametros
3 arreglos para guardar el username, hostname y path actual. Para eso se sirve de las funciones _get_username_,
_get_hostname_ y _get_current_path_. Luego mediante la funcion _getenv_ recibe el directorio de la variable de entorno
$HOME para que si es parte del directorio en el que me encuentro no lo muestre, y si es donde me encuentro lo reemplaza
por _~_.

#### 2. Internal commands
_myshell_ soporta algunos comandos internos. Para identificar cual es y hacer la accion correspondiente está la funcion 
_identificar_cmd_ que recibe un string y si lo reconoce como comando llama la funcion de ese comando, si es un string vacio no hace nada y en cualquier otro caso trata de invocar como un programa al input.
-   cd: implementado en _cambiar_dir_. Esta funcion inspecciona el string recibido y si es "-" mediante _getenv_ y _setenv_ cambia swapea PWD con OLDPWD (cambia del directorio actual al anterior) y en caso que el ultimo no este seteado reporta el error y se mantiene en el directorio que estaba.
Si el string recibido es otra cosa llama a la funcion _chdir_ que se encarga de cambiar el directorio actual por el que le pasemos. En caso de que no existe o no este presente lo reporta y se queda en el que estaba, sino cambia el directorio actual de trabajo, setea la variable OLDPWD con el directorio anterior y PWD con el directorio actual obtenido con _getcwd_.
-   clr: La limpieza de la consola de la terminal se realiza mediante un _printf_ con la expresion regular _"\033[3J\033[H\033[2J"_ que borra los caracteres y vuelve el cursor al principio.
-   echo: recibe un string y lo va recorriendo palabra a palabra (separadas por " ") mediante _strtok_, se fija en cada una si está precedida por un '$' y si es así imprime si encuentra esa variable de entorno y sino imprime la palabra que sea. Tambien reemplaza todos los espacios y tabs por uno solo.
-   quit: imprime un mensaje de despedida y mediante _exit_ realiza una salida limpia con codigo de salida '0'.

#### 3. Program invocation
Cuando el input de usuario no es un comando interno se trata de invocar como programa. Lo implemente con la funcion _invocar_. Esta recibe un string, si no comienza con '\' se lo agrega y crea una lista de hasta 4 argumentos. El primero es el nombre del programa por si solo, lo identifica mediante _strrchr_, y en el caso de que haya más (hasta 3) los guarda en _arg_list_. Luego identifica si se llamo con '&' para ejecutar en segundo plano y en tal caso elimina ese char del string y pone en '1' un flag. Acto seguido se llama la funcion _spawn_ con el nombre del programa (path con el que fue invocado), _arg_list_ el flag de 2do plano y la cantidad de argumentos.
La funcion _spawn_ se encarga de llamar a _fork_ y luego segun lo que retorne:
-   '-1': error en fork, lo reporta y realiza salida limpia.
-   '0': es el child. Se sirve de la funcion _ejecutar_ que se encarga de llamar a _execl_ segun la cantidad de argumentos con que se haya invocado al programa. Primero lo intenta con el path que se haya pasado, si es absoluto se ejecutará el programa. Luego concatena el path con directorios de Linux donde se suelen encontrar los ejecutables e intenta ejecutarlo. Si no tuvo exito prueba con el path en que se encuentra, y si tampoco hubo exito reporta que no se encontro el programa y realiza una salida limpia con codigo de salida '1'.
-   default: es el parent. Se encarga de limpiar procesos zombies si los hay y si se invoco el programa en primer plano espera a que finalice, y si no imprime la informacion del proceso en segundo plano (job y pid).

#### 4. Batch file
Si se invoca a _myshell_ con un batch file como argumento se llama a la funcion _leer_batchfile_ la cual se encarga de leer linea a linea los comandos en el archivo y ejecutarlos. Cuando termina se sale de la shell.

#### 5. Background execution
Explicado en el apartado 3.