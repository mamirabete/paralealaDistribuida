/*
// % mpicc -o ProductoEscalarP ProductoEscalarP.c
// % mpirun -np 2 ProductoEscalarP
//
//
// Este código utiliza la función MPI_Init para inicializar MPI, 
// MPI_Comm_rank para obtener el rango del proceso, MPI_Comm_size 
// para obtener el número de procesos, MPI_Reduce para sumar los 
// productos escalares parciales de todos los procesos y MPI_Finalize 
// para finalizar MPI. Además, divide el trabajo entre los procesos 
// con la variable elementosPorProceso y maneja los elementos restantes 
// en el proceso 0.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

// cantidad de elementos por defecto
#define CANTIDAD_DEF 100 

int main(int argc, char *argv[]) {
  int cantidadElementos = CANTIDAD_DEF;
  int par, impar, count,  outCsv = 0, encabezado = 0;
  long long int productoEscalar = 0, productoEscalarParcial = 0;
  
  // estructuras para medir el tiempo de ejecución en microsegundos
  struct timeval inicio, fin; 
  // variables para medir el tiempo de ejecución en segundos
  clock_t iniSeg, finSeg; 

  // Inicializar MPI
  int miRango, numProcs;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &miRango);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

  // Se verifican los parámetros
  // TO DO: Se puede mejorar el control de parámetros. Pero no es 
  // el objeto de estudio
  if (miRango == 0) {
    if (argc < 2) {
        printf("No se han especificado valores correctos para los parámetros requeridos.\n");
        printf("La cantidad de elementos serán: %d sin formato CSV.\n", cantidadElementos);
        printf("Utilizando %d procesos\n", numProcs);
        printf("Uso: ProductoEscalarP <cantidadElementos> <csv> <encabezado>\n\n");
    } else {
      if (argc >= 2) {
        cantidadElementos = atoi(argv[1]);
      }
      if (argc >= 3) 
        outCsv=1;

      if (argc == 4)
        encabezado = 1;
    }
  }

  // Dividir el trabajo entre los procesos
  int elementosPorProceso = cantidadElementos / numProcs;
  int elementosRestantes = cantidadElementos % numProcs;

  // guardar el tiempo de inicio
  iniSeg = clock(); 
  gettimeofday(&inicio, NULL); 

  // Creo los vectores
  int* vectorA;
  int* vectorB;

  // Asignar memoria para los vectores 
  vectorA = (int*) malloc(elementosPorProceso * sizeof(int));
  vectorB = (int*) malloc(elementosPorProceso * sizeof(int));

  // Verificar si la asignación de memoria fue exitosa
  if ((vectorA == NULL) || (vectorB == NULL)) {
      printf("No se pudo asignar memoria\n");
      MPI_Finalize(); // Finalizar MPI
      exit(1);
  }

  //Cargar los vectores
  par = 0; 
  impar = 0;
  for (count = miRango * elementosPorProceso; count < (miRango + 1) * elementosPorProceso; ++count) {
    if (count % 2 == 0) { // si es par, lo agrega al VectorA
      vectorA[par] = count;
      par++; // aumentar el contador de números pares
    } else { // si es impar, lo agrega al VectorB
      vectorB[impar] = count;
      impar++; // aumentar el contador de números impares
    }
  }

  // Si hay elementos restantes, se procesan en el proceso 0
  if (miRango == 0 && elementosRestantes > 0) {
    for (count = cantidadElementos - elementosRestantes; count < cantidadElementos; ++count) {
      if (count % 2 == 0) { // si es par, lo agrega al VectorA
        vectorA[par] = count;
        par++; // aumentar el contador de números pares
      } else { // si es impar, lo agrega al VectorB
        vectorB[impar] = count;
        impar++; // aumentar el contador de números impares
      }
    }
  }

  // Calcular el producto escalar parcial
  productoEscalarParcial = 0;
  for (count = 0; count < elementosPorProceso; ++count) {
      productoEscalarParcial += vectorA[count] * vectorB[count];
  }

  // Sumar los productos escalares parciales de todos los procesos
  MPI_Reduce(&productoEscalarParcial, // Elemento a enviar
             &productoEscalar, // Variable donde se almacena la reunion de los datos
             1, // Cantidad de datos a reunir 
             MPI_LONG_LONG_INT, // Tipo del dato que se reunira
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

  // Mostrar el resultado
  if (miRango == 0) {
    if (outCsv != 1){
      printf("Ejecución paralela del producto escalar entre\n");
      printf("dos vectores de %d elementos es: %lli\n", cantidadElementos, productoEscalar);
      printf("Tiempo de ejecución: %f microsegundos (µs)\n", tiempo);
      printf("Tiempo de ejecución: %f segundos\n\n\n", tiempoSeg);
    } else {
      if (encabezado == 1)
        printf("cantidadElementos, cantidadProcesadores,productoEscalar,microsegundosEjec,segundosEjec\n");

      printf("%d,%d,%lli,%f,%f\n", cantidadElementos, numProcs, productoEscalar, tiempo, tiempoSeg);
    }
  }

  // Liberar la memoria asignada a los vectores 
  free(vectorA);
  free(vectorB);

  // Finalizar MPI
  MPI_Finalize();

  return 0;
}