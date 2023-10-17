#include <stdio.h>

void printIntsFromFile(const char *filename, int x) {
    FILE *file;

    file = fopen(filename, "rb");  // Open the file in binary mode for reading

    long num;
    int count = 0;

    while (fread(&num, sizeof(long), 1, file) == 1) {
        printf("%ld ", num);
        count++;

        if (count == x) {
            printf("\n");
            count = 0;
        }
    }

    // Print a newline if the last line is not complete
    if (count > 0) {
        printf("\n");
    }

    fclose(file);
}
