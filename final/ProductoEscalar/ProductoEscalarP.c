/*
// % mpicc -o ProductoEscalarP ProductoEscalarP.c
// % mpirun -np 2 ProductoEscalarP
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"  // Biblioteca para procesamiento paralelo

#define CANTIDAD_DEF 100 // Cantidad de elementos por defecto

int main(int argc, char *argv[]) {
  int cantidadElementos; 
  int par, impar, count, outCsv = 0, encabezado = 0;
  long long int productoEscalar = 0, resultado = 0;

  struct timeval inicio, fin; // estructuras para medir el tiempo de ejecución en microsegundos
  clock_t iniSeg, finSeg; // variables para medir el tiempo de ejecución en segundos

  int myId, numProc;



  // guardar el tiempo de inicio
  iniSeg = clock(); 
  gettimeofday(&inicio, NULL); 

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);

  // Se verifican los parámetros. Admás lo que hacemos es que el número de elementos 
  // sea un múltiplo del número de procesadores ingresados.
  // TO DO: Se puede mejorar el control de parámetros. Pero no es el onjeto de estudio
  if (argc < 2) {
    if (myId == 0) {
      cantidadElementos = numProc * CANTIDAD_DEF;
    }
  } else {
    if (argc >= 2) 
      cantidadElementos = atoi(argv[1]);

      if (cantidadElementos < numProc)
        cantidadElementos = numProc;
      else {
          int i = 1, num = numProc;
          while (cantidadElementos > num) {
              ++i;
              num = numProc*i;
          }
          if (cantidadElementos != num) {
              if (myId == 0)
                  // printf("Cantidad de elementos calculada %d por %d procesadores ingresados\n", num, numProc) ;
                cantidadElementos = num;
          }
      }

    if (argc >= 3) 
      outCsv=1;

    if (argc == 4)
      encabezado = 1;
  }
  

  // Creo los vectores
  int* vectorA;
  int* vectorB;
  int* vectorALocal;
  int* vectorBLocal;
  //int* vectorALocal[cantidadElementos/numProc];
  //int* vectorBLocal[cantidadElementos/numProc];

  // Asignar memoria para los vectores 
  vectorA = (int*) malloc(cantidadElementos * sizeof(int));
  vectorB = (int*) malloc(cantidadElementos * sizeof(int));
  vectorALocal = (int*) malloc((cantidadElementos/numProc) * sizeof(int));
  vectorBLocal = (int*) malloc((cantidadElementos/numProc) * sizeof(int));


  // Verificar si la asignación de memoria fue exitosa
  if ((vectorA == NULL) || (vectorB == NULL) || 
      (vectorALocal == NULL) || (vectorBLocal == NULL)) {
      printf("No se pudo asignar memoria\n");
      exit(1);
  }


  if(myId == 0) { // Si es el proceso 0
    //Cargar los vectores
    par = 0; 
    impar = 0;
    for (int count = 0; count < (cantidadElementos * 2) - 1; ++count) {
      if (count % 2 == 0) { // si es par, lo agrega al VectorA
        vectorA[par] = count;
        par++; // aumentar el contador de números pares
      } else { // si es impar, lo agrega al VectorB
        vectorB[impar] = count;
        impar++; // aumentar el contador de números impares
      }
    }
  }

  // Repartimos los valores del vector A
  MPI_Scatter(&vectorA[0], // Valores a compartir
          cantidadElementos / numProc, // Cantidad que se envia a cada proceso
          MPI_LONG, // Tipo del dato que se enviara
          &vectorALocal[0], // Variable donde recibir los datos
          cantidadElementos / numProc, // Cantidad que recibe cada proceso
          MPI_LONG, // Tipo del dato que se recibira
          0,  // proceso principal que reparte los datos
          MPI_COMM_WORLD); // Comunicador (En este caso, el global)
  
  // Repartimos los valores del vector B
  MPI_Scatter(&vectorB[0],
          cantidadElementos / numProc,
          MPI_LONG,
          &vectorBLocal[0],
          cantidadElementos / numProc,
          MPI_LONG,
          0,
          MPI_COMM_WORLD);


  // Calculamos el producto escalar entre dos vectores
  productoEscalar = 0;
  for (int count = 0; count < (cantidadElementos/numProc); ++count) {
      productoEscalar += vectorA[count] * vectorB[count];
  }

  // Reunimos los datos en un solo proceso, aplicando una operación
  // aritmetica, en este caso, la suma.
  MPI_Reduce(&productoEscalar, // Elemento a enviar
          &resultado, // Variable donde se almacena la reunion de los datos
          1, // Cantidad de datos a reunir
          MPI_LONG, // Tipo del dato que se reunira
          MPI_SUM, // Operacion aritmetica a aplicar
          0, // Proceso que recibira los datos
          MPI_COMM_WORLD); // Comunicador

    

  // guardar el tiempo de finalización
  finSeg = clock(); 
  gettimeofday(&fin, NULL); 


  // calcular y mostrar el tiempo de ejecución en microsegundos
  double tiempo = (double)(fin.tv_usec - inicio.tv_usec) + (double)(fin.tv_sec - inicio.tv_sec) * 1000000;
  
  // calcular y mostrar el tiempo de ejecución en segundos
  double tiempoSeg = ((double) (finSeg - iniSeg)) / CLOCKS_PER_SEC;

  
  if (myId == 0)
  {
    if (outCsv != 1){
      if (argc < 2) {
        printf("No se han especificado valores correctos para los parámetros requeridos.\n");
        printf("Se ejecutarán los procesos con una cantidad de elementos por defecto: %d\n", numProc * CANTIDAD_DEF);
        printf("y sin formato CSV.\n");
        printf("Uso: ProductoEscalarP <cantidadElementos> <csv> <encabezado>\n\n");
      }

      printf("Ejecución en paralelo del producto escalar entre\n");
      printf("dos vectores de %d elementos es: %lli\n", cantidadElementos, resultado);
      printf("Utilizando %d procesadores\n", numProc);
      printf("Tiempo de ejecución: %f microsegundos (µs)\n", tiempo);
      printf("Tiempo de ejecución: %f segundos\n\n\n", tiempoSeg);
    } else {
      if (encabezado == 1)
        printf("cantidadElementos, numProcesadores,productoEscalar,microsegundosEjec,segundosEjec\n");

      printf("%d,%d,%lli,%f,%f\n", cantidadElementos, numProc ,resultado, tiempo, tiempoSeg);
    }
  }

  // Finaliza la ejecución de los procesos
  MPI_Finalize();

   // Liberar la memoria asignada a los vectores 
  free(vectorA);
  free(vectorB);
  free(vectorALocal);
  free(vectorBLocal);
   
  return 0;
}