#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "gen_matrix.h"
#include "my_malloc.h"

int main(int argc, char** argv)
{
    double **r;
    double **result;
    int i;
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

    MPI_Init(&argc, &argv);
    int rank, num_processes;
    double **column_matrices;
    double *row_matrix;
    double *output_matrix;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //alloc space
    int chunk_size = (matrix_dimension_size/num_processes)*matrix_dimension_size;
    row_matrix = (double *)my_malloc(sizeof(double) * chunk_size);
    column_matrices = (double **)my_malloc(sizeof(double *) * (num_arg_matrices-1));
    output_matrix = (double *)my_malloc(sizeof(double) * chunk_size);
    //gen matricies
    
    //Generating matrix for rows owned by this rank
    int num_rows = matrix_dimension_size/num_processes;
    int y_start = rank * num_rows;
    int y_end = (y_start + num_rows) - 1;
    if(gen_sub_matrix(rank, test_set, 0, row_matrix, 0, matrix_dimension_size - 1, 1, y_start, y_end, 1, 1) == NULL){
        printf("inconsistency in gen_sub_matrix\n");
        exit(1);
    }

    //Generating matrices for columns owned by this rank
    int num_columns = num_rows;
    int x_start = rank * num_columns;
    int x_end = (x_start + num_columns) - 1;
    for(i=0; i < num_arg_matrices-1; i++){
        column_matrices[i] = (double *)my_malloc(sizeof(double) * chunk_size);
        if (gen_sub_matrix(rank, test_set, i+1, column_matrices[i], x_start, x_end, 1, 0, matrix_dimension_size - 1, 1, 1) == NULL) {
                printf("inconsistency in gen_sub_matrix\n");
                exit(1);
        }
    }


    //if debug, print all elements owned
    //multiply
    int block_dim = matrix_dimension_size / num_processes;
    int matrix_id, block_id;
    double *recieve_chunk = (double *)my_malloc(sizeof(double) * chunk_size);
    double *A = row_matrix;
    double *B = recieve_chunk;
    double *C = output_matrix;
    for(matrix_id = 0; matrix_id < num_arg_matrices-1; matrix_id++)
    {
        for(block_id = 0; block_id < num_processes; block_id++)
        {
            //if rank matches, send owned column
            //else recieve column B
            if(rank == block_id) B = column_matrices[matrix_id];
            else B = recieve_chunk;
            MPI_Bcast(B, chunk_size, MPI_DOUBLE, block_id, MPI_COMM_WORLD);
            //compute
            int i, j, k;
            for(i = 0; i < block_dim; i++)
            {
                for(j = 0; j < block_dim; j++)
                {
                    double * r = &C[i*matrix_dimension_size + block_id*block_dim + j];
                    *r = 0;
                    for(k = 0; k < matrix_dimension_size; k++)
                    {
                        *r += A[i*matrix_dimension_size + k] * B[j*matrix_dimension_size + k];
                    }
                }
            }
        }
        //if matrix complete, increment to next matrix
        double* intermediate = A;
        A = C;
        C = intermediate;
        //loop
    }
    //if debug, print all elements owned
    //sum all
    //collect sums and print?
    //free
    //done

    printf("world rank/size: %d/%d \n", rank,num_processes);

    MPI_Finalize();
    return 0;
}
