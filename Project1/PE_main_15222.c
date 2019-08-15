#include "PE_functions_15222.h"

int main(int argc, char* argv[]){
    /*
    Program for performing the page rank algorithm on a web graph.
    Utilizes the read_graph_from_file function for organizing the graph data in a
    CRS matrix format. The PageRank_iterations function iterates the matrix-vector
    multiplication in parallel untill the score vector of the pages converge. The top_n_webpages
    function sorts the top scores and prints them to the terminal.
    As command line arguments it must be provided with the filename of the web graph,
    aswell as the desired number of score listings, damping constant of the matrix and the
    tolerance for the convergence of the score vector. Optional 5th command line argument
    is to specify number of threads to spawn in the parallel region of the PageRank_iterations
    function.
    */


    //terminate program if insuficcient command line arguments
    if(argc < 5){
        printf("Not enough command line arguments..\n");
        printf("Filename of web graph, no. of score listings, damping constant and iteration tolerance must be provided.\n");
        exit(1);
    }



    //initialize variables and array pointers used in organizing the data in a CRS matrix
    struct timespec start_read, end_read, start_mul, end_mul, start_top_n, end_top_n, start_tot, end_tot;
    clock_gettime(CLOCK_REALTIME, &start_tot);

    char* filename = argv[1];
    int nOutput = atoi(argv[2]);
    double dDamping = atof(argv[3]);
    double eps = atof(argv[4]);
    if(argc == 6) omp_set_num_threads(atoi(argv[5]));


    int nNodes = 0;
    int nEdges = 0;
    int dCount = 0;
    int *D = NULL;
    double *val = NULL;
    int *colIdx = NULL;
    int *rowPtr = NULL;


    //read page graph from file and get matrix
    clock_gettime(CLOCK_REALTIME, &start_read);
    read_graph_from_file(filename, &nNodes, &nEdges, &dCount, &D,  &val, &colIdx, &rowPtr);
    clock_gettime(CLOCK_REALTIME, &end_read);


    //allocate memory for the score vector
    double *x = malloc(nNodes*sizeof(double));

    //perform matrix multiplication
    clock_gettime(CLOCK_REALTIME, &start_mul);
    PageRank_iterations(nNodes, nEdges, dCount, dDamping, D, colIdx, rowPtr, val, x, eps);
    clock_gettime(CLOCK_REALTIME, &end_mul);


    //get top scoring web pages and print results
    clock_gettime(CLOCK_REALTIME, &start_top_n);
    top_n_webpages(nNodes, nOutput, x);
    clock_gettime(CLOCK_REALTIME, &end_top_n);

    clock_gettime(CLOCK_REALTIME, &end_tot);

    //print timings
    double time_spent_read = (end_read.tv_sec - start_read.tv_sec) + (end_read.tv_nsec - start_read.tv_nsec) / 1000000000.0;
    double time_spent_mul = (end_mul.tv_sec - start_mul.tv_sec) + (end_mul.tv_nsec - start_mul.tv_nsec) / 1000000000.0;
    double time_spent_top_n = (end_top_n.tv_sec - start_top_n.tv_sec) + (end_top_n.tv_nsec - start_top_n.tv_nsec) / 1000000000.0;
    double time_spent_tot = (end_tot.tv_sec - start_tot.tv_sec) + (end_tot.tv_nsec - start_tot.tv_nsec) / 1000000000.0;
    printf("----------------Timings [sec]--------------\n");
    printf("Reading web graph:    %5f \n", time_spent_read);
    printf("Page rank iterations: %5f \n", time_spent_mul);
    printf("Sorting top pages:    %5f \n", time_spent_top_n);
    printf("Total:                %5f \n", time_spent_tot );
    printf("-------------------------------------------\n");

    //free memory
    free(x);
    free(val);
    free(colIdx);
    free(rowPtr);
    free(D);


    return 0;
}
