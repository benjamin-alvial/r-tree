#include <stdio.h>
#include <stdlib.h>

// Definición de estructuras

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
} Node;

typedef struct
{
    int x, y;
} Point;

// Funciones auxiliares para calcular posición en la Curva de Hilbert

static int rotate(int n, int x, int y, int rx, int ry)
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

Point getCenter(Rectangle rect)
{
    Point center;
    center.x = (rect.x1 + rect.x2) / 2;
    center.y = (rect.y1 + rect.y2) / 2;
    return center;
}

int compareHilbertValue(const void *a, const void *b)
{
    Rectangle *rectA = (Rectangle *)a;
    Rectangle *rectB = (Rectangle *)b;

    Point centerA = getCenter(*rectA);
    Point centerB = getCenter(*rectB);

    int d1 = xy2d(524288, centerA.x, centerA.y); // 524288 la potencia n^2 mas cercana al rango [0, 500000]
    int d2 = xy2d(524288, centerB.x, centerB.y);

    return d1 - d2;
}

void sortRectanglesByHilbertValue(Rectangle *rectangles, int n)
{
    qsort(rectangles, n, sizeof(Rectangle), compareHilbertValue);
}

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

Node *createLeafNode(Rectangle *rectangles, int M)
{
    Node *node = (Node *)malloc(sizeof(Node));
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

    node->mbr = (Rectangle){minX, minY, maxX, maxY, -1};
    return node;
}

Node *createParentNode(Node **childNodes, int M)
{
    Node *node = (Node *)malloc(sizeof(Node));
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

    node->mbr = (Rectangle){minX, minY, maxX, maxY, -1};
    node->data.childNodes = childNodes;
    return node;
}

int compareNodesByHilbertValue(const void *a, const void *b)
{
    Node *nodeA = *(Node **)a;
    Node *nodeB = *(Node **)b;
    return nodeA->mbr.index - nodeB->mbr.index;
}

Node *buildRTree(Rectangle *rectangles, int n, int size, int M)
{
    int numNodes = (n + M - 1) / M;
    Node **nodes = (Node **)malloc(numNodes * sizeof(Node *));

    for (int i = 0; i < numNodes; i++)
    {
        int remainingRectangles = n - i * M;
        int rectanglesInThisNode = (remainingRectangles < M) ? remainingRectangles : M;
        nodes[i] = createLeafNode(rectangles + i * M, rectanglesInThisNode);
        Point center = getCenter(nodes[i]->mbr);
        nodes[i]->mbr.index = xy2d(size, center.x, center.y);
    }

    qsort(nodes, numNodes, sizeof(Node *), compareNodesByHilbertValue);

    while (numNodes > 1)
    {
        int numParents = (numNodes + M - 1) / M;
        Node **parentNodes = (Node **)malloc(numParents * sizeof(Node *));

        for (int i = 0; i < numParents; i++)
        {
            int remainingNodes = numNodes - i * M;
            int nodesInThisNode = (remainingNodes < M) ? remainingNodes : M;
            parentNodes[i] = createParentNode(nodes + i * M, nodesInThisNode);
            Point center = getCenter(parentNodes[i]->mbr);
            parentNodes[i]->mbr.index = xy2d(size, center.x, center.y);
        }

        qsort(parentNodes, numParents, sizeof(Node *), compareNodesByHilbertValue);

        free(nodes);
        nodes = parentNodes;
        numNodes = numParents;
    }

    Node *root = nodes[0];
    free(nodes);
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

int main()
{
    int n = 100;
    int size = 524288;
    Rectangle *rectangles = readRectangles("rect_R.bin", &n);

    sortRectanglesByHilbertValue(rectangles, 524288);

    int M = 4;
    Node *root = buildRTree(rectangles, n, size, M);

    free(rectangles);
    freeTree(root);

    return 0;
}
