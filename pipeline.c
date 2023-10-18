#include "generateRectangleFile.c"
#include "search.c"
#include <stdio.h>

// Powers of 2 for the size of original data rectangles
#define N_POW_INITIAL 10 
#define N_POW_FINAL 10

// Domain in which the rectangles are defined
#define DOMAIN_MIN 0
#define DOMAIN_MAX 500000

// Range of sizes of rectangles
#define R_SIDE_MIN 0
#define R_SIDE_MAX 100
#define Q_SIDE_MIN 0
#define Q_SIDE_MAX 10000

// Number of searches
#define Q_AMOUNT 100

// Update according to computer architecture:
// If B is the size of a memory block,
// then M = P/sizeof(long) - 4
#define M 4

int main() {

    // This file will be used to plot the performance data
    FILE *results_file = fopen("final_results.txt", "w");

    // Array for the powers of n to test
    int n_values_size = N_POW_FINAL-N_POW_INITIAL+1;
    long n_values[n_values_size]; // Array for values of n (powers of 2);
    for(i=0; i<n_values_size; i++) {
        n_values[i] = pow(2, i+N_POW_INITIAL);
    }

    // Iterate over the powers of n.
    for(int i=0; i<n_values_size; i++) {
        long n = n_values[i];
        generateRectangleFile("rect_R.bin", n, DOMAIN_MIN, DOMAIN_MAX, R_SIDE_MIN, R_SIDE_MAX);
        generateRectangleFile("rect_Q.bin", Q_AMOUNT, DOMAIN_MIN, DOMAIN_MAX, Q_SIDE_MIN, Q_SIDE_MAX);

        createTreeMethodOne("rect_R.bin", M); // Creates the tree_1.bin file
        createTreeMethodTwo("rect_R.bin", M); // Creates the tree_2.bin file
        createTreeMethodThree("rect_R.bin", M); // Creates the tree_3.bin file
        
        //double time_1 = searchRectangleFile("rect_Q.bin", "tree_1.bin", M);
        //double time_2 = searchRectangleFile("rect_Q.bin", "tree_2.bin", M);
        //double time_3 = searchRectangleFile("rect_Q.bin", "tree_3.bin", M);

        //fprintf(results_file, "%ld, %lf, %lf, %lf\n", n, time_3, time_3, time_3); // To graph in another software
        //fprintf(results_file, "%ld, %lf, %lf, %lf\n", n, time_1, time_2, time_3); // To graph in another software
    }

    fclose(results_file);
    return 0;

}
