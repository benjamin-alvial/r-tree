#include <stdio.h>
#include <stdlib.h>
#include "generateRectangleFile.c"
#include "printIntsFromFile.c"


FILE *final_tree;

// PART 1: ESTRUCTURA Y SORTING DE LOS RECTANGULOS INICIALES

typedef struct
{
    int x1, y1, x2, y2, index;
} Rectangle;

typedef struct Node
{
    Rectangle mbr;
    union
    {
        struct Node **childNodes; // para nodos internos
        Rectangle *rectangles;    // para nodos hoja
    } data;
    int numChildren;
    int isLeaf;
    int position; // posición donde se escribe en el archivo, en bytes
} Node;

int compareRectangles(const void *a, const void *b)
{
    Rectangle *rectA = (Rectangle *)a;
    Rectangle *rectB = (Rectangle *)b;
    int centerAX = (rectA->x1 + rectA->x2) / 2;
    int centerBX = (rectB->x1 + rectB->x2) / 2;
    return centerAX - centerBX;
}

void sortRectangles(Rectangle *rectangles, int n)
{
    qsort(rectangles, n, sizeof(Rectangle), compareRectangles);
}

int compareMBRCenterX(const void *a, const void *b)
{
    Node *nodeA = *(Node **)a;
    Node *nodeB = *(Node **)b;

    int centerX_A = (nodeA->mbr.x1 + nodeA->mbr.x2) / 2;
    int centerX_B = (nodeB->mbr.x1 + nodeB->mbr.x2) / 2;

    return centerX_A - centerX_B; // Para orden ascendente
}

// PARTE 2: LEER EL INPUT DE RECTANGULOS

Rectangle *readRectangles(const char *filename, int *n)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    *n = fileSize / (4 * sizeof(int));
    Rectangle *rectangles = (Rectangle *)malloc(*n * sizeof(Rectangle));

    for (int i = 0; i < *n; i++)
    {
        fread(&(rectangles[i].x1), sizeof(int), 1, file);
        fread(&(rectangles[i].y1), sizeof(int), 1, file);
        fread(&(rectangles[i].x2), sizeof(int), 1, file);
        fread(&(rectangles[i].y2), sizeof(int), 1, file);
        rectangles[i].index = -i;
    }

    fclose(file);
    return rectangles;
}

// PARTE 3: CREAR EL R-TREE

Node *createLeafNode(Rectangle *rectangles, int M)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data.rectangles = rectangles; // Guardar los rectangulos
    node->numChildren = M;
    node->isLeaf = 1;

    // Determinar el MBR que agrupa a todos los rectangulos
    int minX = rectangles[0].x1;
    int minY = rectangles[0].y1;
    int maxX = rectangles[0].x2;
    int maxY = rectangles[0].y2;

    for (int i = 1; i < node->numChildren; i++)
    {
        minX = rectangles[i].x1 < minX ? rectangles[i].x1 : minX;
        minY = rectangles[i].y1 < minY ? rectangles[i].y1 : minY;
        maxX = rectangles[i].x2 > maxX ? rectangles[i].x2 : maxX;
        maxY = rectangles[i].y2 > maxY ? rectangles[i].y2 : maxY;
    }

    //printf("minx and min x: %d, %d\n", minX, minY);
    node->mbr = (Rectangle){minX, minY, maxX, maxY, -1};
    return node;
}

Node *createParentNode(Node **childNodes, int M)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data.childNodes = childNodes; // Guardar los nodos hijos
    node->numChildren = M;
    node->isLeaf = 0;

    // Determinar el MBR que agrupa a todos los MBRs hijos
    int minX = childNodes[0]->mbr.x1;
    int minY = childNodes[0]->mbr.y1;
    int maxX = childNodes[0]->mbr.x2;
    int maxY = childNodes[0]->mbr.y2;

    for (int i = 1; i < node->numChildren; i++)
    {
        minX = childNodes[i]->mbr.x1 < minX ? childNodes[i]->mbr.x1 : minX;
        minY = childNodes[i]->mbr.y1 < minY ? childNodes[i]->mbr.y1 : minY;
        maxX = childNodes[i]->mbr.x2 > maxX ? childNodes[i]->mbr.x2 : maxX;
        maxY = childNodes[i]->mbr.y2 > maxY ? childNodes[i]->mbr.y2 : maxY;
    }

    node->mbr = (Rectangle){minX, minY, maxX, maxY, -1};
    node->data.childNodes = childNodes;
    return node;
}

