/* Practica2 : Suma dos matrices, cada proceso se encarga de una fila.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int longitud;
    char nombre[32];
    double tInicio = 0, tFin = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(nombre, &longitud);

    int N = size;

    // Declaramos un doble puntero para almacenar las matrices
        // Esto nos permite referenciar los elementos de la matriz mediante [fila][columna]
    int** A;
    int** B;
    int** C;

    A = (int**)malloc(N * sizeof(int*));
    B = (int**)malloc(N * sizeof(int*));
    C = (int**)malloc(N * sizeof(int*));
    int* MfA;
    MfA = (int*)malloc(N * sizeof(int));
    int* MfB;
    MfB = (int*)malloc(N * sizeof(int));
    int* MfC;
    MfC = (int*)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        A[i] = MfA + i * N;
        B[i] = MfB + i * N;
    }

    if (rank == 0) {
        printf("Numero de procesos: %d \n", size);
        printf("Dimensiones de las matrices: %d x %d \n", N, N);
        fflush(stdout);

        srand((int)time(NULL));
        //Llenar matrices A y B en el proceso 0
        for (int i = 0; i < N; i++) {
            //for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = rand() % 10;
                B[i][j] = rand() % 10;
            }
        }

        //Matriz A por pantalla
        
        printf("Matriz A\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d   ", A[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        //Matriz B por pantalla

        printf("Matriz B\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d   ", B[i][j]);
            }
            printf("\n");
        }
        fflush(stdout);

        //enviar las matrices al resto de pocesos

        MPI_Bcast(A[0], N * N, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(B[0], N * N, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        tInicio = MPI_Wtime();

        //Suma de matrices
        for (int i = rank; i < N; i += size) {
            for (int j = 0; j < N; j++) {
                C[i][j] = A[i][j] + B[i][j];
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        tFin = MPI_Wtime();

        MPI_Gather(&B[rank][0], N * N / size, MPI_INT, &B[0], N * N / size, MPI_INT, 0, MPI_COMM_WORLD);

        //Matriz resultado por pantalla
        printf("Matriz resultado:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");

        }
        printf("Tiempo de ejecuci�n: %f \n", tFin - tInicio);
        fflush(stdout);

    }else {
        //enviar las matrices al resto de pocesos

        MPI_Bcast(A[0], N * N, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(B[0], N * N, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        tInicio = MPI_Wtime();


        for (int i = rank; i < N; i += size) {
            for (int j = 0; j < N; j++) {
                C[i][j] = A[i][j] + B[i][j];
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        tFin = MPI_Wtime();

        MPI_Gather(&B[rank][0], N * N / size, MPI_INT, &B[0], N * N / size, MPI_INT, 0, MPI_COMM_WORLD);

    }
    

    // Liberar memoria asignada
    for (int i = 0; i < N; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    MPI_Finalize();
    return 0;
}