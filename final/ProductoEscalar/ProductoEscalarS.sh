#!/bin/bash

# definir una función que ejecuta el programa  
# iterando a través de los argumentos ingresados
ejecutaPrograma() {
    # ejecutar el programa con los argumentos ingresados
    encab=0
    for arg in "$@"; do
        for repetir in 1 2 3; do
            if [ $encab -eq 0 ]; then
                ./ProductoEscalarS "$arg" csv encabezado
            else         
                ./ProductoEscalarS "$arg" csv
            fi
            encab=1
        done
    done
}

# verificar que se proporcionó al menos un argumento
if [ $# -lt 1 ]; then
    echo "Debe proporcionar al menos un argumento."
    echo "USO: $0 <CantidadElementos> <CantidadElementos>...<CantidadElementos>"
    exit 1
fi

# llamar a la función ejecutaPrograma con los argumentos proporcionados
ejecutaPrograma "$@"