# Laboratorio N5 SOI - Francisco Daniele - FCEFyN - UNC
## My Shell advance
---
---
### Desarrollo
---
Este trabajo consistía en realizar un fork del laboratorio 4 y agregarle funcionalidades para luego hacer un pull request al repo original emulando un workflow de contribución apropiado. La funcionalidad nueva debía permitir a la shell realizar redireccion de input y output, soportar pipes e IPC y manejar algunas señales.

---
---
### Actividades
---
#### 1. Signal Handling
Para esta consigna creé una función (en _src/utils.c_) que se llama _instalar_signals_ que recibe un ___sighandler_t_ y lo designa como handler de las señales SIGQUIT, SIGTSTP y SIGINT mediante la función de la libreria _signal.h_ _signal_. Para que _myshell_ ignore dichas señales cuando no hay otro proceso en primer plano, al comienzo del programa les designo el handler _SIG_IGN_, luego cada vez que se ejecuta algún proceso en primer plano les designo un handler propio _enviar_signal_ que recibe como argumento el n° de la señal y se la envia al proceso que está en ejecución actualmente mediante la funcion _kill_. Previamente, para que el proceso en ejecución reaccione a esas señales le designe como handler _SIG_DFL_.

---
#### 2. Pipe
Para este apartado primero detecto en el input del usuario si hay algún "|" para ver si hay que crear un pipe. De ahí se llama a la función _tuberia_ que recibe como argumento el comando ingresado y mediante la función _obtener_io_ guarda en un buffer los programas a ejecutar con sus argumentos y devuelve la cantidad de programas. Si es menor a 2 imprime un mensaje de error y retorna porque no estarían los dos lados del pipe. Luego con la función _obtener_args_ creamos una lista de punteros a strings "null terminated" con el nombre de cada programa y sus argumentos que será lo que se enviará a la funcion _execvp_. Estas listas obtenidas se guardan en un puntero a un puntero a strings también null terminated para ir realizando las sucesivas pipes necesarias. En este momento es que se llama la funcion _spawn_pipe_ que recibe como argumento este puntero doble a strings y la cantidad de programas a ejecutar sucesivamente. Esta funcion realiza un fork en el cual el padre se queda esperando a que se termine toda la pipe y el hijo crea un pipe y ejecuta otro fork en un while mientras haya procesos a ejecutar. El hijo, ahora padre, se queda esperando que el "nieto" ejecute, tomando como stdin el stdout que el proceso anterior del pipe lleno con informacion. Esto ultimo se hace con la funcion _dup2_ con el hijo/padre guardando su read end del pipe (donde escribio su hijo) para el proximo proceso, y el nieto/hijo abriendo como su stdin el file descriptor de input viejo, y si aun queda algun programa a ejecutar hace _dup2_ sobre su stdoutput para que lo sea su read end del pipe. Una vez que no quedan procesos a ejecutar, el proceso hijo/nieto hace un _exit_ y _myshell_ vuelve a el tomar control, regresa y libera la memoria alocada por los punteros al principio.

#### 2.1 ¿Dónde se encuentran los pipes en el filesystem, qué atributos tienen?
Los pipes al no ser precisamente archivos no se encuentran en el filesystem. Lo que si tienen son file descriptors para realizar la lectura/escritura. Si son FIFOs, es decir "named pipes" tienen un punto de acceso en el _fs_ que será su filename.

Atributos:
-   Buffer de datos de tamaño fijo.
-   Read-end y write-end. Existe la posibilidad de ser bidireccionales aunque no es recomendable.
-   Lectura de read-end vacio resulta en bloqueo del proceso hasta que haya algo que leer.
-   Lo mismo con escritura de write-end lleno: bloqueo hasta que haya lugar.
-   Permiten evitar operaciones I/O en disco para las comunicaciones y así mejorar la performance.

---
#### 3. I/O redirection 
Para esto primero se identifica en el comando ingresado por el usuario si hay alguna redireccion, es decir si se encuentran los simbolos "<" o ">" y tambien si es de programas comunes o del comando interno _echo_. En ese momento se llama a la funcion _redireccionar_ que recibe el comando y un flag que indica si viene de _echo_ o no.
#### Redireccion de programas
-   Redireccion de entrada: se realiza con la funcion _redireccion_entrada_ que obtiene el programa que se va a ejecutar y el archivo que usa como entrada y mediante la funcion _add_inputfile_ invoca a dicho programa con su correspondiente input mediante la funcion _invocar_ del laboratorio anterior.
-   Redireccion de salida. Se usa la funcion _redireccion_salida_ que recibe el programa y un flag de si se debe sobre escribir el programa o concatenar la salida. Obtiene el programa a ejecutar y el archivo donde redireccionar el output (con _obtener_io_) y mediante _reemplazar_stdout_ se fija si hay que sobreescribir o concatenar y segun eso, mediante _freopen_ reemplaza stdout con el archivo en el modo "w" o "a+". Ahí se invoca el programa, y cuando retorna se vuelve a redireccionar la salida a la consola mediante _redireccionar_a_consola_.
-   Redireccion doble: Se usa la funcion _redireccion_doble_ que recibe el programa y un flag de si se debe sobre escribir el programa o concatenar la salida. Primero encuentra cual es el programa, el input y el output pudiendo venir de las formas "prog < in > out" o "prog > out < in" mediante _obtener_io_ y con ayuda de _strchr_ para detectar que forma fue la ingresada. Después se ejecuta _reemplazar_stdout_ con el output obtenido, y se hace _add_inputfile_ con el input donde se invoca al programa. Al volver de la invocacion se redirecciona el output a consola y se libera la memoria alocada al principio.
#### Redireccion de echo
Si se llama _redireccionar_ _flag_eco_ = 1 esta funcion detecta que tipo de redireccion se realiza, y mediante _read_text_file_ si hay que redireccionar la entrada lee el archivo input y luego se llama a la funcion del comando intero _eco_ con el texto obtenido, y se imprime o en consola, o si se redirigió la salida mediante _reemplazar_stdout_, en el output pertinente.

---
---