#include <time.h>

double searchRectangleFile(char *Q_file_name, char *tree_file_name) {

    FILE *Q_file, tree_file;
    int *array_of_ints;

    file = fopen(Q_file_name, "rb");
    fseek(Q_file, 0, SEEK_END);
    number_of_ints = ftell(Q_file) / sizeof(int);
    rewind(Q_file);

    array_of_ints = (int *) malloc(number_of_ints * sizeof(int));
    fread(array_of_ints , sizeof(int), number_of_ints, Q_file);

    // At this point, the array_of_ints contains the contents of the file.
    
    // The performance of the search of all the rectangles in Q is measured.
    tree_file= fopen(tree_file_name, "rb");
    clock_t start_time, end_time;

    start_time = clock();

    for (i=0; i < number_of_ints; i+=4) {
        int x1 = array_of_ints[i];
        int y1 = array_of_ints[i+1];
        int x2 = array_of_ints[i+2];
        int y2 = array_of_ints[i+3];
        searchRectangleInTree(x1, y1, x2, y2, 0, tree_file);
    }

    end_time = clock();

    double cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    return cpu_time_used;

}

struct searchResult {
    int *intersected_rectangles; //array
    int number_accesses;
}

struct searchResult searchRectangleInTree(int x1, int y1, int x2, int y2, int intial_fp_pos, FILE *file) {

    //  initial_fp_pos is the file pointer position of the root of the tree.
    // It must be checked whether it is pointing to a leaf, in which case the places where
    // its children should be are outside the bounds of the file.
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    if (initial_fp_pos 
   

    if (leaf) { // Leaf
        result
    }

    else { // Internal node
        int children_fp[M]; // Array of file pointers to children
        int rect_size = 4*sizeof(int);
        for(int k=0; k<M; k++) {
            children_fp[M] = fseek(file, child_fp);
        }
    }

    for(int i =0; i<M; i++) {
        int child_fp = children_fp[M];
        int x1_child;
        int y1_child;
        int x2_child;
        int y2_child;
        fseek(file, child_fp, 0);
        fread(&x1_child, sizeof(int), 1, file);
        fread(&y1_child, sizeof(int), 1, file);
        fread(&x2_child, sizeof(int), 1, file);
        fread(&y2_child, sizeof(int), 1, file);

        int intersection_bool = Intersection(x1, x2, y1, y2, x1_child, x2_child, y1_child, y2_child);
        if(intersection_bool == 1) {
            searchRectangleInTree(x1, y1, x2, y2, child_fp, file);
        }
    }


}
   
