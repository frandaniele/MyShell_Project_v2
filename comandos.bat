echo Empezamos con el comando clear, esto no lo vas a ver en la terminal
clr
echo Hola $USER ! Esta es mi shell. Actualmente te encontrás en $PWD .
echo Estás por intentar cambiar a un directorio inexistente.
cd no_existe/
echo Como viste, seguís en $PWD . Ahora vas a ir a /home
cd /home
echo Estás en $PWD . Ahora volverás al directorio anterior.
cd -
echo Estás en $PWD . Hasta acá llegamos, chau!
quit
echo Esto nunca será ejecutado