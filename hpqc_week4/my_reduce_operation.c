#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size, int initial);
void fill_vector(int vector[], int size);
long long partial_sum_vector(int vector[], int start, int end);

/* Custom reduction function */
void my_sum_function(void *invec, void *inoutvec, int *len, MPI_Datatype *datatype)
{
    long long *in = (long long *)invec;
    long long *inout = (long long *)inoutvec;

    for (int i = 0; i < *len; i++)
    {
        inout[i] += in[i];
    }
}

int main(int argc, char **argv)
{
    int my_rank, uni_size, num_arg;
    int *my_vector = NULL;
    int start, end;
    long long local_sum = 0, global_sum = 0;
    double t1, t2;
    MPI_Op my_sum_op;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    num_arg = check_args(argc, argv);

    my_vector = malloc(num_arg * sizeof(int));
    if (my_vector == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    initialise_vector(my_vector, num_arg, 0);
    fill_vector(my_vector, num_arg);

    start = my_rank * num_arg / uni_size;
    end   = (my_rank + 1) * num_arg / uni_size;

    local_sum = partial_sum_vector(my_vector, start, end);

    /* Create custom reduction operator */
    MPI_Op_create(my_sum_function, 1, &my_sum_op);

    MPI_Barrier(MPI_COMM_WORLD);
    t1 = MPI_Wtime();

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, my_sum_op, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();

    if (my_rank == 0)
    {
        printf("Custom reduce version\n");
        printf("Array size = %d\n", num_arg);
        printf("Processes  = %d\n", uni_size);
        printf("Sum        = %lld\n", global_sum);
        printf("Runtime    = %lf seconds\n", t2 - t1);
    }

    MPI_Op_free(&my_sum_op);
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