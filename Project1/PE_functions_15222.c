#include "PE_functions_15222.h"



void swap(int *a, int *b) {
    int t=*a; *a=*b; *b=t;
}

void swapDouble(double *a, double *b){
    double t=*a; *a=*b; *b=t;
}

//quickSort
void sort(int *arr, int beg, int end){
    if (end > beg + 1){
        int piv = arr[beg], l = beg + 1, r = end;
        while (l < r){
            if (arr[l] <= piv)
                l++;
        else
            swap(&arr[l], &arr[--r]);
        }
    swap(&arr[--l], &arr[beg]);
    sort(arr, beg, l);
    sort(arr, r, end);
    }
}




void read_graph_from_file(char* filename, int* nNodes, int* nValidEdges, int* dCount, int** D, double** val, int** colIdx, int** rowPtr){
    /*
    This function is used for reading the web graph and organizing its data in a
    sparse matrix in CRS format. The function must be provided with the filename of the web graph.
    The other arguments, which include the number of nodes, valid edges and danglig nodes
    are initialized in the main function, passed to the function by reference and
    updated according to the data in the file.
    The arrays containing the danling node indices, matrix values, column indices and the row pointers
    are passed in as nullpointers and initialized by the function.
    */


    //initialize file buffers and variables
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    FILE* filepointer;
    char* line_buffer = NULL;
    size_t line_buff_size = 0;
    ssize_t line_size;
    filepointer = fopen(filename, "r");
    int nEdges = 0;

    if(filepointer == NULL){
        printf("Unable to load graph. Exiting..\n");
        exit(1);
    }
    printf("Loading web graph %s... \n", filename );

    //read number of nodes and edges from file
    line_size = getline(&line_buffer, &line_buff_size, filepointer);
    line_size = getline(&line_buffer, &line_buff_size, filepointer);
    fscanf(filepointer, " %*s %*s %d %*s %d", nNodes, &nEdges);
    line_size = getline(&line_buffer, &line_buff_size, filepointer);
    line_size = getline(&line_buffer, &line_buff_size, filepointer);


    //initialize variables and allocate memory for organizing
    //the web graph in a CRS matrix format
    int* fromNodes = malloc(nEdges*sizeof(int));
    int* toNodes = malloc(nEdges*sizeof(int));
    int fromNodeTemp = 0;
    int toNodeTemp = 0;
    int nSelfLinks = 0;
    int rowPtrLen = *nNodes + 1;
    int* outboundCount = malloc(*nNodes*sizeof(int));
    int* inboundCount = calloc(*nNodes,sizeof(int));


    //scan web graph and count number of incomming and outgoing
    //links for each node
    int edgeCount = 0;
    for(int i = 0; i < nEdges; i++){
        fscanf(filepointer, "%d %d", &fromNodeTemp, &toNodeTemp);
        if(fromNodeTemp != toNodeTemp){
            fromNodes[edgeCount] = fromNodeTemp;
            toNodes[edgeCount] = toNodeTemp;
            (*nValidEdges)++;
            inboundCount[toNodeTemp]++;
            outboundCount[fromNodeTemp]++;
            edgeCount++;
        }
        else{
            nSelfLinks++;
        }

    }


    //find number of dangling nodes
    for(int node = 0; node < (*nNodes); node++){
        if(outboundCount[node] == 0){
            (*dCount)++;
        }
    }


    //find indices of dangling nodes
    (*D) = malloc((*dCount)*sizeof(int));
    int danglingIdx = 0;
    for(int node = 0; node < (*nNodes); node++){
        if(outboundCount[node] == 0){
            (*D)[danglingIdx] = node;
            danglingIdx++;
        }
    }


    //find the row pointer vector of the CRS matrix
    (*rowPtr) = malloc(rowPtrLen*sizeof(int));
    (*rowPtr)[0] = 0;
    int prevRowIdx = 0;
    for(int node = 0; node < rowPtrLen; node++){
        (*rowPtr)[node+1] = inboundCount[node] + (*rowPtr)[node];
    }



    //find the column index vector of the matrix.
    int* elmInRowCount = calloc((*nNodes),sizeof(int));
    (*colIdx) = malloc((*nValidEdges)*sizeof(int));
    int col = 0;
    int row = 0;
    int idx = 0;
    for(int edge = 0; edge < (*nValidEdges); edge++){
        col = fromNodes[edge];
        row = toNodes[edge];
        idx = (*rowPtr)[row] + elmInRowCount[row] ;
        (*colIdx)[idx] = col;
        elmInRowCount[row]++;
    }


    //sort the column index vector
    for(int i = 0; i < (*nNodes); i++){
        int startIdx = (*rowPtr)[i];
        int endIdx = (*rowPtr)[i+1];
        sort((*colIdx), startIdx, endIdx);
    }




    //find the value array of the matrix
    (*val) = malloc((*nValidEdges)*sizeof(double));
    for(int edge = 0; edge < (*nValidEdges); edge++){
        (*val)[edge] = 1.0/((double) outboundCount[(*colIdx)[edge]]);
    }


    //print graph info
    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("-----------------Graph info----------------\n");
    printf("Number of nodes:                %d\n", *nNodes);
    printf("Number of outbound links:       %d\n", *nValidEdges);
    printf("Number of self links:           %d\n", nSelfLinks);
    printf("Number of dangling pages:       %d\n", *dCount);
    printf("-------------------------------------------\n");


    //free allocated memory
    fclose(filepointer);
    free(fromNodes);
    free(toNodes);
    free(filepointer);
    free(line_buffer);
    free(inboundCount);
    free(outboundCount);
    free(elmInRowCount);
}






