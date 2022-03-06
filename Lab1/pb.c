#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


int main(int argc, char* argv[]) {
    
    //abrir fichero
    FILE *estudiantes_archivo = fopen(argv[1], "r");

    //si hay algun error al abrir el archivo salimos
    if(estudiantes_archivo == NULL){
        printf("El fichero %s NO existe!", argv[1]);
        exit(EXIT_FAILURE);
    }

    //variables necesarias para el manejo de fichero
    char linea_fichero[256];
    char *linea_fichero_dividida; //esta variable tendra cada linea cortada
    int i = 0;
    char *dni_alumno;
    char *modelo_examen;

    //variable para cargar el comando que copiara los modelos de examen en los directorios
    char *comando = malloc(sizeof(char)*256);
    
    //revisaremos cada linea para sacar el DNI
    while(fgets(linea_fichero, 100, estudiantes_archivo) != NULL) {

        linea_fichero_dividida = strtok(linea_fichero, " ");
        i = 0; //restablecemos la posicion de i para poder manejar cada trozo de linea

        while(linea_fichero_dividida != NULL) {
            while(i < 3) { //tenemos 3 partes en cada linea de fichero (0,1,2)
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

            //genero el comando
            sprintf(comando, "cp ./MODELOSEXAMEN/MODELO");
            
            //le meto el modelo de examen necesario
            strcat(comando, modelo_examen);
            strcat(comando, ".pdf");

            //ahora añado el directorio destino de la copia
            strcat(comando, " ./carpetas/");
            strcat(comando, dni_alumno);
            strcat(comando, " >/dev/null 2>&1");
            
            //ejecuto en shell
            system(comando);

            //pruebas para PD
            pause();
        }
    }

    fclose(estudiantes_archivo);
    //salimos, ya hemos terminado
    exit(EXIT_SUCCESS);
}