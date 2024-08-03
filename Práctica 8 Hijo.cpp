/* Practica8 : Gestión dinámica de procesos.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char* argv[])
{
    int tamano, mirango;
    char saludo_pa[20];
    char saludo_hijo[] = "Hola hermano, soy el hijo menor";
    char saludo_her[33];

    MPI_Comm commPadre, intracom;
    MPI_Init(&argc, &argv);

    MPI_Comm_get_parent(&commPadre);
    if (commPadre == MPI_COMM_NULL) {
        printf("No se pudo obtener el comunicador del padre.\n");
        fflush(stdout);
        //MPI_Comm_disconnect(&intracom);
        MPI_Finalize();
        return 0;
    }
    MPI_Intercomm_merge(commPadre, 1, &intracom);
    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);

    printf("Hola, soy el hijo %d y he sido creado por mi padre\n", mirango);
    fflush(stdout);

    MPI_Recv(saludo_pa, 20, MPI_CHAR, 0, 0, intracom, MPI_STATUS_IGNORE);
    //MPI_Bcast(saludo_pa, 20, MPI_CHAR, 0, intracom);

    printf("Soy el hijo %d y he recibido el saludo: %s\n", mirango, saludo_pa);
    fflush(stdout);

    fflush(stdout);
    if (mirango == 0) // primer proceso hijo
    {
        //MPI_Bcast(saludo_hijo, sizeof(saludo_hijo), MPI_CHAR, 0, intracom);
        for (int i = 1; i < tamano; i++) {  // Aseg�rate de enviar a todos los hermanos en el nuevo comunicador
            MPI_Send(saludo_hijo, strlen(saludo_hijo) + 1, MPI_CHAR, i, 1, intracom);
        }
    }
    else
    {
        MPI_Recv(saludo_her, strlen(saludo_hijo) + 1, MPI_CHAR, 0, 0, intracom, MPI_STATUS_IGNORE);
        printf("Soy el proceso %d y he recibido: %s\n", mirango, saludo_her);
        fflush(stdout);

    }
    MPI_Comm_disconnect(&intracom);
    MPI_Finalize();
    return 0;
}