#include <stdio.h>
#include <stdlib.h>
//#include "generateRectangleFile.c"
#include "printIntsFromFile.c"

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

typedef struct
{
    int x, y;
} Point;

Point getCenter(Rectangle rect)
{
    Point center;
    center.x = (rect.x1 + rect.x2) / 2;
    center.y = (rect.y1 + rect.y2) / 2;
    return center;
}

// Función de comparación para nodos basada en el centro de su MBR
int compareNodeCenters(const void *a, const void *b)
{
    Node *nodeA = *(Node **)a;
    Node *nodeB = *(Node **)b;
    Point centerA = getCenter(nodeA->mbr);
    Point centerB = getCenter(nodeB->mbr);
    return centerA.x - centerB.x;
}

// Ordena nodos según el centro de su MBR
void sortNodesByCenter(Node **nodes, int n)
{
    qsort(nodes, n, sizeof(Node *), compareNodeCenters);
}

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

    *n = fileSize / (4 * sizeof(int)); // Corrected the number of rectangles

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

    node->mbr = (Rectangle){minX, minY, maxX, maxY, -1};
    return node;
}

Node *createParentNode(Node **childNodes, int M)
{
    Node *node = (Node *)malloc(sizeof(Node));
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

    int numLeaves = (n + M - 1) / M; // Corrected numLeaves calculation
    // printf("%d\n", numLeaves);
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
        
        // Write the four coordinates of the leaf rectangle
        int numbers_insert[4] = {leafNodes[i]->mbr.x1, leafNodes[i]->mbr.y1, leafNodes[i]->mbr.x2, leafNodes[i]->mbr.y2};
        leafNodes[i]->position = ftell(final_tree);
        fwrite(numbers_insert, sizeof(int), 4, final_tree);

        for (int j = 0; j < 4; j++)
        {
            // Write the indices of the leaf pointers to data rectangles
            int idx = leafNodes[i]->data.rectangles[j].index - 1; // Indices used in search start with 1
            fwrite(&idx, sizeof(int), 1, final_tree);
            printf("Rectangulo %d de la hoja %d\n", j, i);
            printf("x1: %d y1: %d x2: %d y2: %d index: %d\n", leafNodes[i]->data.rectangles[j].x1, leafNodes[i]->data.rectangles[j].y1, leafNodes[i]->data.rectangles[j].x2, leafNodes[i]->data.rectangles[j].y2, leafNodes[i]->data.rectangles[j].index);
        }
    }

    sortNodesByCenter(leafNodes, numLeaves);

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
            
            // Write the four coordinates of the node rectangle
            int numbers_insert[4] = {parentNodes[i]->mbr.x1, parentNodes[i]->mbr.y1, parentNodes[i]->mbr.x2, parentNodes[i]->mbr.y2};
            parentNodes[i]->position = ftell(final_tree);

            // Write the positions in bytes of the node's children
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

        sortNodesByCenter(parentNodes, numParents);

        // En la proxima iteracion, los nodos padres seran combinados en un nivel superior
        //free(leafNodes);
        leafNodes = parentNodes;
        numLeaves = numParents;
    }

    // La raiz del R-tree es el ultimo nodo
    Node *root = leafNodes[0];

    // Write the root node at the beginning of the file, first its coordinates
    fseek(final_tree, 0, SEEK_SET);
    int numbers_insert[] = {root->mbr.x1, root->mbr.y1, root->mbr.x2, root->mbr.y2};
    fwrite(numbers_insert, sizeof(int), 4, final_tree);

    // Write the positions of the root's children
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

    //free(leafNodes);
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

int test_m1()
{

    int M = 4;
    int n = 100;

    printf("Generation of rectangles for testing...\n");
    printf("--------------\n");
    //generateRectangleFile("rect_test.bin", n, 0, 100, 0, 5);
    printIntsFromFile("rect_test.bin", 4);

    printf("-------------\n");
    printIntsFromFile("final_tree.bin", M+4);

    // Leer rectangulos del binario.
    Rectangle *rectangles = readRectangles("rect_test.bin", &n);

    // Ordenar los rectangulos por el centro de su coordenada X
    sortRectangles(rectangles, n);

    // Construir el R-tree con un M especifico
    
    Node *root = buildRTree(rectangles, n, M);
    printf("-------------\n");
    printf("The tree has been successfully created.\n");

    // Se libera la memoria del arbol y los rectangulos guardados
    //free(rectangles);
    //freeTree(root);

    return 0;
}

int createTreeMethodOne(char *tree_file_name, int n, int M) {
    // Leer rectangulos del binario.
    Rectangle *rectangles = readRectangles(tree_file_name, &n);
    // Ordenar los rectangulos por el centro de su coordenada X
    sortRectangles(rectangles, n);
    // Construir el R-tree con un M especifico
    Node *root = buildRTree(rectangles, n, M);
}