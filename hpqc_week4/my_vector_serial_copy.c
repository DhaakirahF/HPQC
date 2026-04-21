#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size, int initial);
void fill_vector(int vector[], int size);
long long partial_sum_vector(int vector[], int start, int end);

int main(int argc, char **argv)
{
    int my_rank, uni_size;
    int num_arg;
    int *my_vector = NULL;
    int local_start, local_end, local_n;
    long long local_sum = 0, global_sum = 0;
    double start_time, end_time, runtime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    num_arg = check_args(argc, argv);

    my_vector = malloc(num_arg * sizeof(int));
    if (my_vector == NULL)
    {
        fprintf(stderr, "Rank %d: Memory allocation failed.\n", my_rank);
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    initialise_vector(my_vector, num_arg, 0);
    fill_vector(my_vector, num_arg);

    local_start = my_rank * num_arg / uni_size;
    local_end = (my_rank + 1) * num_arg / uni_size;
    local_n = local_end - local_start;

    local_sum = partial_sum_vector(my_vector, local_start, local_end);

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    runtime = end_time - start_time;

    if (my_rank == 0)
    {
        printf("Full-copy version\n");
        printf("Array size = %d\n", num_arg);
        printf("Processes  = %d\n", uni_size);
        printf("Sum        = %lld\n", global_sum);
        printf("Runtime    = %lf seconds\n", runtime);
    }

    free(my_vector);
    MPI_Finalize();
    return 0;
}

long long partial_sum_vector(int vector[], int start, int end)
{
    long long sum = 0;
    for (int i = start; i < end; i++)
    {
        sum += vector[i];
    }
    return sum;
}

void initialise_vector(int vector[], int size, int initial)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] = initial;
    }
}

void fill_vector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] = i * i + 1;
    }
}

int check_args(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [ARRAY_SIZE]\n", argv[0]);
        exit(-1);
    }
    return atoi(argv[1]);
}