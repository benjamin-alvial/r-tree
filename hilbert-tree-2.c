#include <stdio.h>
#include <stdlib.h>
//#include "printIntsFromFile.c"

// Definición de estructuras

typedef struct
{
    int x1, y1, x2, y2, index;
} RectangleH;

typedef struct NodeH
{
    RectangleH mbr;
    union
    {
        struct NodeH **childNodes; // para nodos internos
        RectangleH *rectangles;    // para nodos hoja
    } data;
    int numChildren;
    int isLeaf;
    int position; // posición donde se escribe en el archivo, en bytes
} NodeH;

typedef struct
{
    int x, y;
} PointH;

// Funciones auxiliares para calcular posición en la Curva de Hilbert

int rotate(int n, int x, int y, int rx, int ry)
{
    if (ry == 0)
    {
        if (rx == 1)
        {
            x = n - 1 - x;
            y = n - 1 - y;
        }
        return y * n + x;
    }
    return x;
}

int xy2d(int n, int x, int y)
{
    int rx, ry, s, d = 0;
    for (s = n / 2; s > 0; s /= 2)
    {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        x = rotate(s, x, y, rx, ry);
    }
    return d;
}

PointH getCenterH(RectangleH rect)
{
    PointH center;
    center.x = (rect.x1 + rect.x2) / 2;
    center.y = (rect.y1 + rect.y2) / 2;
    return center;
}

int compareHilbertValue(const void *a, const void *b)
{
    RectangleH *rectA = (RectangleH *)a;
    RectangleH *rectB = (RectangleH *)b;

    PointH centerA = getCenterH(*rectA);
    PointH centerB = getCenterH(*rectB);

    int d1 = xy2d(524288, centerA.x, centerA.y); // 524288 la potencia n^2 mas cercana al rango [0, 500000]
    int d2 = xy2d(524288, centerB.x, centerB.y);

    return d1 - d2;
}

void sortRectanglesByHilbertValue(RectangleH *rectangles, int n)
{
    qsort(rectangles, n, sizeof(RectangleH), compareHilbertValue);
}

RectangleH *readRectanglesH(const char *filename, int *n)
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

    RectangleH *rectangles = (RectangleH *)malloc(*n * sizeof(RectangleH));

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

NodeH*createLeafNodeH(RectangleH *rectangles, int M)
{
    NodeH*node = (NodeH*)malloc(sizeof(Node));
    node->data.rectangles = rectangles;
    node->numChildren = M;
    node->isLeaf = 1;

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

    node->mbr = (RectangleH){minX, minY, maxX, maxY, -1};
    return node;
}

NodeH*createParentNodeH(NodeH**childNodes, int M)
{
    NodeH*node = (NodeH*)malloc(sizeof(Node));
    node->numChildren = M;
    node->isLeaf = 0;

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

    node->mbr = (RectangleH){minX, minY, maxX, maxY, -1};
    node->data.childNodes = childNodes;
    return node;
}

int compareNodesByHilbertValue(const void *a, const void *b)
{
    NodeH*nodeA = *(NodeH**)a;
    NodeH*nodeB = *(NodeH**)b;
    return nodeA->mbr.index - nodeB->mbr.index;
}

