#include "generateRectangleFile.c"
#include "search.c"
//#include "x-tree-2.c"
#include "printIntsFromFile.c"
#include "hilbert-tree-2.c"

// There are two sets of rectangles.
// The data rectangles set (R) has the original rectangle data points.
// The query rectangles set (Q) has the rectangles for which 
// intersections with rectangles in R will be calculated.

// Number of data points (number of rectangles in R)
// follow powers of 2 from N_POW_INITIAL to N_POW_FINAL
#define N_POW_INITIAL 6
#define N_POW_FINAL 6

// Number of queries (number of rectangles in Q)
#define Q_AMOUNT 100

// Domain in which the rectangles are defined
#define DOMAIN_MIN 0
#define DOMAIN_MAX 500000

// Range of sizes of rectangles data (R) and query (Q)
#define R_SIDE_MIN 0
#define R_SIDE_MAX 100
#define Q_SIDE_MIN 0
#define Q_SIDE_MAX 100000

// Update according to computer architecture:
// If B is the size of a memory block,
// it must contain exactly 4 ints for rectangle and M ints for children pointers,
// Then, B = (4+M)*sizeof(int) => M = B/sizeof(int) - 4.
// In the machine used, B = 512 bytes, sizeof(int) = 4,
// then M = 512/4 - 4 = 124
#define M 4

int main() {

    // This file will be used to plot the performance data
    FILE *results_file = fopen("performance_results.txt", "w");

    // Array for the values of n (powers of 2)
    int n_values_size = N_POW_FINAL-N_POW_INITIAL+1;
    long n_values[n_values_size];
    for(int i=0; i<n_values_size; i++) {
        n_values[i] = pow(2, i+N_POW_INITIAL);
    }

    // Iterate over the values of n (powers of 2)
    for(int i=0; i<n_values_size; i++) {
        int n = n_values[i];
        // Generate the data rectangles set (R) and the query rectangles set (Q)
        generateRectangleFile("rect_R.bin", n, DOMAIN_MIN, DOMAIN_MAX, R_SIDE_MIN, R_SIDE_MAX);
        printf("----- Set R of rectangles successfully created. -----\n");
        printIntsFromFile("rect_R.bin", 4);

        generateRectangleFile("rect_Q.bin", Q_AMOUNT, DOMAIN_MIN, DOMAIN_MAX, Q_SIDE_MIN, Q_SIDE_MAX);
        printf("----- Set Q of rectangles successfully created. -----\n");
        printIntsFromFile("rect_Q.bin", 4);
        
        // Generate the R-Tree with three different methods
        //createTreeMethodOne("rect_R.bin", n, M, "tree_1.bin");
        //printf("----- Tree with method 1 (Nearest-X) successfully created. -----\n");
        //printIntsFromFile("tree_1.bin", 4+M);

        createTreeMethodTwo("rect_R.bin", n, M, "tree_2.bin"); // Creates the tree_2.bin file
        printf("----- Tree with method 2 (Hilbert Curve) successfully created. -----\n");
        printIntsFromFile("tree_2.bin", 4+M);

        //createTreeMethodThree("rect_R.bin", M); // Creates the tree_3.bin file
        //printf("----- Tree with method 3 (Sort-Tile-Recursive) successfully created. -----\n");
        //printIntsFromFile("tree_3.bin", 4+M);

        // Queries of rectangles in Q are done for each tree and performance times are measured
        //double time_1 = searchRectangleFile("rect_Q.bin", "tree_1.bin", M);
        //printf("----- Time taken for queries in first tree in seconds: %lf -----\n", time_1);
        double time_2 = searchRectangleFile("rect_Q.bin", "tree_2.bin", M);
        printf("----- Time taken for queries in second tree in seconds: %lf -----\n", time_2);
        //double time_3 = searchRectangleFile("rect_Q.bin", "tree_3.bin", M);
        //printf("----- Time taken for queries in third tree in seconds: %lf -----\n", time_3);

        // Write performance data for each n and tree to plot elsewhere
        //fprintf(results_file, "%d, %lf, %lf, %lf\n", n, time_1, time_1, time_1);
        //fprintf(results_file, "%d, %lf, %lf, %lf\n", n, time_1, time_2, time_3);
    }

    fclose(results_file);
    return 0;

}