void PageRank_iterations(int nNodes, int nEdges, int dCount, double dDamping, int *D, int *colIdx, int *rowPtr, double *val, double *x, double eps){
    /*
    This function performs the matrix-vector multiplication according to the page rank algorithm, and updates
    the score vector x, initialized in the main function.
    It must be provided with the graph info variables aswell as the matrix in SRC format, as organized
    by the read_graph_from_file function. The convergence tolerance eps, is also specified in main, and
    provided to the function.
    */




    printf("Iterating page ranks with tolerance=%g and damping=%.2f \n",eps, dDamping );

    //initialize variables used for calculating the matrix-vector product
    double *xTemp = malloc(nNodes*sizeof(double));
    double xInit = 1.0/ ((double) nNodes);
    double xFactor1 = (1.0-dDamping)/((double) nNodes);
    double xFactor2;
    double xFactors;
    double W;
    double maxDiff;
    double diff;
    int nIterations = 0;
    double iterDiff = 10;

    //set up initial state of the score vector and multiply the value array with the damping constant
    for(int i = 0; i < nNodes; i++) x[i] = xInit;
    for(int i = 0; i < nEdges; i++) val[i] *= dDamping;



    //Iterate the matrix-vector multiplication untill convergence is reached
    while(iterDiff > eps){

        //reset iteration condition variables and dangling page sum
        maxDiff = 0;
        diff = 0;
        W = 0;

        //calculate page rank scores of dangling pages from previous iteration
        for(int i = 0; i < dCount; i++){
            W += x[D[i]];
        }

        //calculate constant variables
        xFactor2 = (dDamping*W)/((double) nNodes);
        xFactors = xFactor1 + xFactor2;

        //parallel region. Perform matrix multiplication
        #pragma omp parallel for schedule(dynamic, 1000)
            for(int i = 0; i < nNodes; i++){
                xTemp[i] = xFactors;
                for(int j = rowPtr[i]; j < rowPtr[i+1]; j++){
                    xTemp[i] += val[j] * x[colIdx[j]];
                }
            }
        //end parallel region


        //find max difference in page scores since previous iteration and update score vector
        for(int i = 0; i < nNodes; i++){
            diff = fabs(xTemp[i] - x[i]);
            if(diff > maxDiff) maxDiff = diff;
            x[i] = xTemp[i];
        }
        iterDiff = maxDiff;
        nIterations++;

    }//end while loop


    //print number of iterations untill convergence was reached
    printf("Iterations: %d\n", nIterations);

    //free memory
    free(xTemp);

}




void top_n_webpages(int nNodes, int nOutput, double* x){
    /*
    This function is responsible for finding the top n page scores and printing
    the results to the terminal. The number of nodes and desired number of result
    listings are passed to the function, along with the pointer to the unsorted
    score vector.
    */

    //initialize variables and vector used for sorting results
    int *perm = malloc(nNodes*sizeof(int));
    for(int i = 0; i < nNodes; i++) perm[i] = i;
    int maxIdx;
    double maxVal;


    //identify and sort top n scoring pages and their corresponding values
    for(int i = 0; i < nOutput; i++){
        maxIdx = i;
        maxVal = x[i];
        for(int j = i+1; j < nNodes; j++){
            if(x[j] > maxVal){
                maxIdx = j;
                maxVal = x[j];
                swapDouble(&x[i], &x[maxIdx]);
                swap(&perm[i], &perm[maxIdx]);
            }
        }
    }

    //print results
    printf("-------------------Results-----------------\n");
    printf("  Rank:   | Node Number: |   Score:\n" );
    for(int i = 0; i < nOutput; i++){
        printf("%5d     |   %7d    |  %g\n", i+1, perm[i], x[i]);
    }

    //free memory
    free(perm);

}
