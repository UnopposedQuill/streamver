#include <stdio.h>

int main() {
    FILE *archivoConfiguracion;
    int puertoServidor = 1500;
    char serverIP[17], nombre[30], contrasenha[30];
    //char serverIP[17] = "127.0.0.1", nombre[30] = "esteban", contrasenha[30] = "2011";

    archivoConfiguracion = fopen("configFile.ini","r");
    if (archivoConfiguracion != NULL){
        fscanf(archivoConfiguracion,
               "[SETUP]\n"
               "Port=%d\n"
               "ServerIP=\"%s\"\n"
               "Name=\"%s\"\n"
               "Password=\"%s\"\n", &puertoServidor, serverIP, nombre, contrasenha);
        //printf("%d\n", puertoCliente);
        fclose(archivoConfiguracion);
        printf("Archivo de configuración leído correctamente\n");
    }
    else{
        printf("Error de apertura del archivo de configuración. Utilizando datos por defecto\n");
    }
}
