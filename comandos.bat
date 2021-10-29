echo Empezamos con el comando clear, esto no lo vas a ver en la terminal
clr
echo < bienvenida.txt
echo Estás por intentar cambiar a un directorio inexistente.
cd no_existe/
echo Como viste, seguís en $PWD. Ahora vas a ir a /home
cd /home
echo Estás en $PWD. Ahora volverás al directorio anterior.
cd -
echo Estás en $PWD.
echo Ahora vas a invocar un programa en 2do plano
src/bin/app&
sleep 1
echo Como ves, puedes seguir ingresando comandos (en este caso ps) y se ejecutan inmediatamente
ps &
echo Ahora, invocas el mismo programa (app) en primer plano
src/bin/app
ps
echo Este mensaje lo verás cuando termine de ejecutarse el programa anterior
echo Ahora un ejemplo de redireccion doble
wc < src/app.c > redir.txt
sleep 10
echo Veremos unos ejemplos de pipe
sleep 1
src/bin/app | grep esperar | sort | wc
grep bash /etc/passwd | cut -d : -f 1 | sort -r
echo Hasta acá llegamos, chau! >> despedida.txt
quit