Node *buildRTree(Rectangle *rectangles, int n, int M)
{

    FILE *final_tree;
    final_tree = fopen("final_tree.bin", "wb");

    if (final_tree == NULL) {
        printf("Error opening fileeeee\n");
     // Exit with an error code
    }

    // Saltarse la raíz por mientras
    fseek(final_tree, (M+4)*sizeof(int), SEEK_SET);


    int numLeaves = n / M + n % M;
    printf("%d\n", numLeaves);
    Node **leafNodes = (Node **)malloc(numLeaves * sizeof(Node *));

    // Crear los nodos hojas
    for (int i = 0; i < numLeaves; i++)
    {
        printf("-------------\n");
        printf("Creating leaf node %d\n", i);

        int remainingNodes = numLeaves - i * M;
        int nodesInThisNode = (remainingNodes < M) ? remainingNodes : M;

        leafNodes[i] = createLeafNode(rectangles + i * M, nodesInThisNode);
        leafNodes[i]->mbr.index = i;

        printf("leaf rectangle %d: x1= %d y1 = %d x2= %d y2 = %d \n", i, leafNodes[i]->mbr.x1, leafNodes[i]->mbr.y1, leafNodes[i]->mbr.x2, leafNodes[i]->mbr.y2);
        int numbers_insert[4] = {leafNodes[i]->mbr.x1, leafNodes[i]->mbr.y1, leafNodes[i]->mbr.x2, leafNodes[i]->mbr.y2};
        leafNodes[i]->position = ftell(final_tree);
        fwrite(numbers_insert, sizeof(int), 4, final_tree);

        for (int j = 0; j < 4; j++)
        {
            int idx = leafNodes[i]->data.rectangles[j].index - 1; // Indices used in search start with 1
            fwrite(&idx, sizeof(int), 1, final_tree);
        }
            
        for (int j = 0; j < 4; j++)
        {
            printf("Rectangulo %d de la hoja %d\n", j, i);
            printf("x1: %d y1: %d x2: %d y2: %d\n", leafNodes[i]->data.rectangles[j].x1, leafNodes[i]->data.rectangles[j].y1, leafNodes[i]->data.rectangles[j].x2, leafNodes[i]->data.rectangles[j].y2);
        }
    }

    // qsort(leafNodes, numLeaves, sizeof(Node*), compareMBRCenterX);
    for (int i = 0; i < numLeaves; i++)
    {
        printf("MBR x1: %d y1: %d x2: %d y2: %d\n index %d\n", leafNodes[i]->mbr.x1, leafNodes[i]->mbr.y1, leafNodes[i]->mbr.x2, leafNodes[i]->mbr.y2, leafNodes[i]->mbr.index);
    }

    // Construir el R-tree
    while (numLeaves > 1)
    {
        int numParents = (numLeaves + M - 1) / M;
        printf("Num parents: %d\n", numParents);
        Node **parentNodes = (Node **)malloc(numParents * sizeof(Node *));

        for (int i = 0; i < numParents; i++)
        {
            printf("-------------\n");
            printf("Creating parent node %d\n", i);

            int remainingNodes = numLeaves - i * M;                          // Number de nodos faltantes para ser procesados
            int nodesInThisNode = (remainingNodes < M) ? remainingNodes : M; // Se usan los que quedan si son menos de M

            parentNodes[i] = createParentNode(leafNodes + i * M, nodesInThisNode);
            parentNodes[i]->mbr.index = i;
            printf("parent rectangle %d: x1= %d y1 = %d x2= %d y2 = %d \n", i, parentNodes[i]->mbr.x1, parentNodes[i]->mbr.y1, parentNodes[i]->mbr.x2, parentNodes[i]->mbr.y2);
            int numbers_insert[4] = {parentNodes[i]->mbr.x1, parentNodes[i]->mbr.y1, parentNodes[i]->mbr.x2, parentNodes[i]->mbr.y2};
            parentNodes[i]->position = ftell(final_tree);
            
            fwrite(numbers_insert, sizeof(int), 4, final_tree);
            for (int j = 0; j<M; j++)
            {
                if(j < parentNodes[i]->numChildren) {
                    int posChild = (parentNodes[i]->data.childNodes[j])->position;
                    fwrite(&posChild, sizeof(int), 1, final_tree);
                }
                else {
                    int noChild = -1;
                    fwrite(&noChild, sizeof(int), 1, final_tree);
                }
                
            }


        }

        // En la proxima iteracion, los nodos padres seran combinados en un nivel superior
        //free(leafNodes);
        leafNodes = parentNodes;
        numLeaves = numParents;
    }

    // La raiz del R-tree es el ultimo nodo
    Node *root = leafNodes[0];
    //free(leafNodes);

    // Escribir la raíz... ir al inicio
    fseek(final_tree, 0, SEEK_SET);
    int numbers_insert[] = {root->mbr.x1, root->mbr.y1, root->mbr.x2, root->mbr.y2};
    fwrite(numbers_insert, sizeof(int), 4, final_tree);

    for (int j = 0; j<M; j++)
            {
                if(j < root->numChildren) {
                    int posChild = (root->data.childNodes[j])->position;
                    fwrite(&posChild, sizeof(int), 1, final_tree);
                }
                else {
                    int noChild = -1;
                    fwrite(&noChild, sizeof(int), 1, final_tree);
                }
                
            }

    return root;
}

void freeTree(Node *node)
{
    if (!node->isLeaf)
    {
        for (int i = 0; i < node->numChildren; i++)
        {
            freeTree(node->data.childNodes[i]);
        }
        free(node->data.childNodes);
    }
    free(node);
}

