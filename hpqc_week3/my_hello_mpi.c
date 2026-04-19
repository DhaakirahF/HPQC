#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int my_rank, uni_size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    printf("Hello, from process %d out of %d processes\n", my_rank, uni_size);

    MPI_Finalize();
    return 0;
}