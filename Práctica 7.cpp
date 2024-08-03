/* Practica7 : División de una matriz en dos matrices triangulares (superior e inferior) a dos procesos.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 4  // Tama�o de la matriz

// Funci�n para inicializar la matriz con n�meros enteros aleatorios
void inicializar_matriz(int matriz[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matriz[i][j] = rand() % 100;  // N�meros aleatorios entre 0 y 99
        }
    }
}

// Funci�n para imprimir la matriz
void imprimir_matriz(int matriz[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    int mirango, tamano;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);

    srand(time(NULL) + mirango);  // Semilla para la generaci�n de n�meros aleatorios

    int matriz[N][N];

    // Crear los tipos de datos derivados para las matrices triangulares
    MPI_Datatype tipo_triang_sup, tipo_triang_inf;

    // Tipo para la matriz triangular superior
    int longitudes[N];
    MPI_Aint desplazamientos[N];
    for (int i = 0; i < N; i++) {
        longitudes[i] = N - i;
        desplazamientos[i] = i * N * sizeof(int) + i * sizeof(int);
    }
    MPI_Type_indexed(N, longitudes, (int*)desplazamientos, MPI_INT, &tipo_triang_sup);
    MPI_Type_commit(&tipo_triang_sup);

    // Tipo para la matriz triangular inferior
    for (int i = 0; i < N; i++) {
        longitudes[i] = i + 1;
        desplazamientos[i] = i * N * sizeof(int);
    }
    MPI_Type_indexed(N, longitudes, (int*)desplazamientos, MPI_INT, &tipo_triang_inf);
    MPI_Type_commit(&tipo_triang_inf);

    // Proceso principal inicializa la matriz y la env�a
    if (mirango == 0) {
        inicializar_matriz(matriz);
        printf("Proceso %d tiene la matriz completa:\n", mirango);
        imprimir_matriz(matriz);
        fflush(stdout);

        // Enviar la matriz triangular superior al proceso 1
        //if (tamano > 1) {
        MPI_Send(matriz, 1, tipo_triang_sup, 1, 0, MPI_COMM_WORLD);
        //}

        // Enviar la matriz triangular inferior al proceso 2
        //if (tamano > 2) {
        MPI_Send(matriz, 1, tipo_triang_inf, 2, 0, MPI_COMM_WORLD);
        //}
    }

    // Procesos receptores reciben su matriz correspondiente y la imprimen
    if (mirango == 1) {
        int matriz_recibida_sup[N][N] = { 0 };
        MPI_Recv(matriz_recibida_sup, 1, tipo_triang_sup, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proceso %d recibi� la matriz triangular superior:\n", mirango);
        imprimir_matriz(matriz_recibida_sup);
    }
    else if (mirango == 2) {
        int matriz_recibida_inf[N][N] = { 0 };
        MPI_Recv(matriz_recibida_inf, 1, tipo_triang_inf, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proceso %d recibi� la matriz triangular inferior:\n", mirango);
        imprimir_matriz(matriz_recibida_inf);
    }

    // Liberar los tipos de datos derivados y finalizar MPI
    MPI_Type_free(&tipo_triang_sup);
    MPI_Type_free(&tipo_triang_inf);
    MPI_Finalize();
    return 0;
}
