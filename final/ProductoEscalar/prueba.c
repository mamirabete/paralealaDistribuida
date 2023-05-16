#include <stdio.h>
#include <stdlib.h>

int main() {
    int* vector1;
    int* vector2;
    long long int producto_escalar = 0;
    int i;

    // Asignar memoria para los vectores utilizando malloc
    vector1 = (int*) malloc(10000000 * sizeof(int));
    vector2 = (int*) malloc(10000000 * sizeof(int));

    // Verificar si la asignación de memoria fue exitosa
    if (vector1 == NULL || vector2 == NULL) {
        printf("No se pudo asignar memoria\n");
        exit(1);
    }

    // Inicializar los elementos del primer vector con los primeros 10000000 números impares
    for (i = 0; i < 10000000; i++) {
        vector1[i] = 2 * i + 1;
    }

    // Inicializar los elementos del segundo vector con los primeros 10000000 números pares
    for (i = 0; i < 10000000; i++) {
        vector2[i] = 2 * i;
    }

    // Calcular el producto escalar de los dos vectores
    for (i = 0; i < 10000000; i++) {
        producto_escalar += vector1[i] * vector2[i];
    }

    // Imprimir el producto escalar
    printf("El producto escalar de los dos vectores es: %lld\n", producto_escalar);

    // Liberar la memoria asignada para los vectores utilizando free
    free(vector1);
    free(vector2);

    return 0;
}
