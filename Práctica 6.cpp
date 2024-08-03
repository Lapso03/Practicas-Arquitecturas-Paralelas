/* Practica6 : Cálculo del factorial de un número introducido por teclado.
* 
* Sandro Mart�n, Roc�o Esteban, Sergio Buil.
*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int mirango, i;
    MPI_Status estado;
    int dato;
    float factorial1, factorial2;
    MPI_Request request1, request2;
    int testigo1, testigo2;

    // Inicializaci�n de MPI y obtenci�n del rango del proceso
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mirango);

    do {
        // Proceso principal (mirango == 0)
        if (mirango == 0) {
            int numero;
            // Solicitar al usuario ingresar un n�mero
            printf("Introducir el numero: ");
            fflush(stdout);
            scanf("%d", &numero);
            dato = numero;

            // Enviar el n�mero al proceso 1
            MPI_Isend(&dato, 1, MPI_INT, 1, 10, MPI_COMM_WORLD, &request1);
            MPI_Wait(&request1, &estado);

            // Si el n�mero es 0, abortar todos los procesos
            if (dato == 0) {
                MPI_Abort(MPI_COMM_WORLD, 0);
            }

            // Recibir el resultado del proceso 1 (factorial)
            MPI_Irecv(&factorial2, 1, MPI_FLOAT, 1, 10, MPI_COMM_WORLD, &request2);
            MPI_Test(&request2, &testigo2, &estado);
            while (!testigo2) {
                MPI_Test(&request2, &testigo2, &estado);
            }

            // Mostrar el resultado del c�lculo del factorial
            printf("El resultado es: %f\n", factorial2);
        }
        // Proceso secundario (mirango == 1)
        else if (mirango == 1) {
            // Recibir el n�mero del proceso 0
            MPI_Recv(&dato, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &estado);
            factorial1 = 1;

            // Calcular el factorial del n�mero recibido
            for (i = 1; i <= dato; i++) {
                factorial1 = factorial1 * i;
            }

            // Enviar el resultado (factorial) al proceso 0
            MPI_Isend(&factorial1, 1, MPI_FLOAT, 0, 10, MPI_COMM_WORLD, &request2);
            MPI_Wait(&request2, &estado);
        }

        // Sincronizaci�n de ambos procesos
        MPI_Barrier(MPI_COMM_WORLD);

    } while (1); // Bucle principal que permite al usuario ingresar m�ltiples n�meros

    // Finalizaci�n de MPI
    MPI_Finalize();
    return 0;
}