void writeNode(FILE *file, Node *node, int *currentOffset)
{
    // Escribir MBR
    fwrite(&(node->mbr.x1), sizeof(int), 1, file);
    fwrite(&(node->mbr.y1), sizeof(int), 1, file);
    fwrite(&(node->mbr.x2), sizeof(int), 1, file);
    fwrite(&(node->mbr.y2), sizeof(int), 1, file);

    if (node->isLeaf)
    {
        // Si es un nodo hoja, escribir los índices de sus rectángulos hijos
        for (int i = 0; i < node->numChildren; i++)
        {
            fwrite(&(node->data.rectangles[i].index), sizeof(int), 1, file);
        }
    }
    else
    {
        // Si no es un nodo hoja, escribir las posiciones (offsets) de sus hijos
        // (inicialmente desconocidas, así que escribimos un valor temporal)
        int tempOffset = -1;
        for (int i = 0; i < node->numChildren; i++)
        {
            fwrite(&tempOffset, sizeof(int), 1, file);
        }

        // Recuerda la posición actual en el archivo
        int returnPosition = ftell(file);

        // Ahora, escribir cada hijo y actualizar la posición (offset) en el archivo
        for (int i = 0; i < node->numChildren; i++)
        {
            // Ir a la posición donde deberíamos actualizar el offset
            fseek(file, *currentOffset + sizeof(Rectangle) + i * sizeof(int), SEEK_SET);
            fwrite(currentOffset, sizeof(int), 1, file); // Escribir el offset actual

            // Regresar al final del archivo para continuar escribiendo
            fseek(file, 0, SEEK_END);
            writeNode(file, node->data.childNodes[i], currentOffset);
            *currentOffset = ftell(file);
        }

        // Regresar a la posición después de este nodo
        fseek(file, returnPosition, SEEK_SET);
    }
}

void saveRTreeToFile(Node *root, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        fprintf(stderr, "Cannot open file %s for writing\n", filename);
        return;
    }

    int currentOffset = sizeof(Rectangle);
    writeNode(file, root, &currentOffset);

    fclose(file);
}

Node *readNode(FILE *file, int offset, int M)
{
    fseek(file, offset, SEEK_SET);

    Node *node = (Node *)malloc(sizeof(Node));

    // Leer MBR
    fread(&(node->mbr.x1), sizeof(int), 1, file);
    fread(&(node->mbr.y1), sizeof(int), 1, file);
    fread(&(node->mbr.x2), sizeof(int), 1, file);
    fread(&(node->mbr.y2), sizeof(int), 1, file);

    // Determinar si el nodo es una hoja o un nodo interno
    int childOffset;
    fread(&childOffset, sizeof(int), 1, file);

    if (childOffset == -1)
    {
        // Es un nodo hoja
        node->isLeaf = 1;
        node->data.rectangles = (Rectangle *)malloc(M * sizeof(Rectangle)); // Asumiendo que M es una constante global
        for (int i = 0; i < M; i++)
        {
            fread(&(node->data.rectangles[i].index), sizeof(int), 1, file);
        }
    }
    else
    {
        // Es un nodo interno
        node->isLeaf = 0;
        node->data.childNodes = (Node **)malloc(M * sizeof(Node *));
        for (int i = 0; i < M; i++)
        {
            node->data.childNodes[i] = readNode(file, childOffset, 4);
            fread(&childOffset, sizeof(int), 1, file);
        }
    }

    return node;
}

void printTree(Node *node, int depth, int M)
{
    // Indentar según la profundidad
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }

    // Imprimir el MBR del nodo
    printf("MBR: [%d, %d, %d, %d]\n", node->mbr.x1, node->mbr.y1, node->mbr.x2, node->mbr.y2);

    if (node->isLeaf)
    {
        // Si es un nodo hoja, imprimir los índices de los rectángulos
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < depth + 1; j++)
            {
                printf("  ");
            }
            printf("Rectangle Index: %d\n", node->data.rectangles[i].index);
        }
    }
    else
    {
        // Si no es un nodo hoja, imprimir sus hijos (recursivamente)
        for (int i = 0; i < M; i++)
        {
            printTree(node->data.childNodes[i], depth + 1, 4);
        }
    }
}

void readAndPrintTree(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Cannot open file %s for reading\n", filename);
        return;
    }

    Node *root = readNode(file, 0, 4);
    fclose(file);

    printTree(root, 0, 4);
}

int main()
{

    int M = 4;
    int n = 20;

    printf("Generation of rectangles for testing...\n");
    printf("--------------\n");
    generateRectangleFile("rect_test.bin", n, 0, 100, 0, 5);
    printIntsFromFile("rect_test.bin", 4);

    printf("-------------\n");
    printIntsFromFile("final_tree.bin", M+4);

    // Leer rectangulos del binario.
    Rectangle *rectangles = readRectangles("rect_test.bin", &n);

    // Ordenar los rectangulos por el centro de su coordenada X
    sortRectangles(rectangles, n);

    // Escribir las hojas y los nodos internos
    // Construir el R-tree con un M especifico
    Node *root = buildRTree(rectangles, n, M);

    printf("The tree has been successfully created.\n");
    
    return 0;
}