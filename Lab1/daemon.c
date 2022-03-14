#include <stdio.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define COMANDO "cp -r ./info_estudiantes /home/$(whoami)/backup_carpetas/backup_carpetas_"
#define CREAR_CARPETA_BACKUP "mkdir /home/$(whoami)/backup_carpetas >/dev/null 2>&1"
#define CREAR_LOGS "touch /home/$(whoami)/backup_carpetas/log.txt >/dev/null 2>&1"

/*manejo de señal SIGTERM*/
void terminar_demonio(int signum);

/*variables globales - manejo de la hora*/
time_t t;
struct tm *tm;

int main(int argc, char* argv[]) {
    
    char *fechahora = malloc(sizeof(char)*15);
    char *comando = malloc(sizeof(char)*100);

    /*creando carpeta y archivo de logs para las backups*/
    system(CREAR_CARPETA_BACKUP);
    system(CREAR_LOGS);

    /*para cuando se de la orden de parar el proceso con ps -aux*/
    signal(SIGTERM, terminar_demonio);

    /*
        En este bucle esperamos 60seg, recogemos la hora exacta y creamos
        una copia backup de la informacion de los estudiantes con esa hora
        concreta
    */
    while(1) {
        sleep(60);
        sprintf(comando, COMANDO);
        printf("HOLA");
        t = time(NULL);
        tm = localtime(&t);
        strftime(fechahora, 15, "%H-%M-%S", tm);
        strcat(comando, fechahora);
        system(comando);
        free(fechahora);
    }
}

void terminar_demonio(int signum) {
    char *fechahora = malloc(sizeof(char)*15);
    char *mensaje_log = malloc(sizeof(char)*100);
    switch (signum) {
        case SIGTERM:
            t = time(NULL);
            tm = localtime(&t);
            strftime(fechahora, 15, "%H-%M-%S", tm);
            sprintf(mensaje_log, "echo \"Me han pedido terminar! Hora: \"%s >> /home/$(whoami)/backup_carpetas/log.txt", fechahora);
            system(mensaje_log);
            free(fechahora);
            free(mensaje_log);
            exit(EXIT_SUCCESS);
    }
}
