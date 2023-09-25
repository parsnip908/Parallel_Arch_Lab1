#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include "gen_matrix.h"
#include "my_malloc.h"

#include <stdio.h>
#include <stdlib.h>

int block_dim;

void multiply_block(double* A, double* B, double* C, int block_x, int block_y, int matrix_dim)
{
    int i, j, k;
    for(i = block_y*block_dim; i < (block_y+1)*block_dim; i++)
    {
        for(j = block_x*block_dim; j < (block_x+1)*block_dim; j++)
        {
            double r = 0;
            for(k = 0; k < matrix_dim; k++)
                r += A[i*matrix_dim + k] * B[j*matrix_dim + k];
            C[i*matrix_dim + j] = r;
        }
    }

}

void print_matrix_row_major(double* matrix_array, int matrix_dim)
{
    int x, y;
    for (y = 0; y < matrix_dim; y++) {
        for (x = 0; x < matrix_dim; x++) {
            printf("%f ", matrix_array[y * matrix_dim + x]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_matrix_column_major(double* matrix_array, int matrix_dim)
{
    int x, y;
    for (y = 0; y < matrix_dim; y++) {
        for (x = 0; x < matrix_dim; x++) {
            printf("%f ", matrix_array[x * matrix_dim + y]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char** argv, char** envp)
{
    char* nworkers = getenv("CILK_NWORKERS");
    if(nworkers == NULL)
    {
        printf("CILK_NWORKERS not found\n");
        exit(1);
    }
    printf("CILK_NWORKERS = %s\n\n",nworkers);
    int num_processes = atoi(nworkers);

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

    //alloc space for all arg matrices
    int matrix_size = matrix_dimension_size * matrix_dimension_size;
    double *arg_matrices = (double *)my_malloc(sizeof(double*) * num_arg_matrices * matrix_size);
    double *output_matrix = (double *)my_malloc(sizeof(double) * matrix_size);
    double *row_matrix = arg_matrices;
    double *column_matrices = arg_matrices + matrix_size;
    
    //Generating arg matrices
    if (gen_sub_matrix(0, test_set, 0, &arg_matrices[0], 0, matrix_dimension_size-1, 1, 0, matrix_dimension_size - 1, 1, 1) == NULL) 
    {
        printf("inconsistency in gen_sub_matrix\n");
        exit(1);
    }
    for(i=1; i < num_arg_matrices; i++)
    {
        if (gen_sub_matrix(0, test_set, i, &arg_matrices[i*matrix_size], 0, matrix_dimension_size-1, 1, 0, matrix_dimension_size - 1, 1, 0) == NULL) 
        {
            printf("inconsistency in gen_sub_matrix\n");
            exit(1);
        }
    }

    //print everything
    if(debug_perf == 0)
    {
        printf("argument matrix 0\n");
        print_matrix_row_major(&arg_matrices[j*matrix_size], matrix_dimension_size);
        for(j=1; j < num_arg_matrices; j++)
        {
            printf("argument matrix %d \n", j);
            print_matrix_column_major(&arg_matrices[j*matrix_size], matrix_dimension_size);
        }
    }

    //multiply
    int matrix_id, block_x, block_y;
    block_dim = matrix_dimension_size/num_processes;
    double *A = row_matrix;
    double *B = column_matrices;
    double *C = output_matrix;
    for(matrix_id = 0; matrix_id < num_arg_matrices-1; matrix_id++)
    {
        cilk_scope
        {
            for(block_x = 0; block_x < num_processes; block_x++)
            {
                for(block_y = 0; block_y < num_processes; block_y++)
                {
                    cilk_spawn multiply_block(A, B, C, block_x, block_y, matrix_dimension_size);
                }
            }
        }
        //matrix mult complete
        //result matrix is now the new input matrix
        double* intermediate = A;
        A = C;
        C = intermediate;
        B += matrix_size;
    }
    double * result = A;

    //print result
    if(debug_perf == 0)
    {
        printf("result matrix %d \n", j);
        print_matrix_row_major(result, matrix_dimension_size);
    }
    else
    {
        double sum = 0;
        for (int i = 0; i < matrix_dimension_size*matrix_dimension_size; i++)
            sum += result[i];
        printf("%f\n", sum);
    }

    my_free(arg_matrices);
    my_free(output_matrix);
    return 0;
}
