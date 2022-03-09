#include <stdio.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

/*señal SIGINT*/
void manejador(int signum);

int main(int argc, char* argv[]) {
    
    /*abrir fichero*/
    FILE *estudiantes_archivo = fopen(argv[1], "r");

    /*si hay algun error al abrir el archivo salimos*/
    if(estudiantes_archivo == NULL){
        printf("El fichero %s NO existe!", argv[1]);
        exit(EXIT_FAILURE);
    }

    /*Para poder captar la señal*/
    signal(SIGINT, manejador);

    /*variables para el log de sistema*/
    char *mensaje_pb = malloc(sizeof(char)*70);

    /*variables necesarias para el manejo de fichero*/
    char linea_fichero[256];
    char *linea_fichero_dividida; /*esta variable tendra cada linea cortada*/
    int i = 0;
    char *dni_alumno;
    char *modelo_examen;

    /*variable para cargar el comando que copiara los modelos de examen en los directorios*/
    char *comando = malloc(sizeof(char)*256);
    
    /*revisaremos cada linea para sacar el DNI*/
    while(fgets(linea_fichero, 100, estudiantes_archivo) != NULL) {

        linea_fichero_dividida = strtok(linea_fichero, " ");
        i = 0; /*restablecemos la posicion de i para poder manejar cada trozo de linea*/

        while(linea_fichero_dividida != NULL) {
            while(i < 3) { /*tenemos 3 partes en cada linea de fichero (0,1,2)*/
                switch(i) {
                    case 0:
                        dni_alumno = linea_fichero_dividida;
                        break;
                    case 1:
                        modelo_examen = linea_fichero_dividida;
                        break;
                }
                i++;
                linea_fichero_dividida = strtok(NULL, " ");
            }

            /*comeinzo a generar el comando*/
            sprintf(comando, "cp ./MODELOSEXAMEN/MODELO");
            
            /*añado el modelo de examen necesario al comando*/
            strcat(comando, modelo_examen);
            strcat(comando, ".pdf");

            /*añado el directorio destino de la copia al comando*/
            strcat(comando, " ./info_estudiantes/");
            strcat(comando, dni_alumno);
            strcat(comando, " >/dev/null 2>&1");
            
            /*ejecuto*/
            system(comando);
        }
    }

    /*cerramos fichero - muy importante*/
    fclose(estudiantes_archivo);

    /*pruebas para PD*/
    /*sleep(50);*/

    /*salimos, ya hemos terminado*/
    exit(EXIT_SUCCESS);
}

void manejador(int signum) {
    switch(signum) {
        case SIGINT:
            //printf("Soy PB y PD me ha ordenado salir, asi que salgo....\n");
            exit(EXIT_SUCCESS);
    }
}