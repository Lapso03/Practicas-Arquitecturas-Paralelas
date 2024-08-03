/* Practica1 : Se manda un mensaje de un proceso a otro y se lee en el que lo recibe.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <stdio.h>
int main(int argc, char* argv[])
{

    int mirango, tamano;
    int longitud;
    char nombre[32];
    int mensaje; // El mensaje que se enviar�
    int destino;
    int origen;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);
    MPI_Get_processor_name(nombre, &longitud);


    //Al menos dos procesos para que se puedan enviar mensajes.
    if (tamano < 2) {
        fprintf(stderr, "Este programa requiere al menos dos procesos MPI.\n");
        MPI_Finalize();
        return 1;
    }

    // Calcula el rango del proceso destino y origen
    destino = (mirango + 1) % tamano;
    origen = (mirango - 1 + tamano) % tamano;

    if (mirango == 0) {
        // Proceso 0 recopila la entrada del usuario
        printf("[Maquina %s]> Proceso %d de %d: Escribe el mensaje a enviar: ", nombre, mirango, tamano);
        fflush(stdout);
        scanf("%d", &mensaje);

        // Env�a el mensaje al proceso siguiente en el anillo
        printf("[Maquina %s]> Proceso %d de %d: Enviando mensaje a Proceso %d\n", nombre, mirango, tamano, destino);
        fflush(stdout);
        MPI_Send(&mensaje, 1, MPI_INT, destino, 0, MPI_COMM_WORLD);

    }
    else {
        // Los dem�s procesos solo reciben y env�an mensajes
        MPI_Recv(&mensaje, 1, MPI_INT, origen, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("[Maquina %s]> Proceso %d de %d: Mensaje recibido: %d\n\n", nombre, mirango, tamano, mensaje);
        fflush(stdout);

        // Env�a el mensaje al siguiente proceso
        printf("[Maquina %s]> Proceso %d de %d: Enviando mensaje a Proceso %d\n", nombre, mirango, tamano, destino);
        MPI_Send(&mensaje, 1, MPI_INT, destino, 0, MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;
}