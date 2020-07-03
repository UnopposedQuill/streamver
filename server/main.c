
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <libnet.h>

#define BUFFER_SIZE 512

//Esta variable le da señal al servidor de cuándo debe estar corriendo o no: 0 para detenerse
static volatile int keepRunning = 1;

//Necesito un handler que maneje la SIGINT que es disparada cuando se escribe CTRL+C en la terminal
void intHandler(int signal){
    //simplemente le paso la señal al servidor de que pare
    keepRunning = 0;
}

void *attendClient(void *argument){

    return NULL;
}

int main() {
    /*
    printf("Hello, World!\n");
    int a = 5;
    pthread_t hilo[3];
    for (int i = 0; i < 3; ++i) {
        pthread_create(&hilo[i], NULL, work, &a);
    }
    for (int i = 0; i < 3; ++i) {
        pthread_join(hilo[i], NULL);
    }
    */

    //<editor-fold defaultstate="collapsed" desc="Configuración del Servidor">

    FILE *archivoConfiguracion, *archivoClientes;
    char nombreArchivoClientes[30] = "clientes.txt";
    int maximoConexionesPendientes = 3, puertoServidor = 15000, maximoClientesConectados = 5;

    archivoConfiguracion = fopen("configFile.ini","r");
    if (archivoConfiguracion != NULL){
        fscanf(archivoConfiguracion,
               "[SETUP]\n"
               "Port=%d\n"
               "MaxWaitingConnections=%d\n"
               "MaxConnectedClients=%d\n"
               "ClientDatabase=%s\n", &puertoServidor, &maximoConexionesPendientes, &maximoClientesConectados, nombreArchivoClientes);
        //printf("%d\n", puertoCliente);
        fclose(archivoConfiguracion);
        printf("Archivo de configuración leído correctamente\n");
    }
    else{
        printf("Error de apertura del archivo de configuración. Utilizando datos por defecto\n");
    }

    //Ahora crear el archivo de base de clientes, en caso que no exista
    archivoClientes = fopen(nombreArchivoClientes, "a+");
    //fprintf(archivoClientes, "esteban;2011\n");
    //fclose(archivoClientes);

    //Ahora a señalarle al SO que use el handler para SIGINT
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);

    //Finalmente a colocar los manejadores de Pthreads
    pthread_t hilos[maximoClientesConectados];
    short ocupados[maximoClientesConectados];

    //El buffer con el que trabajará el servidor
    char buffer[BUFFER_SIZE] = {0};
    //</editor-fold>

    //<editor-fold defaultstate="collapsed" desc="Configuración de Socket atendedor">
    int server_fd, new_socket, valread;
    int opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    //Primero creo un handler para el socket del servidor
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        //Ocurrió un error al crear el socket
        perror("Socket creation error");
        return EXIT_FAILURE;
    }
    //Creación del socket exitosa, ahora lo que hago es que señalo al SO que reutilice el puerto y la ip
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0){
        perror("SO couldn't be instructed to reuse the socket address and port");
        return EXIT_FAILURE;
    }

    //En este punto ya es seguro colocar la ip, puertos y protocolos de los que deseo leer
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(puertoServidor);

    //Ahora ya uniré el socket y la dirección, necesito el handler del socket, la dirección
    //a unir y finalmente la longitud de la dirección
    if(bind(server_fd, (struct sockaddr*) &address, (socklen_t) addrlen) < 0){
        perror("Socket couldn't be bound to specified address, port or protocols");
        return EXIT_FAILURE;
    }

    //Ahora falta señalarle al SO que va a escuchar del puerto, con un máximo de conexiones pendientes
    if(listen(server_fd, maximoConexionesPendientes) < 0){
        perror("Socket couldn't be signaled to listen");
        return EXIT_FAILURE;
    }
    printf("Initialization Success, listening on port %d\n", puertoServidor);
    //</editor-fold>

    //<editor-fold defaultstate="collapsed" desc="Ciclo de Atención">
    while (keepRunning){
        printf("Ejecutando\n");
        if((new_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0){
            //No pudo aceptarla
            perror("\nError upon accepting a new connection");
        }
        else {
            //Pudo aceptarla
            printf("Solicitud Entrante\n");

            //Primero tengo que leer el primer byte, el cual me dirá qué acción tengo que tomar
            if ((valread = read(new_socket, buffer, 1)) < 0) {
                perror("Error upon reading new data\n");
                continue;
            }
            //en este momento valread contiene la cantidad de bytes leídos
            //Por ahora sólo le ordenaré al servidor imprimir todo lo que lee
            char directiva = buffer[0] - '0';
            switch (directiva) {
                case 0: {
                    //tengo que registrar un nuevo usuario, primero creo las variables que guardarán su información
                    printf("Registrar\n");

                    // leeré a lo sumo 30 bytes de nombre
                    while ((valread = read(new_socket, buffer, 30)) != 0) { //Hasta encontrar un EOF
                        if (valread < 0) {
                            perror("Error al leer el nombre del cliente\n");
                            break;
                        }
                        // No hubo error al leer, sólo que no hay EOF, coloco lo que llevo en el archivo
                        fprintf(archivoClientes, "%s", buffer);
                        memset(buffer, 0, BUFFER_SIZE);

                        //strncpy(c->ipRegistrada, inet_ntoa(address.sin_addr), 16);
                    }
                    if (valread < 0){
                        break; //muevo el error hacia adelante para que salga del switch
                    }

                    fprintf(archivoClientes, ";");//El separador de nombre;contraseña

                    //Se leyó correctamente el nombre
                    //leeré a lo sumo 30 bytes de contraseña
                    while ((valread = read(new_socket, buffer, 30)) != 0) { //Hasta encontrar un EOF
                        if (valread < 0) {
                            perror("Error al leer el nombre del cliente\n");
                            break;
                        }
                        // No hubo error al leer, sólo que no hay EOF, coloco lo que llevo en el archivo
                        fprintf(archivoClientes, "%s", buffer);
                        memset(buffer, 0, BUFFER_SIZE);
                    }
                    fprintf(archivoClientes, "\n");//Finalizo
                    //Coloco código de respuesta
                    strncpy(buffer, "1", 1);

                    close(new_socket);//Ya terminé con la conexión
                    break;
                }
                case 1:{
                    //tengo que iniciar sesión con un usuario
                    printf("Iniciar Sesión\n");

                }
                default:{
                    printf("Unrecognized option\n");
                }
            }
        }
    }

    //</editor-fold>

    //<editor-fold defaultstate="collapsed" desc="Cerrado">
    printf("Saliendo...\n");

    //Cerrar socket del servidor
    close(server_fd);

    //</editor-fold>

    return EXIT_SUCCESS;
}
