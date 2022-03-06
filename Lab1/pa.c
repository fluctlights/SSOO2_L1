#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define CIFRAS_DNI 8

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
    int i = 0;

    //con esta variable obtendremos la subcadena que contiene el DNI como tal
    char *dni_alumno = malloc(sizeof(char)*10);

    //variable para cargar el comando de generar las subcarpetas
    char *comando = malloc(sizeof(char)*256);
    
    //revisaremos cada linea para sacar el DNI
    while(fgets(linea_fichero, 100, estudiantes_archivo) != NULL) {
        
        //recorro las 8 primeras posiciones de cada linea, que contienen el DNI
        //almaceno cada posicion en dni_alumno, que contendra el DNI
        for(i=0; i<CIFRAS_DNI; i++) {
            dni_alumno[i] = linea_fichero[i];
        }
        
        //creacion de subdirectorio para no interferir con el directorio de los programas
        //genero el comando para crear el directorio con el DNI
        sprintf(comando, "mkdir ./carpetas/");

        //lo ejecuto en shellconcatenando comando con dni_alumno, que posee el DNI como tal
        strcat(comando, dni_alumno);
        strcat(comando, " >/dev/null 2>&1");
        system(comando);
    }

    fclose(estudiantes_archivo);
    free(dni_alumno);
    free(comando);

    //salimos, ya hemos terminado
    exit(EXIT_SUCCESS);
}