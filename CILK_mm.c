#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include "gen_matrix.h"
#include "my_malloc.h"

int main(int argc, char** argv)
{
    int i, j, k;
    int num_arg_matrices;

    if (argc != 4) 
    {
        printf("usage: debug_perf test_set matrix_dimension_size\n");
        exit(1);
    }
    int debug_perf = atoi(argv[1]);
    int test_set = atoi(argv[2]);
    matrix_dimension_size = atoi(argv[3]);
    num_arg_matrices = init_gen_sub_matrix(test_set);

    double *result;

    //alloc space for all arg matrices
    int matrix_size = matrix_dimension_size * matrix_dimension_size;
    double *arg_matrices = (double *)my_malloc(sizeof(double*) * num_arg_matrices * matrix_size);
    double *output_matrix = (double *)my_malloc(sizeof(double) * matrix_size);
    double *row_matrix = arg_matrices;
    double *column_matrices = arg_matrices + matrix_size;
    
    //Generating arg matrices
    if (gen_sub_matrix(0, test_set, i, &arg_matrices[0], 0, matrix_dimension_size-1, 1, 0, matrix_dimension_size - 1, 1, 1) == NULL) 
    {
        printf("inconsistency in gen_sub_matrix\n");
        exit(1);
    }
    for(i=1; i < num_arg_matrices; i++)
    {
        // arg_matrices[i] = (double *)my_malloc(sizeof(double) * matrix_dimension_size * matrix_dimension_size);
        if (gen_sub_matrix(0, test_set, i, &arg_matrices[i*matrix_size], 0, matrix_dimension_size-1, 1, 0, matrix_dimension_size - 1, 1, 0) == NULL) 
        {
            printf("inconsistency in gen_sub_matrix\n");
            exit(1);
        }
    }

    //print everything

    //multiply

    //print result

    return 0;
}
