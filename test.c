#include <stdio.h>
#include "generateRectangleFile.c"
#include "intersection.c"

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
    printf("--------------\n");

    return 0;
}
