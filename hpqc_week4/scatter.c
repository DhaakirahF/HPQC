#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size, int initial);
void fill_vector(int vector[], int size);
long long sum_vector(int vector[], int size);

int main(int argc, char **argv)
{
    int my_rank, uni_size;
    int num_arg;
    int *full_vector = NULL;
    int *local_vector = NULL;
    int local_n;
    long long local_sum = 0, global_sum = 0;
    double start_time, end_time, runtime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    num_arg = check_args(argc, argv);

    if (num_arg % uni_size != 0)
    {
        if (my_rank == 0)
        {
            fprintf(stderr, "Array size must be divisible by number of processes for this version.\n");
        }
        MPI_Finalize();
        return -1;
    }

    local_n = num_arg / uni_size;
    local_vector = malloc(local_n * sizeof(int));
    if (local_vector == NULL)
    {
        fprintf(stderr, "Rank %d: Local memory allocation failed.\n", my_rank);
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    if (my_rank == 0)
    {
        full_vector = malloc(num_arg * sizeof(int));
        if (full_vector == NULL)
        {
            fprintf(stderr, "Root: Full memory allocation failed.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    if (my_rank == 0)
    {
        initialise_vector(full_vector, num_arg, 0);
        fill_vector(full_vector, num_arg);
    }

    MPI_Scatter(full_vector, local_n, MPI_INT,
                local_vector, local_n, MPI_INT,
                0, MPI_COMM_WORLD);

    local_sum = sum_vector(local_vector, local_n);

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    runtime = end_time - start_time;

    if (my_rank == 0)
    {
        printf("Scatter version\n");
        printf("Array size = %d\n", num_arg);
        printf("Processes  = %d\n", uni_size);
        printf("Sum        = %lld\n", global_sum);
        printf("Runtime    = %lf seconds\n", runtime);
    }

    free(local_vector);
    if (my_rank == 0)
    {
        free(full_vector);
    }

    MPI_Finalize();
    return 0;
}

long long sum_vector(int vector[], int size)
{
    long long sum = 0;
    for (int i = 0; i < size; i++)
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