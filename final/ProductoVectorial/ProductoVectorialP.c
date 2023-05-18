/*
// % mpicc -o ProductoVectorialP ProductoVectorialP.c
// % mpirun -np 2 ProductoVectorialP
//
// Para implementar la librería MPI en el código de ProsuctoVectorialS, es 
// necesario incluir la biblioteca MPI y establecer la comunicación entre los  
// procesos. Para esto, se debe dividir el trabajo y asignar una parte de los  
// vectores a cada proceso. Cada proceso debe calcular la suma de los productos 
// de los elementos correspondientes de los dos vectores asignados y enviar el 
// resultado al proceso principal. El proceso principal luego suma los 
// resultados para obtener el producto escalar total.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

#define CANTIDAD_DEF 100 // cantidad de elementos por defecto

int main(int argc, char *argv[]) {
    int cantidadElementos = CANTIDAD_DEF;
    int outCsv = 0, encabezado = 0;
    int count1, count2;
    int *vectorA, *vectorB;
    long long int *productoVectorial = 0;

    struct timeval inicio, fin; // estructuras para medir el tiempo de ejecución en microsegundos
    clock_t iniSeg, finSeg; // variables para medir el tiempo de ejecución en segundos

    int miRango, numProcs;

    // Inicializar MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &miRango);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    // Se verifican los parámetros
    // TO DO: Se puede mejorar el control de parámetros. Pero no es el objeto de estudio
    if (miRango == 0) {
        if (argc < 2) {
            printf("No se han especificado valores correctos para los parámetros requeridos.\n");
            printf("La cantidad de elementos serán: %d sin formato CSV.\n", cantidadElementos);
            printf("Uso: ProductoVectorialP <cantidadElementos> <csv> <encabezado>\n\n");
        } else {
            if (argc >= 2) {
                cantidadElementos = atoi(argv[1]);
            }
            if (argc >= 3)
                outCsv = 1;

            if (argc == 4)
                encabezado = 1;
        }
    }

    //Difundir datos 
    MPI_Bcast(&cantidadElementos, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&outCsv, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&encabezado, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // guardar el tiempo de inicio
    iniSeg = clock();
    gettimeofday(&inicio, NULL);

    // Asignación dinámica de memoria para los vectores y el resultado
    vectorA = (int *)malloc(cantidadElementos * sizeof(int));
    vectorB = (int *)malloc(cantidadElementos * sizeof(int));
    productoVectorial = (long long int *)malloc(cantidadElementos * sizeof(long long int));

    // Verificar si la asignación de memoria fue exitosa
    if ((vectorA == NULL) || (vectorB == NULL) || (productoVectorial == NULL)) {
        printf("No se pudo asignar memoria\n");
        exit(1);
    }

    // Llenar el primer vector con los primeros n números naturales impares
    for (count1 = miRango; count1 < cantidadElementos; count1 += numProcs) {
        vectorA[count1] = 2 * count1 + 1;
    }

    // Llenar el segundo vector con los primeros n números naturales pares
    for (count1 = miRango; count1 < cantidadElementos; count1 += numProcs) {
        vectorB[count1] = 2 * count1;
    }

    // Multiplicación de vectores utilizando el algoritmo de
    // la convención aportada en el seminario
    for (count1 = miRango; count1 < cantidadElementos; count1 += numProcs) {
        productoVectorial[count1] = 0;
        for (count2 = 0; count2 < cantidadElementos; count2++) {
            productoVectorial[count1] += vectorA[count1] * vectorB[count2];
        }
    }

    long long int *resultados;
    if (miRango == 0) {
        resultados = (long long int *)malloc(cantidadElementos * sizeof(long long int));
    }

    //Reunir los resultados
    MPI_Gather(productoVectorial, 
               cantidadElementos / numProcs, 
               MPI_LONG_LONG_INT,
               resultados, 
               cantidadElementos / numProcs, MPI_LONG_LONG_INT, 
               0, 
               MPI_COMM_WORLD);

    // guardar el tiempo de finalización
    finSeg = clock();
    gettimeofday(&fin, NULL);

    // calcular y mostrar el tiempo de ejecución en microsegundos
    double tiempo = (double)(fin.tv_usec - inicio.tv_usec) + (double)(fin.tv_sec - inicio.tv_sec) * 1000000;
    // calcular y mostrar el tiempo de ejecución en segundos
    double tiempoSeg = ((double)(finSeg - iniSeg)) / CLOCKS_PER_SEC;

    if (miRango == 0) {
        if (outCsv != 1) {
            printf("Ejecución paralela del producto vectorial entre\n");
            printf("dos vectores de %d elementos\n", cantidadElementos);
            printf("utilizando %d procesadores\n", numProcs);
            // Imprimir el resultado del productoVectorial
            // for (count1 = 0; count1 < cantidadElementos; count1++) {
            //    printf("%lli ", resultados[count1]);
            // }
            printf("\n");
            printf("Tiempo de ejecución: %f microsegundos (µs)\n", tiempo);
            printf("Tiempo de ejecución: %f segundos\n\n\n", tiempoSeg);
        } else {
            if (encabezado == 1) {
                // printf("cantidadElementos,productoVectorial,microsegundosEjec,segundosEjec\n");
                printf("cantidadElementos,microsegundosEjec,segundosEjec\n");
            }
            // printf("%d,%lln,%f,%f\n", cantidadElementos, numProcs,resultados, tiempo, tiempoSeg);
            printf("%d,%d,%f,%f\n", cantidadElementos, numProcs, tiempo, tiempoSeg);
        }

        free(resultados);
    }

    // Liberar la memoria asignada a los vectores y el resultado
    free(vectorA);
    free(vectorB);
    free(productoVectorial);

    // Finalizar MPI
    MPI_Finalize();

    return 0;
}
