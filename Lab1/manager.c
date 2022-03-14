#include <stdio.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <signal.h>

/*constantes para los procesos*/
#define PA 0
#define PB 1
#define PC 2
#define PD 4

/*constantes para la tuberia creada: indice 0 e indice 1 (int tuberia[2])*/
#define LECTURA 0
#define ESCRITURA 1

/*constantes para el daemon a crear*/
#define DAEMON "./daemon >/dev/null 2>&1 &"

/*constantes para el log.txt de manager*/
#define RUTA_LOGS "./manager_logs/log.txt"
#define BORRAR_LOGS "rm ./manager_logs/log.txt"
#define ARCHIVO_LOGS "mkdir ./manager_logs >/dev/null 2>&1 && touch ./manager_logs/log.txt >/dev/null 2>&1"
#define TITULO_LOGS "echo \"******** Log del sistema ********\" >> ./manager_logs/log.txt"

/*mensajes de finalizacion de cada proceso*/
#define MENSAJE_PA "Creación de directorios finalizada."
#define MENSAJE_PB "Copia de modelos de examen, finalizada."
#define MENSAJE_PC "Creación de archivos con nota necesaria para alcanzar la nota de corte, finalizada."
#define MENSAJE_PD "Se ha producido una interrupción voluntaria ordenada por el propio usuario"

/*variables globales*/
pid_t pids[4];

/*funcion lanzada tras producirse la señal SIGINT*/
void limpiarInformacion(int signo);

/*comprobar si existe el fichero log.txt*/
int existeArchivo(char *archivo);

int main(int argc, char* argv[]) {
    
    /*manejo de procesos*/
    int pid;
    int i = 0;
    int status;

    /*tuberias de R/W*/
    int tuberia[2];
    char *descriptor_tuberia_lectura = malloc(sizeof(char)*256);
    char *descriptor_tuberia_escritura = malloc(sizeof(char)*256);
    char *mensaje_recibido = malloc(sizeof(char)*256);
    char *mensaje_notaclase = malloc(sizeof(char)*256);
    int nbytes_lectura=0;
    pipe(tuberia);
    
    /*comandos para el log de manager*/
    char *mensaje_pa = malloc(sizeof(char)*256);
    sprintf(mensaje_pa, "echo \"%s\" >> %s", MENSAJE_PA, RUTA_LOGS);
    char *mensaje_pb = malloc(sizeof(char)*256);
    sprintf(mensaje_pb, "echo \"%s\" >> %s", MENSAJE_PB, RUTA_LOGS);
    char *mensaje_pc = malloc(sizeof(char)*256);
    sprintf(mensaje_pc, "echo \"%s\" >> %s", MENSAJE_PC, RUTA_LOGS);

    /*para el manejo de señal Ctrl + C, tendremos que borrar la informacion de los alumnos*/
    signal(SIGINT, limpiarInformacion);
    
    /*Convertimos los descriptores de tuberia en char* para poder pasarlos por argumento en execl() de PC*/
    sprintf(descriptor_tuberia_escritura, "%d", tuberia[ESCRITURA]);
    sprintf(descriptor_tuberia_lectura, "%d", tuberia[LECTURA]);

    /*borramos logs previos del sistema*/
    if((existeArchivo(RUTA_LOGS) == 1)) { system(BORRAR_LOGS); }
    
    /*comenzamos a rellenar el archivo de logs*/
    system(ARCHIVO_LOGS);
    system(TITULO_LOGS);

    /*arrancamos el daemon*/
    system(DAEMON);

    switch (pids[PA] = fork()) {
        case 0:
            if((execl("./pa", "pa","estudiantes.txt", NULL)) == -1) {
                printf("Error al hacer execl");
                exit(EXIT_FAILURE);
            }
    
        default:
            if(waitpid(pids[PA], &status, 0) == pids[PA]) {
                if(WIFEXITED(status)) { 
                    system(mensaje_pa); 
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

                            /*sincronizacion del proceso PB, asegurandonos de que terminamos el bucle*/
                            while(i<1) {
                                pid = wait(NULL);
                                if(pid == pids[PB])
                                    if(WIFEXITED(status)) {
                                        system(mensaje_pb);
                                        i++; 
                                    }  
                            } 

                            /*la sincronizacion del proceso PC no es necesaria, porque usamos la directiva read*/
                            read(tuberia[LECTURA], mensaje_recibido, (sizeof(char)*256)); 
                            system(mensaje_pc);
                            sprintf(mensaje_notaclase, "echo \"%s\" >> %s", mensaje_recibido, RUTA_LOGS);
                            system(mensaje_notaclase);
                    }
            }

    }

    /*liberacion de recursos*/
    free(mensaje_recibido);
    free(mensaje_pa);
    free(mensaje_pb);
    free(mensaje_pc);
    free(descriptor_tuberia_escritura);
    free(descriptor_tuberia_lectura);
    free(mensaje_notaclase);

    /*terminamos*/
    exit(EXIT_SUCCESS);
}

void limpiarInformacion(int signum) {

    /*construimos el comando para mandar el mensaje a log.txt*/
    char *mensaje_pd = malloc(sizeof(char)*256);
    sprintf(mensaje_pd, "echo \"%s\" >> %s", MENSAJE_PD, RUTA_LOGS);

    /*convertimos a char * los pids para pasarlos por argumentos a PD*/
    char pa[6];
    sprintf(pa, "%d", (int) pids[PA]);
    char pb[6];
    sprintf(pb, "%d", pids[PB]);
    char pc[6];
    sprintf(pc, "%d", pids[PC]);
    int status;

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
                    system(mensaje_pd);
                }
            }
    }
    free(mensaje_pd);
    exit(EXIT_SUCCESS);
}

int existeArchivo(char* archivo) {
    struct stat buffer;
    int existe = stat(archivo, &buffer);
    if(existe == 0)
        return 1;
    else  
        return 0;
}