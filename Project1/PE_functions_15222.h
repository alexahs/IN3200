#ifndef PE_FUNCTIONS_15222_H
#define PE_FUNCTIONS_15222_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>




void swap(int *a, int *b);
void swapDouble(double *a, double *b);
void sort(int *arr, int beg, int end);

void read_graph_from_file(char* filename, int* nNodes, int* nEdgesNew, int* dCount, int** D, double** val, int** colIdx, int** rowPtr);
void PageRank_iterations(int nNodes, int nEdges, int dCount, double dDamping, int *D, int *colIdx, int *rowPtr, double *val, double *x, double eps);
void top_n_webpages(int nNodes, int nOutput, double* x);

#endif
