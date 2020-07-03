#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <libnet.h>

#define BUFFER_SIZE 512

int main() {

    //<editor-fold defaultstate="collapsed" desc="Configuración de Cliente">
    FILE *archivoConfiguracion;
    int puertoServidor = 1500;
    char direccionServidor[17] = "127.0.0.1", nombre[30] = "esteban", contrasenha[30] = "2011";

    archivoConfiguracion = fopen("configFile.ini", "r");
    if (archivoConfiguracion != NULL) {
        fscanf(archivoConfiguracion,
               "[SETUP]\n"
               "Port=%d\n"
               "ServerIP=%s\n"
               "Name=%s\n"
               "Password=%s\n", &puertoServidor, direccionServidor, nombre, contrasenha);
        //printf("%d\n", puertoCliente);
        fclose(archivoConfiguracion);
        printf("Archivo de configuración leído correctamente\n");
    } else {
        printf("Error de apertura del archivo de configuración. Utilizando datos por defecto\n");
    }

    //El buffer que contendrá los datos que se le enviarán al servidor
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    //</editor-fold>

    //<editor-fold defaultstate="collapsed" desc="Conexión Inicial">
    //La dirección del socket del servidor
    struct sockaddr_in serv_addr;
    int server_fd, socket_handler, valread;

    if ((socket_handler = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return EXIT_FAILURE;
    }
    //Memset se encarga de asignar un valor a la memoria, en este caso, la
    //idea es limpiar los valores de <serv_addr> que no necesito
    memset(&serv_addr, 0, sizeof(serv_addr));

    //Ahora coloco los valores que sí necesito
    //Primero el protocolo: Ipv4
    serv_addr.sin_family = AF_INET;
    //Ahora el puerto: puertoServidor
    serv_addr.sin_port = htons(puertoServidor);

    //Ahora necesito convertir las direcciones a su forma binaria:
    if (inet_pton(AF_INET, direccionServidor, &serv_addr.sin_addr) <= 0) {
        perror("Address invalid or not supported");
        return EXIT_FAILURE;
    }

    //Ahora intento conectar con el servidor
    if (connect(socket_handler, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Couldn't connect to server\n");
        return EXIT_FAILURE;
    }

    //Coloco el paquete de inicio dentro del buffer
    sprintf(buffer, "0%s;%s\n", nombre, contrasenha);

    //Coloco la solicitud
    if(send(socket_handler, buffer, 1 + strlen(nombre) + 1 + strlen(contrasenha) + 1, 0) < 0){
        perror("No se pudo enviar la solicitud\n");
        return EXIT_FAILURE;
    }

    memset(buffer, 0, BUFFER_SIZE);
    if ((valread = read(socket_handler, buffer, 1)) < 0) {
        perror("Error consiguiendo la respuesta del servidor\n");
        return EXIT_FAILURE;
    }

    if (strcmp(buffer, "1") != 0) {
        return EXIT_FAILURE;
    }

    //</editor-fold>

    return EXIT_SUCCESS;
}
