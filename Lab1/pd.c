#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define CIFRAS_DNI 8

int main(int argc, char* argv[]) {
    
    int i=0;
    pid_t pids[3];
    //variables para guardar el comando que borra la informacion
    char *comando = "rm -R ./carpetas";

    //terminamos los procesos PB y PC, de la tabla de procesos
    kill(pids[1], SIGUSR1);
    kill(pids[2], SIGUSR1);

    //borrar informacion de los alumnos (./carpetas)
    system(comando);
    //salimos, ya hemos terminado
    exit(EXIT_SUCCESS);
}