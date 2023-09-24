#include <mpi.h>
#include <stdio.h>
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
    int chunk = (matrix_dimension_size/num_processes)*matrix_dimension_size;
    row_matrix = (double *)my_malloc(sizeof(double) * chunk);
    column_matrices = (double **)my_malloc(sizeof(double *) * (num_arg_matrices-1));
    output_matrix = (double *)my_malloc(sizeof(double) * chunk);
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
        column_matrices[i] = (double *)my_malloc(sizeof(double) * chunk);
        if (gen_sub_matrix(rank, test_set, i+1, column_matrices[i], x_start, x_end, 1, 0, matrix_dimension_size - 1, 1, 1) == NULL) {
                printf("inconsistency in gen_sub_matrix\n");
                exit(1);
        }
    }


    //if debug, print all elements owned
    //multiply
        //if rank matches, send owned column
        //recieve column B
        //compute
        //if matrix complete, increment to next matrix
        //loop
    //if debug, print all elements owned
    //sum all
    //collect sums and print?
    //free
    //done

    printf("world rank/size: %d/%d \n", rank);

    MPI_Finalize();
    return 0;
}
