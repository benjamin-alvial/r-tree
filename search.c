#include <time.h>
#include "intersection.c"

void searchRectangleInTree(long x1, long y1, long x2, long y2, FILE *file, long initial_fp_pos, int M);
double searchRectangleFile(char *Q_file_name, char *tree_file_name, int M);

// Searches the entirety of the rectangles in Q_file_name
// in the tree saved in tree_file_name, returning the cpu time used.
double searchRectangleFile(char *Q_file_name, char *tree_file_name, int M) {

    FILE *Q_file;
    FILE *tree_file;

    // Save the contents of the Q_file in an array.
    long *array_of_ints;
    Q_file = fopen(Q_file_name, "rb");
    fseek(Q_file, 0, SEEK_END);
    long number_of_ints = ftell(Q_file) / sizeof(long);
    rewind(Q_file);
    array_of_ints = (long *) malloc(number_of_ints * sizeof(long));
    fread(array_of_ints , sizeof(long), number_of_ints, Q_file);

    
    // The performance of the search of all the rectangles in Q is measured.
    tree_file= fopen(tree_file_name, "rb");
    clock_t start_time, end_time;

    start_time = clock();

    for (int i=0; i < number_of_ints; i+=4) {
        long x1 = array_of_ints[i];
        long y1 = array_of_ints[i+1];
        long x2 = array_of_ints[i+2];
        long y2 = array_of_ints[i+3];
        // Search the rectangle in the root of the tree, at position 0 bytes.
        searchRectangleInTree(x1, y1, x2, y2, tree_file, 0, M);
    }

    end_time = clock();

    free(array_of_ints);
    double cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    return cpu_time_used;

}

// Struct that returns the result of a search of a single rectangle in a given tree.
struct searchResult {
    long *intersected_rectangles; //array
    long number_accesses;
};

// Searches a single rectangle in a given tree (contained in the file),
// starting the search from the given file pointer position.
// The file must be organized as
// x1 y1 x2 y2 p1 p2 ... pM (...) x1 y1 x2 y2 p1 p2 ... pM,
// that is, four numbers representing the rectangle followed by M pointers 
// representing positions in the file where its children are located.
// The leaves point to the negative indices of rectangles in the original data R_rect file.
// Negative numbers in leaves are used to identify leaves in recursion.
void searchRectangleInTree(long x1, long y1, long x2, long y2, FILE *file, long initial_fp_pos, int M) {
//struct searchResult searchRectangleInTree(int x1, int y1, int x2, int y2, FILE *file, int intial_fp_pos, int M) {

    // Extract the rectangle's information at the given node.
    long x1_node;
    long y1_node;
    long x2_node;
    long y2_node;
    fseek(file, initial_fp_pos, SEEK_SET);
    fread(&x1_node, sizeof(long), 1, file);
    fread(&y1_node, sizeof(long), 1, file);
    fread(&x2_node, sizeof(long), 1, file);
    fread(&y2_node, sizeof(long), 1, file);

    // Extract array of file pointers to children
    long children_fp[M];
    for(int k=0; k<M; k++) {
        fread(&children_fp[k], sizeof(long), 1, file);
    }

    if (children_fp[0] < 0) { // Leaf
        for(int i =0; i<M; i++) { // Print each index of an intersected rectangle.
            printf("Intersected rectangle at index %ld\n", -children_fp[i]);
            /*
            children_fp[k] = (-pos_index - 1)*sizeof(int); // Change -index to position in file.
            */
        }
    }

    else { // Internal node
        for(int i =0; i<M; i++) { // Search in recursion in subtree if need be.
            long child_fp = children_fp[i];
            if (child_fp != 0) { // 0 child means no child.
                long x1_child;
                long y1_child;
                long x2_child;
                long y2_child;
                fseek(file, child_fp, SEEK_SET);
                fread(&x1_child, sizeof(long), 1, file);
                fread(&y1_child, sizeof(long), 1, file);
                fread(&x2_child, sizeof(long), 1, file);
                fread(&y2_child, sizeof(long), 1, file);

                int intersection_bool = Intersection(x1, y1, x2, y2, x1_child, y1_child, x2_child, y2_child);

                if(intersection_bool == 1) {
                    searchRectangleInTree(x1, y1, x2, y2, file, child_fp, M);
                }

            }

        }  
    }
}
   
