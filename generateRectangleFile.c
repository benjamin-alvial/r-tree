#include <math.h>
#include <stdlib.h>

// Creates and saves a binary file with n rectangles.
// Every four integers correspond to a rectangle's two vertices (x1, y1, x2, y2) that determine it.
// The rectangles live in the specified domain and have side lengths in the specified range.
void generateRectangleFile(char* file_name, long n, int domain_min, int domain_max, int side_min, int side_max) {

    FILE *file = fopen(file_name, "wb");

    int i = 0;
    while (i < n) {
        long x1 = rand() % (domain_max - domain_min + 1) + domain_min;
        long y1 = rand() % (domain_max - domain_min + 1) + domain_min;

        long dx = rand() % (side_max - side_min+ 1) + side_min;
        long dy = rand() % (side_max - side_min+ 1) + side_min;

        long x2 = x1 + dx;
        long y2 = y1 + dy;

        // Only consider rectangles that don't escape from the domain.
        if (x2 < domain_max || y2 < domain_max) {
            long numbers_insert[] = {x1, y1, x2, y2};
            fwrite(numbers_insert, sizeof(long), 4, file);
            i++;
        } 
    }

    fclose(file);
    return;
}
