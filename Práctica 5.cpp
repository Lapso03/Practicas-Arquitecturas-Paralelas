/* Practica5 : Tratamiento de ficheros.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 20 // N�mero de veces que cada proceso escribe en el archivo

int main(int argc, char* argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_File file;
    MPI_Status estado;

    char filename[] = "output.txt";
    char datarep[] = "native";
    int escribir[N], leer[N], i;

    // Abrir el archivo en modo de escritura
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &file);
    MPI_File_set_view(file, N * rank * sizeof(int), MPI_INT, MPI_INT, datarep, MPI_INFO_NULL);

    // Escribir en el archivo el rango del proceso N veces
    for (i = 0; i < N; i++) {
        escribir[i] = 48 + rank;// Convertir el rango a ASCII
    }
    MPI_File_write_at(file, 0, escribir, N, MPI_INT, &estado);

    // Cerrar el archivo
    MPI_File_close(&file);

    // Abrir el archivo en modo de lectura
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &file);
    // Leer del archivo
    MPI_File_set_view(file, N * rank * sizeof(int), MPI_INT, MPI_INT, datarep, MPI_INFO_NULL);
    MPI_File_read_at(file, 0, leer, N, MPI_INT, &estado);

    // Cerrar el archivo
    MPI_File_close(&file);

    // Mostrar los datos
    for (i = 0; i < N; i++) {
        leer[i] = leer[i] - 48;
    }
    printf("Proceso %d lee:\n ", rank);

    for (i = 0; i < N; i++) {
        printf("%d ", leer[i]);
    }
    printf("\n");

    MPI_Finalize();
    return 0;
}
