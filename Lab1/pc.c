#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include<sys/stat.h>

#define NOTA_NECESARIA "La nota que debes obtener en este nuevo examen para superar la prueba es "
#define NOTA_GLOBAL "La nota media de la clase es: "
#define NOTA_MAX 10

void comandosNecesarios(char* dni, char* modelo, char* nota);
int leerLineas(char *archivo);
int existeArchivo(char *archivo);

int main(int argc, char* argv[]) {
    
    //sacar lineas de fichero, para saber los estudiantes que hay
    int n_estudiantes = leerLineas("estudiantes.txt");
    
    //cerramos la tuberia de lectura, solo vamos a escribir...
    //close(atoi(argv[2]));

    //Variables para guardar nota acumulada de todos los estudiantes, para hacer la media luego
    double nota_acumulada;
    char *nota_clase = malloc(sizeof(char)*256);
    char *buffer = malloc(sizeof(char)*256);

    //abrir fichero
    FILE *estudiantes_archivo = fopen("estudiantes.txt", "r");

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
    char *nota_examen;
    
    //revisaremos linea por linea
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
                    case 2:
                        nota_examen = linea_fichero_dividida;
                        break;       
                }
                i++;
                linea_fichero_dividida = strtok(NULL, " ");
            }

        //ejecutamos los comandos necesarios para cada usuario
        comandosNecesarios(dni_alumno, modelo_examen, nota_examen);

        //metemos la nota cada alumno dentro de la acumulada
        nota_acumulada += atoi(nota_examen); 

        //reiniciamos los valores
        dni_alumno="";
        modelo_examen="";
        nota_examen=""; 

        }
    }
    //cerramos el achivo
    fclose(estudiantes_archivo);

    //hacemos media de la clase
    nota_acumulada = (nota_acumulada/n_estudiantes);
    sprintf(buffer, "%1.2f", nota_acumulada);
    sprintf(nota_clase, NOTA_GLOBAL);
    strcat(nota_clase, buffer);

    //pruebas
    printf("%s\n", nota_clase);
    //exit(0); 

    //pasamos la nota media global al manager con una tuberia

    write(atoi(argv[3]), nota_clase, sizeof(char)*256);
    //salimos, ya hemos terminado
    exit(EXIT_SUCCESS);
}

void comandosNecesarios(char *dni, char* modelo_examen, char* nota) {
    
    char *comando = malloc(sizeof(char)*256);
    char *comando_touch = malloc(sizeof(char)*256);
    char *comando_dividido;
    int i;
    char *directorio = malloc(sizeof(char)*256);
    char *frase = malloc(sizeof(char)*150);
    char nota_necesaria_char[20];
    char *borrado = malloc(sizeof(char)*256);
    
    //genero el comando para crear el fichero donde metemos la nota necesaria
    sprintf(comando,"touch ./carpetas/");        
    
    //le meto el dni necesario
    strcat(comando, dni);
    strcat(comando, "/nota_necesaria.txt");
    sprintf(comando_touch, "%s", comando);
    
    if(existeArchivo(comando) == 0) { //el archivo ya existe

        //recojo el directorio como tal, que esta presente en el segundo trozo de strtok
        comando_dividido = strtok(comando, " ");
    
        i = 0;
        while(comando_dividido != NULL) {
            if(i==1)
                sprintf(directorio, "%s",comando_dividido);
            i++;
            comando_dividido = strtok(NULL, " "); //siguiente trozo
        }
        sprintf(borrado, "rm ");
        strcat(borrado, directorio);
        strcat(borrado, " >/dev/null 2>&1");
        system(borrado);
    }
    //ejecuto el comando
    system(comando_touch);

    //saco la nota necesaria, y genero el text completo para cada archivo
    int nota_necesaria = NOTA_MAX - atoi(nota);
    sprintf(nota_necesaria_char, "%d", nota_necesaria);
    sprintf(frase, NOTA_NECESARIA);
    strcat(frase, nota_necesaria_char);

    //comenzamos rellenando parte del comando para introducir la frase deseada en el archivo
    sprintf(comando, "echo \"%s\" >> %s", frase, directorio);

    //ejecutamos
    system(comando);

    //liberamos memoria asignada
    free(comando);
    free(comando_dividido);
    free(directorio);
    free(borrado);
}

int leerLineas(char *archivo) {
    FILE *fichero = fopen(archivo, "r");
    char linea_fichero[100];
    int lineas = 0;

    while(fgets(linea_fichero, 100, fichero) != NULL) {
        lineas++;
    }
    fclose(fichero);
    return lineas;
}

int existeArchivo(char* archivo){
    struct stat buffer;
    int existe = stat(archivo, &buffer);
    if(existe == 0)
        return 1;
    else  
        return 0;
}