NodeH*buildRTreeH(RectangleH *rectangles, int n, int size, int M, char *tree_file_name)
{
    FILE *final_tree;
    final_tree = fopen(tree_file_name, "wb");

    if (final_tree == NULL) {
        printf("Error opening fileeeee\n");
     // Exit with an error code
    }

    // Saltarse la raíz por mientras
    fseek(final_tree, (M+4)*sizeof(int), SEEK_SET);

    int numNodes = (n + M - 1) / M;
    NodeH **nodes = (NodeH **)malloc(numNodes * sizeof(NodeH *));

    for (int i = 0; i < numNodes; i++)
    {
        int remainingRectangles = n - i * M;
        int rectanglesInThisNode = (remainingRectangles < M) ? remainingRectangles : M;
        nodes[i] = createLeafNodeH(rectangles + i * M, rectanglesInThisNode);
        PointH center = getCenterH(nodes[i]->mbr);
        nodes[i]->mbr.index = xy2d(size, center.x, center.y);

        nodes[i] = createLeafNodeH(rectangles + i * M, rectanglesInThisNode);
        nodes[i]->mbr.index = i;

        //printf("leaf rectangle %d: x1= %d y1 = %d x2= %d y2 = %d \n", i, leafNodes[i]->mbr.x1, leafNodes[i]->mbr.y1, leafNodes[i]->mbr.x2, leafNodes[i]->mbr.y2);
        
        // Write the four coordinates of the leaf rectangle
        int numbers_insert[4] = {nodes[i]->mbr.x1, nodes[i]->mbr.y1, nodes[i]->mbr.x2, nodes[i]->mbr.y2};
        nodes[i]->position = ftell(final_tree);
        fwrite(numbers_insert, sizeof(int), 4, final_tree);

        for (int j = 0; j < M; j++)
        {

            int idx = nodes[i]->data.rectangles[j].index - 1; // Indices used in search start with 1
            fwrite(&idx, sizeof(int), 1, final_tree);
            //printf("Rectangulo %d de la hoja %d\n", j, i);
            //printf("x1: %d y1: %d x2: %d y2: %d index: %d\n", leafNodes[i]->data.rectangles[j].x1, leafNodes[i]->data.rectangles[j].y1, leafNodes[i]->data.rectangles[j].x2, leafNodes[i]->data.rectangles[j].y2, leafNodes[i]->data.rectangles[j].index);

        }

    }

    qsort(nodes, numNodes, sizeof(NodeH *), compareNodesByHilbertValue);

    while (numNodes > 1)
    {
        int numParents = (numNodes + M - 1) / M;
        NodeH **parentNodes = (NodeH **)malloc(numParents * sizeof(NodeH *));

        for (int i = 0; i < numParents; i++)
        {
            int remainingNodes = numNodes - i * M;
            int nodesInThisNode = (remainingNodes < M) ? remainingNodes : M;
            parentNodes[i] = createParentNodeH(nodes + i * M, nodesInThisNode);
            PointH center = getCenterH(parentNodes[i]->mbr);
            parentNodes[i]->mbr.index = xy2d(size, center.x, center.y);

            parentNodes[i] = createParentNodeH(nodes + i * M, nodesInThisNode);
            parentNodes[i]->mbr.index = i;

            //printf("parent rectangle %d: x1= %d y1 = %d x2= %d y2 = %d \n", i, parentNodes[i]->mbr.x1, parentNodes[i]->mbr.y1, parentNodes[i]->mbr.x2, parentNodes[i]->mbr.y2);
            
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
                    int noChild = 0;
                    fwrite(&noChild, sizeof(int), 1, final_tree);
                }
                
            }

        }

        qsort(parentNodes, numParents, sizeof(NodeH *), compareNodesByHilbertValue);

        free(nodes);
        nodes = parentNodes;
        numNodes = numParents;
    }

    fclose(final_tree);

    FILE *final_tree2 = fopen(tree_file_name, "rb+");

     // Determine the size of the file
    fseek(final_tree2, 0, SEEK_END);
    long file_size = ftell(final_tree2);
    printf("file size: %ld\n", file_size);

    // Move back by the size of 8 integers
    fseek(final_tree2, -(M+4) * sizeof(int), SEEK_END);

    // Read the last 8 integers
    int last_8_ints[M+4];
    
    fread(last_8_ints, sizeof(int), M+4, final_tree2);
    
    // Move back to the beginning of the file
    fseek(final_tree2, 0, SEEK_SET);

    // Write the last 8 integers to the beginning of the file
    fwrite(last_8_ints, sizeof(int), M+4, final_tree2);
    fclose(final_tree2);

    NodeH *root = nodes[0];
    free(nodes);
    return root;
}

void freeTreeH(NodeH *node)
{
    if (!node->isLeaf)
    {
        for (int i = 0; i < node->numChildren; i++)
        {
            freeTreeH(node->data.childNodes[i]);
        }
        free(node->data.childNodes);
    }
    free(node);
}

/*
int test_m2()
{
    int n = 100;
    int size = 524288;
    RectangleH *rectangles = readRectanglesH("rect_R.bin", &n);

    printIntsFromFile("rect_R.bin", 4);

    printf("pre sort\n");
    sortRectanglesByHilbertValue(rectangles, n);

    printf("sort done\n");
    int M = 4;
    NodeH *root = buildRTreeH(rectangles, n, size, M);

    printf("tree done\n");
    //free(rectangles);
    //freeTreeH(root);

    return 0;
}
*/

int createTreeMethodTwo(char *R_file_name, int n, int M, char *tree_file_name) {
    RectangleH *rectangles = readRectanglesH(R_file_name, &n);
    sortRectanglesByHilbertValue(rectangles, n);
    int size = 524288;
    NodeH *root = buildRTreeH(rectangles, n, size, M, tree_file_name);
}
