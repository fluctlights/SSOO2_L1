#=====================#
# PRACTICA 1 - SSOO 2 #
#=====================#

Para probar el programa:

    $ make
    $ ./manager

Tras esto, podremos detener el demonio con:

    $ killall daemon

Para ver los backups generados por el demonio hay que ir a /home/<usuario>/backup_carpetas.
Dentro estarán todos los backups con la fecha concreta.
Tambien se proporciona un log.txt para saber el flujo de ejecución del daemon.

En el directorio info_estudiantes estará presente toda la informacion acerca de los estudiantes,
cuyas carpetas son sus respectivos DNIs y dentro de las mismas tendremos el modelo de examen
y un .txt indicando la nota necesaria para hacer media.

Por otro lado para ver el log del manager, este esta alojado en manager_logs, donde tenemos el
log.txt que ha registrado la actividad de manager.

Para eliminar archivos innecesarios podemos hacer:

    $ make clean

