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


    double **arg_matrices;
    double *result;
    double *output_matrix;

    //alloc space for all arg matrices
    arg_matrices = (double **)my_malloc(sizeof(double*) * num_arg_matrices *);
    output_matrix = (double *)my_malloc(sizeof(double) * matrix_dimension_size * matrix_dimension_size);
    //gen matricies
    
    //Generating arg matrices
    for(i=0; i < num_arg_matrices-1; i++){
          arg_matrices[i] = (double *)my_malloc(sizeof(double) * matrix_dimension_size * matrix_dimension_size);
          if (gen_sub_matrix(0, test_set, i, arg_matrices[i], 0, matrix_dimension_size-1, 1, 0, matrix_dimension_size - 1, 1, 1) == NULL) {
                  printf("inconsistency in gen_sub_matrix\n");
                  exit(1);
          }
      }

    return 0;
}
