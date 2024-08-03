/* Practica3 : Calculamos el producto escalar entre dos vectores.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtiene el rango del proceso actual
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtiene el número total de procesos

    int vector_size = size; // El tamaño de los vectores coincide con el número de procesos

    int* x = (int*)malloc(vector_size * sizeof(int));
    int* y = (int*)malloc(vector_size * sizeof(int));
    int* local_x = (int*)malloc(sizeof(int));
    int* local_y = (int*)malloc(sizeof(int));

    double tInicio = 0, tFin = 0;

    // Inicializa los vectores x e y en el proceso 0
    if (rank == 0) {

        printf("Vector x: ");
        for (int i = 0; i < vector_size; i++) {
            x[i] = rand() % 10; // Números aleatorios entre 0 y 9.
            printf("%d ", x[i]);
        }
        printf("\n");

        printf("Vector y: ");
        for (int i = 0; i < vector_size; i++) {
            y[i] = rand() % 10; // Números aleatorios entre 0 y 9.
            printf("%d ", y[i]);
        }
        printf("\n");
    }
    // Reparte elementos a cada proceso
    MPI_Scatter(x, 1, MPI_INT, local_x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, 1, MPI_INT, local_y, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    tInicio = MPI_Wtime();
    // Calcula el producto escalar local
    int local_dot_product = (*local_x) * (*local_y);

    // Reduce los resultados locales para obtener el producto escalar global
    int global_dot_product;
    MPI_Reduce(&local_dot_product, &global_dot_product, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


    MPI_Barrier(MPI_COMM_WORLD);
    tFin = MPI_Wtime();

    //Muestra el resultado del producto escalar por pantalla.
    if (rank == 0) {
        printf("\nProducto escalar: %d\n\n", global_dot_product);
        printf("Ha tardado %f \n", tFin);
    }
    
    // Libera la memoria y finaliza MPI
    free(x);
    free(y);
    free(local_x);
    free(local_y);

    MPI_Finalize();
    return 0;
}
