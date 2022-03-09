#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define BORRAR_INFO "rm -R ./info_estudiantes"

int main(int argc, char* argv[]) {
    
    /*para recogida de procesos*/
    int i=0;
    pid_t pids[3];
    
    /*recoger pa, pb, pc de argv*/
    for(i=0; i<3; i++) {
        pids[i] = atoi(argv[i+1]);
    }

    /*terminamos los procesos PB y PC, de la tabla de procesos*/
    kill(pids[2], SIGINT);
    kill(pids[3], SIGINT);

    /*borrar informacion de los alumnos (./carpetas)*/
    system(BORRAR_INFO);

    /*salimos*/
    exit(EXIT_SUCCESS);
}