#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define PA 0
#define PB 1
#define PC 2
#define PD 4
#define LECTURA 0
#define ESCRITURA 1

//variables globales
pid_t pids[4];
int status;

//funcion lanzada tras producirse la señal SIGINT
void limpiarInformacion(int signo);

int main(int argc, char* argv[]) {
    
    //manejo de procesos
    int pid;
    bool cont = true;

    //tuberias de R/W
    int tuberia[2];
    char *descriptor_tuberia_lectura = malloc(sizeof(char)*256);
    char *descriptor_tuberia_escritura = malloc(sizeof(char)*256);
    char *mensaje = malloc(sizeof(char)*256);
    int nbytes_lectura;
    pipe(tuberia);
    
    //para el manejo de señal Ctrl + C, tendremos que borrar la informacion de los alumnos
    signal(SIGINT, limpiarInformacion);
    
    //Si los procesos PB y PC terminan correctamente, saldremos del programa
    //signal(SIGCHLD, terminar_programa);


    //Convertimos los descriptores de tuberia en char* para poder pasarlos por argumento en execl() de PC
    sprintf(descriptor_tuberia_escritura, "%d", tuberia[ESCRITURA]);
    sprintf(descriptor_tuberia_lectura, "%d", tuberia[LECTURA]);



    switch (pids[PA] = fork()) {
        case 0:
            if((execl("./pa", "pa","estudiantes.txt", NULL)) == -1) {
                printf("Error al hacer execl");
                exit(EXIT_FAILURE);
            }
    
        default:
            if(waitpid(pids[PA], &status, 0) == pids[PA]) {
                if(WIFEXITED(status)) { 
                    printf("Creacion de directorios finalizada\n"); 
                }
            }

            switch(pids[PB] = fork()) {
                case 0:
                    if((execl("./pb", "pb","estudiantes.txt", NULL)) == -1) {
                        printf("Error al hacer execl");
                        exit(EXIT_FAILURE);
                    }

                default:
                    switch(pids[PC] = fork()) {
                        case 0:
                            if((execl("./pc", "pc","estudiantes.txt", descriptor_tuberia_lectura, descriptor_tuberia_escritura, NULL)) == -1) {
                                printf("Error al hacer execl");
                                exit(EXIT_FAILURE);
                            }
                        
                        default:

                            //sincronizacion del proceso PB, asegurandonos de que terminamos el bucle 
                            while(cont) {
                                pid = wait(NULL);
                                if(pid == pids[PB]) {
                                    if(WIFEXITED(status)) { 
                                        cont = false;
                                    }   
                                }
                            } 

                            //la sincronizacion del proceso PC no es necesaria, porque usamos la directiva read
                            nbytes_lectura = read(tuberia[LECTURA], mensaje, (sizeof(char)*256)); 

                            /*TERMINACION DE PC*/

                            printf("Copiar modelos de examenes finalizado\n");
                            printf("Creacion de archivos con nota necesaria para alcanzar el corte finaliazdo\n"); 
   
                    }
            }

    }
    exit(EXIT_SUCCESS);
}

void limpiarInformacion(int signum) {

    //convertimos a char * los pids para pasarlos por argumentos a PD
    char pa[6];
    sprintf(pa, "%d", (int) pids[PA]);
    char pb[6];
    sprintf(pb, "%d", pids[PB]);
    char pc[6];
    sprintf(pc, "%d", pids[PC]);

    switch (pids[PD] = fork()) {
        case 0:
            if((execl("./pd", "pd", pa, pb, pc, NULL)) == -1) {
                printf("Error al hacer execl");
                exit(EXIT_FAILURE);
            }
    
        default:
            printf("Se ha recibido Ctrl + C, procediendo a terminar todo......\n");
            if(waitpid(pids[PD], &status, 0) == pids[PD]) {
                if(WIFEXITED(status)) { 
                    printf("Creacion de directorios finalizada\n"); 
                }
            }
    }
    exit(EXIT_SUCCESS);
}