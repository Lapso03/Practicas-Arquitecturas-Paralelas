/* Practica9 : Multiplicación de matrices con reserva dinámica de memoria y topología cartesiana.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <string.h>

void imprimir_matriz(float* matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("\t%lf", matrix[i * cols + j]);
        }
        printf("\n");
    }
}
// Funci�n para inicializar una matriz con valores aleatorios.
void initialize_matrix(float* matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = (float)rand() / RAND_MAX * 10.0; // Valores aleatorios entre 0 y 10.
    }
}

// Funci�n para realizar la multiplicaci�n de matrices por bloques.
void multiply_matrices(float* A_block, float* B, float* C_block, int A_cols, int B_cols, int block_rows) {
    for (int i = 0; i < block_rows; i++) {
        for (int j = 0; j < B_cols; j++) {
            float sum = 0.0;
            for (int k = 0; k < A_cols; k++) {
                sum += A_block[i * A_cols + k] * B[k * B_cols + j];
            }
            C_block[i * B_cols + j] = sum;
        }
    }
}

int main(int argc, char* argv[]) {
    int world_size, mirango;
    double start_time, end_time, total_time;

    // Inicializaci�n de MPI.
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);

    srand(time(NULL) + mirango); // Semilla para n�meros aleatorios.

    int N;

    if (mirango == 0) {// N= (argc > 1) ? atoi(argv[1]) : 0;
        printf("\nTama�o de matriz: ");
        fflush(stdout);
        scanf("%d", &N);

        while (N <= 0) {

            printf("\nPor favor, especifique un tama�o de matriz v�lido: ");
            fflush(stdout);
            scanf("%d", &N);
        }
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int rows_per_proc;
    int extra_rows = 0;
    if (mirango == 0) {
        // C�lculo del n�mero de filas por proceso.
        if (world_size > N) {
            rows_per_proc = 1;
        }
        else {
            rows_per_proc = N / world_size;
            extra_rows = N % world_size; // Filas extra para manejar matrices no divisibles exactamente entre los procesos.
        }
    }
    MPI_Bcast(&rows_per_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&extra_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);


    // Reserva de memoria para bloques de la matriz A.
    float* A_block = (float*)malloc(rows_per_proc * N * sizeof(float));
    float* C_block = (float*)malloc(rows_per_proc * N * sizeof(float));
    float* B = (float*)malloc(N * N * sizeof(float)); // Todos los procesos necesitan espacio para B.
    float* A = (float*)malloc(N * N * sizeof(float));

    // Inicializaci�n de la matriz B por el proceso 0 y distribuci�n a todos los procesos.
    // Distribuci�n de la matriz A entre todos los procesos.
    if (mirango == 0) {

        initialize_matrix(A, N, N);
        printf("\nP %d : Matriz A: \n", mirango);
        imprimir_matriz(A, N, N);
        fflush(stdout);

        initialize_matrix(B, N, N);
        printf("\nP %d : Matriz B: \n", mirango);
        imprimir_matriz(B, N, N);
        fflush(stdout);
    }

    start_time = MPI_Wtime();

    // Dividir A entre los procesos, enviando a cada uno su bloque correspondiente.
    MPI_Scatter(A, rows_per_proc * N, MPI_FLOAT, A_block, rows_per_proc * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Broadcast de la matriz B a todos los procesos.
    MPI_Bcast(B, N * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Multiplicaci�n de matrices.
    multiply_matrices(A_block, B, C_block, N, N, rows_per_proc);

    // Recolectar los bloques de la matriz C en el proceso 0.
    float* C = NULL;
    if (mirango == 0) {
        C = (float*)malloc(N * N * sizeof(float));
    }

    // Usamos MPI_Gather para recolectar los resultados.
    MPI_Gather(C_block, rows_per_proc * N, MPI_FLOAT, C, rows_per_proc * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Manejar las filas extras si las hay en el �ltimo proceso
    if (extra_rows > 0 && mirango == 0) {
        float* extra_A_block = (float*)malloc(extra_rows * N * sizeof(float));
        float* extra_C_block = (float*)malloc(extra_rows * N * sizeof(float));

        // Copiar las filas restantes al bloque extra
        memcpy(extra_A_block, A + rows_per_proc * world_size * N, extra_rows * N * sizeof(float));

        printf("\nP %d : Fila extra: \n", mirango);
        for (int i = 0; i < extra_rows * N; i++) {
            printf("    %lf", extra_A_block[i]);
        }
        fflush(stdout);

        // Multiplicar las filas extra
        multiply_matrices(extra_A_block, B, extra_C_block, N, N, extra_rows);

        // Copiar los resultados de vuelta a c
        memcpy(C + (rows_per_proc * world_size * N), extra_C_block, extra_rows * N * sizeof(float));

        free(extra_A_block);
        free(extra_C_block);
    }

    end_time = MPI_Wtime();

    if (mirango == 0) {
        printf("\nP %d : Matriz C: \n", mirango);
        imprimir_matriz(C, N, N);

        printf("\nTiempo de ejecuci�n: %f segundos\n", end_time - start_time);

        fflush(stdout);
    }

    // Liberaci�n de memoria.
    free(A_block);
    free(C_block);
    free(B);
    free(C);

    // Finalizaci�n de MPI.
    MPI_Finalize();
    return 0;
}