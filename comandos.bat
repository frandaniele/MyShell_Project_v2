echo Empezamos con el comando clear, esto no lo vas a ver en la terminal
clr
echo Hola $USER ! Esta es mi shell. Actualmente te encontrás en $PWD .
echo Estás por intentar cambiar a un directorio inexistente.
cd no_existe/
echo Como viste, seguís en $PWD . Ahora vas a ir a /home
cd /home
echo Estás en $PWD . Ahora volverás al directorio anterior.
cd -
echo Estás en $PWD .
echo Ahora vas a invocar un programa en 2do plano
src/bin/app&
echo Como ves, puedes seguir ingresando comandos y se ejecutan inmediatamente
ps &
echo Ahora, invocas el mismo programa en primer plano
src/bin/app
ps
echo Este mensaje lo verás cuando termine de ejecutarse el programa anterior
echo Hasta acá llegamos, chau!