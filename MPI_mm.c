#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "gen_matrix.h"
#include "my_malloc.h"

void print_chunk(double* chunk, int matrix_dim, int num_rows)
{
    int x, y;
    for(x = 0; x < num_rows; x++)
    {
        for(y = 0; y < matrix_dim; y++)
            printf("%f ", chunk[x*matrix_dim+y]);
        printf("\n");
    }
}

int main(int argc, char** argv)
{
    if (argc != 4) 
    {
        printf("usage: debug_perf test_set matrix_dimension_size\n");
        exit(1);
    }

    int debug_perf = atoi(argv[1]);
    int test_set = atoi(argv[2]);
    matrix_dimension_size = atoi(argv[3]);
    int num_arg_matrices = init_gen_sub_matrix(test_set);

    MPI_Init(&argc, &argv);
    int i, j, k;
    int x, y;
    int rank, num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
    //Allocate all necessary space
    int chunk_size = (matrix_dimension_size/num_processes)*matrix_dimension_size;
    double **column_matrices = (double **)my_malloc(sizeof(double *) * (num_arg_matrices-1));
    double *row_matrix    = (double *)my_malloc(sizeof(double) * chunk_size);
    double *output_matrix = (double *)my_malloc(sizeof(double) * chunk_size);
    double *recieve_chunk = (double *)my_malloc(sizeof(double) * chunk_size);
    
    //Generating matrix for rows owned by this rank
    int num_rows = matrix_dimension_size/num_processes;
    int y_start = rank * num_rows;
    int y_end = (y_start + num_rows) - 1;
    if(gen_sub_matrix(rank, test_set, 0, row_matrix, 0, matrix_dimension_size - 1, 1, y_start, y_end, 1, 1) == NULL)
    {
        printf("inconsistency in gen_sub_matrix\n");
        exit(1);
    }

    //Generating matrices for columns owned by this rank
    int num_columns = num_rows;
    int x_start = rank * num_columns;
    int x_end = (x_start + num_columns) - 1;
    for(i=0; i < num_arg_matrices-1; i++)
    {
        column_matrices[i] = (double *)my_malloc(sizeof(double) * chunk_size);
        if (gen_sub_matrix(rank, test_set, i+1, column_matrices[i], x_start, x_end, 1, 0, matrix_dimension_size - 1, 1, 0) == NULL)
        {
                printf("inconsistency in gen_sub_matrix\n");
                exit(1);
        }
    }

    //if debug, print all elements owned (must do arguments first)
    if(debug_perf == 0)
    {
        if(rank==0)
        {
            //Print first arg matrix (row major)
            printf("argument matrix 0\n");
            print_chunk(row_matrix, matrix_dimension_size, num_rows);
            for(i=1; i < num_processes; i++)
            {
                MPI_Recv(recieve_chunk, chunk_size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                print_chunk(recieve_chunk, matrix_dimension_size, num_rows);
            }
            printf("\n");

            //Print all other arg matricies. 
            //The rows are spread out over all processors so MPI_Gather is used to collect the values of each row

            for(i=1; i < num_arg_matrices; i++)
            {
                printf("argument matrix %d\n", i);
                for(j = 0; j < matrix_dimension_size; j++)
                {
                    MPI_Gather(column_matrices[0], num_columns, MPI_DOUBLE, recieve_chunk, num_columns, MPI_DOUBLE, 0, MPI_COMM_WORLD);
                    for(k = 0; k < num_columns; k++)
                        recieve_chunk[k] = column_matrices[i-1][k*matrix_dimension_size+j];
                    for(k = 0; k < matrix_dimension_size; k++)
                        printf("%f ", recieve_chunk[k]);
                    printf("\n");
                }
                printf("\n");
            }
        }
        else
        {
            //Send the owned data of first arg matrix (row major)
            MPI_Send(row_matrix, chunk_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

            //Send data for other arg matricies line by line using MPI_gather
            for(i=1; i < num_arg_matrices; i++)
            {
                for(j = 0; j < matrix_dimension_size; j++)
                {
                    //arrange data for one line
                    for(k = 0; k < num_columns; k++)
                        recieve_chunk[k] = column_matrices[i-1][k*matrix_dimension_size+j];
                    //send it
                    MPI_Gather(recieve_chunk, num_columns, MPI_DOUBLE, NULL, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    //Matrix multiply
    int matrix_id, block_id;
    int block_dim = num_rows;
    double *A = row_matrix;
    double *B = recieve_chunk;
    double *C = output_matrix;
    for(matrix_id = 0; matrix_id < num_arg_matrices-1; matrix_id++)
    {
        for(block_id = 0; block_id < num_processes; block_id++)
        {
            //if rank matches, broadcast owned column
            //else recieve column B
            if(rank == block_id) B = column_matrices[matrix_id];
            else B = recieve_chunk;
            MPI_Bcast(B, chunk_size, MPI_DOUBLE, block_id, MPI_COMM_WORLD);
            
            //Compute current block
            int i, j, k;
            for(i = 0; i < block_dim; i++)
            {
                for(j = 0; j < block_dim; j++)
                {
                    double r = 0;
                    for(k = 0; k < matrix_dimension_size; k++)
                        r += A[i*matrix_dimension_size + k] * B[j*matrix_dimension_size + k];
                    C[i*matrix_dimension_size + block_id*block_dim + j] = r;
                }
            }
        }
        //matrix mult complete
        //result matrix is now the new input matrix
        double* intermediate = A;
        A = C;
        C = intermediate;
    }
    double * result = A;
    
    //if debug, print result
    if(debug_perf == 0)
    {
        if(rank==0)
        {
            printf("result matrix \n");
            print_chunk(result, matrix_dimension_size, num_rows);

            for(i=1; i < num_processes; i++)
            {
                MPI_Recv(recieve_chunk, chunk_size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                print_chunk(recieve_chunk, matrix_dimension_size, num_rows);
            }
        }
        else
        {
            MPI_Send(result, chunk_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }
    //else sum all
    else
    {
        double local_sum = 0;
        double global_sum = 0;
        for(i = 0; i < chunk_size; i++)
            local_sum += result[i];
        
        MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);    
        if(rank == 0)
            printf("%f\n", global_sum);
    }

    //free memory
    my_free(row_matrix);
    my_free(output_matrix);
    my_free(recieve_chunk);
    for(i=0; i < num_arg_matrices-1; i++)
        my_free(column_matrices[i]);
    my_free(column_matrices);

    MPI_Finalize();
    return 0;
}
