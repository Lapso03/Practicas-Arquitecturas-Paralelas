/* Practica8 : Gestión dinámica de procesos.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define N 3  // Numero de hijos

int main(int argc, char* argv[])
{
    //nuevos intercomunicadores
    MPI_Comm intercom, intracom;

    MPI_Init(&argc, &argv);

    int tamano;
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);
        char saludo_padre[] = "Hola, soy tu padre";
    char ruta_hijo[] = "\\Pr�ctica 8 hijo.exe"; // "PracticaARPA8hijo.exe"; //Habría que añadir la ruta entera
    int array_of_errcodes[4];

    printf("\nSoy el padre");
    fflush(stdout);

    MPI_Comm_spawn(ruta_hijo,   //nombre del programa hijo
        MPI_ARGV_NULL, //argumentos en linea de comandos
        3, // numero de copias que lanzar (del programa mpi)
        MPI_INFO_NULL,
        0,                    	//root, rango del porceso pradre
        MPI_COMM_SELF,
        &intercom, //devolviendo un nuevo intercomunicador
        array_of_errcodes);

    printf("\nHola, soy el padre, he lanzado los hijos \n");
    fflush(stdout);

    MPI_Intercomm_merge(intercom, 0, &intracom);

    for (int i = 0; i < tamano; i++) {
        MPI_Send(saludo_padre, 20, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
    //MPI_Bcast(saludo_padre, 20, MPI_CHAR, 0, intracom);

    printf("\nPadre: saludo enviado\n");
    fflush(stdout);

    MPI_Comm_disconnect(&intracom);
    MPI_Finalize();
    return 0;
}