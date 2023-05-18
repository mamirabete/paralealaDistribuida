/*
// gcc ProductoEscalarS.c -o ProductoEscalarS -Wall -pedantic
//
//
// Para realizar el producto escalar de dos vectores de 10000000 elementos 
// en ANSI C, se pueden utilizar las funciones malloc y free para asignar 
// y liberar memoria dinámica, respectivamente. Se puede crear un bucle for 
// para inicializar los elementos de los dos vectores y luego otro bucle for 
// para calcular el producto escalar.
// 
// En este ejemplo, se utiliza malloc para asignar memoria para los vectores 
// de 10000000 elementos. Se verifica si la asignación de memoria fue exitosa 
// y, si es así, se inicializan los elementos de los dos vectores utilizando 
// bucles for. Luego, se utiliza otro bucle for para calcular el producto 
// escalar de los dos vectores y se almacena en una variable llamada 
// producto_escalar. Finalmente, se imprime el producto escalar y se utiliza 
// free para liberar la memoria asignada para los vectores.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define CANTIDAD_DEF 100 // cantidad de elementos por defecto

int main(int argc, char *argv[]) {
  int cantidadElementos = CANTIDAD_DEF;
  int par, impar, count,  outCsv = 0, encabezado = 0;
  long long int productoEscalar = 0;
  
  struct timeval inicio, fin; // estructuras para medir el tiempo de ejecución en microsegundos
  clock_t iniSeg, finSeg; // variables para medir el tiempo de ejecución en segundos


  // Se verifican los parámetros
  // TO DO: Se puede mejorar el control de parámetros. Pero no es el onjeto de estudio
  if (argc < 2) {
    printf("No se han especificado valores correctos para los parámetros requeridos.\n");
    printf("La cantidad de elementos serán: %d sin formato CSV.\n", cantidadElementos);
    printf("Uso: ProductoEscalarS <cantidadElementos> <csv> <encabezado>\n\n");
  } else {
    if (argc >= 2) {
      cantidadElementos = atoi(argv[1]);
    }
    if (argc >= 3) 
      outCsv=1;

    if (argc == 4)
      encabezado = 1;
  }
  
  
  // guardar el tiempo de inicio
  iniSeg = clock(); 
  gettimeofday(&inicio, NULL); 

  
  // Creo los vectores
  int* vectorA;
  int* vectorB;

  // Asignar memoria para los vectores 
  vectorA = (int*) malloc(cantidadElementos * sizeof(int));
  vectorB = (int*) malloc(cantidadElementos * sizeof(int));

  // Verificar si la asignación de memoria fue exitosa
  if ((vectorA == NULL) || (vectorB == NULL)) {
      printf("No se pudo asignar memoria\n");
      exit(1);
  }

  //Cargar los vectores
  par = 0; 
  impar = 0;
  for (count = 0; count < (cantidadElementos * 2) - 1; ++count) {
    if (count % 2 == 0) { // si es par, lo agrega al VectorA
      vectorA[par] = count;
      par++; // aumentar el contador de números pares
    } else { // si es impar, lo agrega al VectorB
      vectorB[impar] = count;
      impar++; // aumentar el contador de números impares
    }
  }

  // Calculo del producto escalar entre dos vectores
  productoEscalar = 0;
  for (count = 0; count < cantidadElementos; ++count) {
      productoEscalar += vectorA[count] * vectorB[count];
  }

  // guardar el tiempo de finalización
  finSeg = clock(); 
  gettimeofday(&fin, NULL); 

  // calcular y mostrar el tiempo de ejecución en microsegundos
  double tiempo = (double)(fin.tv_usec - inicio.tv_usec) + (double)(fin.tv_sec - inicio.tv_sec) * 1000000;
  // calcular y mostrar el tiempo de ejecución en segundos
  double tiempoSeg = ((double) (finSeg - iniSeg)) / CLOCKS_PER_SEC;
  
  if (outCsv != 1){
    printf("Ejecución secuencial del producto escalar entre\n");
    printf("dos vectores de %d elementos es: %lli\n", cantidadElementos, productoEscalar);
    printf("Tiempo de ejecución: %f microsegundos (µs)\n", tiempo);
    printf("Tiempo de ejecución: %f segundos\n\n\n", tiempoSeg);
  } else {
    if (encabezado == 1)
      printf("cantidadElementos,productoEscalar,microsegundosEjec,segundosEjec\n");

    printf("%d,%lli,%f,%f\n", cantidadElementos, productoEscalar, tiempo, tiempoSeg);
  }

  // Liberar la memoria asignada a los vectores 
  free(vectorA);
  free(vectorB);

  return 0;
}