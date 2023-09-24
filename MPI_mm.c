#include <mpi.h>
#include <stdio.h>
#include "gen_matrix.c"
#include "my_malloc.c"

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
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //alloc space
    //gen matricies
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

    printf("world rank/size: %d/%d \t M rank/size: %d/%d \n", rank, num_processes, M_rank, M_size);

    MPI_Finalize();
    return 0;
}
