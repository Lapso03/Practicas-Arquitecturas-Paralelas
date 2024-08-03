/* Practica4 : Producto de matrices de tamaño 3x4.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <vector>
#include <stdio.h>

void imprime(int* matriz, int M, int N);

int main(int argc, char* argv[]) {
    int rank, size;
    int M = 3; // N�mero de filas de las matrices
    int N = 4; // N�mero de columnas de las matrices
    MPI_Comm cart_comm;
    int dims[2] = { M, N };
    int periods[2] = { 0, 0 };
    int coords[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    if (size != M * N) {
        if (rank == 0) {
            printf("El n�mero de procesos debe ser igual a MxN (%dx%d).\n", M, N);
        }
        MPI_Finalize();
        return 1;
    }

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 2, coords);

    printf("Soy el proceso %d y ahora soy [%d][%d]\n", rank, coords[0], coords[1]);
    fflush(stdout);

    int* matrizA = (int*)malloc(M * N * sizeof(int));
    int* matrizB = (int*)malloc(M * N * sizeof(int));
    int sum_local;
    int* sum_final = (int*)malloc(M * N * sizeof(int));

    // Medimos el rendimiento usando MPI_Wtime
    double startTime, endTime;

    startTime = MPI_Wtime();

    // Proceso 0 genera las matrices y las distribuye
    if (rank == 0) {

        // Generamos las matrices
        for (int i = 0; i < M * N; i++) {
            matrizA[i] = rand() % 10;
            matrizB[i] = rand() % 10;
        }

        printf("La matriz A es: \n");
        imprime(matrizA, M, N);
        fflush(stdout);
        printf("La matriz B es: \n");
        imprime(matrizB, M, N);
        fflush(stdout);

    }

    // Distribuimos matrices usando MPI_Bcast
    MPI_Bcast(matrizA, M * N, MPI_INT, 0, cart_comm);
    MPI_Bcast(matrizB, M * N, MPI_INT, 0, cart_comm);


    // Cada proceso suma un elemento de las matrices
    int r = coords[0] * N + coords[1];
    sum_local = matrizA[r] + matrizB[r];
    //printf("[%d][%d] %d\n", coords[0],coords[1], sum_local);

    // Recopilamos los resultados en el proceso 0 usando MPI_Gather
    MPI_Gather(&sum_local, 1, MPI_INT, sum_final, 1, MPI_INT, 0, cart_comm);

    // Proceso 0 muestra los resultados
    if (rank == 0) {
        printf("Resultado de la suma de matrices:\n");
        imprime(sum_final, M, N);
    }

    fflush(stdout);

    endTime = MPI_Wtime();
    if (rank == 0) {
        printf("Tiempo de ejecuci�n: %lf segundos\n", endTime - startTime);
        fflush(stdout);
    }

    // Liberamos memoria
    free(matrizA);
    free(matrizB);
    free(sum_final);

    MPI_Finalize();
    return 0;
}

void imprime(int* matriz, int M, int N) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%4d ", matriz[i * M + j]);
        }
        printf("\n");
    }
}