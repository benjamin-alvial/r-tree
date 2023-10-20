# r-tree
Se implementan tres métodos de construcción para árboles R: Nearest-X (1), curva de Hilbert (2) y Sort-Tile-Recursive (3). Luego, se realizan consultas sobre estos árboles y se miden los tiempos de búsqueda y las cantidades de accesos.

## Pipeline
En `pipeline.c` está el flujo general de la construcción y de las consultas. En él, se deben especificar los parámetros del problema.

Al compilar y ejecutarse, se itera sobre las potencias de 2 dadas y para cada una de ellas:
- Se genera un archivo binario con enteros que representan los rectángulos dato (R).
- Se genera un archivo binario con enteros que representan los rectángulos con los que se realizan consultas (Q).
- Se construye un árbol 1 con la data de R mediante el método Nearest-X.
- Se construye un árbol 2 con la data de R mediante el método curva de Hilbert.
- Se construye un árbol 3 con la data de R mediante el método Sort-Tile-Recursive.
- Se ejecutan las consultas del archivo Q sobre el árbol 1 y se mide el tiempo de búsqueda y la cantidad de accesos.
- Se ejecutan las consultas del archivo Q sobre el árbol 2 y se mide el tiempo de búsqueda y la cantidad de accesos.
- Se ejecutan las consultas del archivo Q sobre el árbol 3 y se mide el tiempo de búsqueda y la cantidad de accesos.
- Se escribe en un archivo de texto la tupla que contiene a n, los 3 tiempos de búsqueda y las 3 cantidades de accesos.

## Generación de rectángulos
En `generateRectangleFile.c` está la implementación de la generación de un archivo de rectángulos, utilizada para crear R y Q.

## Construcción
En `x-tree-2.c` está la implementación del método Nearest-X.
En `hilbert-tree-2.c` está la implementación del método curva de Hilbert.
En `STR-ints.c` está la implementación del método Sort-Tile-Recursive.

## Búsqueda
En `search.c` está la implementación de la búsqueda de un conjunto de rectángulos Q en otro conjunto de rectángulos R, que funciona a partir de la búsqueda iterativa de cada rectángulo en Q sobre el conjunto R. En `intersection.c` se implementa una función que permite decidir si dos rectángulos tienen intersección no vacía.

## Resultados
En `exp_fit.py` está la implementación para encontrar los ajustes polinomiales buscados a partir del archivo de texto.