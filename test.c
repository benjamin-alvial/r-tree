#include <stdio.h>
#include "generateRectangleFile.c"
//#include "intersection.c"
#include "search.c"
//#include "printIntsFromFile.c"

int main() {
    // Testing the correct generation of file of rectangles.
    printf("--------------\n");
    int n = 10;
    generateRectangleFile("rect_test.bin", n, 0, 500, 0, 10);

    FILE *file;
    char filename[] = "rect_test.bin";
    file = fopen(filename, "rb");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    int buffer[4];
    while (fread(buffer, sizeof(int), 4, file) == 4) {
        printf("%d %d %d %d\n", buffer[0], buffer[1], buffer[2], buffer[3]);
        printf("Side lengths: %d, %d \n", buffer[2] - buffer[0], buffer[3] - buffer[1]);
    }

    fclose(file);

    // Testing the correct result from an intersection of two rectangles.
    printf("--------------\n");
    printf("Positive tests...\n");
    int bool1 = Intersection(1, 1, 5, 5, 2, 2, 4, 4); // First rectangle contains second.
    int bool2 = Intersection(2, 2, 4, 4, 1, 1, 5, 5); // Second rectangle contains first.
    int bool3 = Intersection(1, 1, 4, 4, 2, 2, 5, 5); // Corner.
    int bool4 = Intersection(1, 1, 5, 5, 2, 0, 4, 2); // Line.
    printf("First contains second test: %d \n", bool1);
    printf("Second contains first test: %d \n", bool2);
    printf("Corner test: %d \n", bool3);
    printf("Line test: %d \n", bool4);

    printf("--------------\n");
    printf("Negative tests...\n");
    int bool5 = Intersection(1, 1, 5, 5, 10, 10, 20, 20); // Non-intersecting rectangles.
    printf("Non-intersecting rectangles test: %d \n", bool5);

    // Testing the correct search of a rectangle in a tree.
    printf("--------------\n");
    printf("Search test...\n");

    int M = 2;

    int R_data[] = {2, 2, 4, 4, 
                    3, 3, 7, 7, 
                    2, 9, 5, 10, 
                    4, 10, 5, 12, 
                    6, 1, 8, 2, 
                    6, 3, 10, 6, 
                    7, 7, 10, 9, 
                    8, 10, 10, 11};

    int node_size = sizeof(int)*6;

    int tree_data[] = {2, 1, 10, 12, node_size, 2*node_size,
                        2, 2, 5, 12, 3*node_size, 4*node_size,
                        6, 1, 10, 11, 5*node_size, 6*node_size,
                        2, 9, 5, 12, -3, -4,
                        2, 2, 5, 7, -1, -2, 
                        7, 7, 10, 11, -7, -8,
                        6, 1, 10, 6, -5, -6};

    size_t numElements_R = sizeof(R_data) / sizeof(R_data[0]);
    const char *fileName_R = "R_test.bin";
    FILE *file_R = fopen(fileName_R, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", fileName_R);
        return 1;
    }
    fwrite(R_data, sizeof(int), numElements_R, file_R);
    fclose(file_R);
    printf("Data has been written to %s\n", fileName_R);
    printIntsFromFile(fileName_R, 4);

    size_t numElements_tree = sizeof(tree_data) / sizeof(tree_data[0]);
    const char *fileName_tree = "tree_test.bin";
    FILE *file_tree = fopen(fileName_tree, "wb");
    if (file_tree == NULL) {
        fprintf(stderr, "Error opening file: %s\n", fileName_tree);
        return 1;
    }
    fwrite(tree_data, sizeof(int), numElements_tree, file_R);
    fclose(file_tree);
    printf("Data has been written to %s\n", fileName_tree);
    printIntsFromFile(fileName_tree, 6);

    FILE *file_tree_read = fopen(fileName_tree, "rb");
    printf("Expected indices: 1, 2, 5, 6\n");
    searchRectangleInTree(2, 1, 7, 4, file_tree, 0, M);
    fclose(file_tree_read);

    printf("--------------\n");
    return 0;
}
