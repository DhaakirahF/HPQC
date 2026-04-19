#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int check_args(int argc, char **argv);
int chunk_sum(int start, int end);
double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv)
{
    int num_arg = 0;
    int my_rank = 0, uni_size = 0;
    int ierror = 0;

    struct timespec start_time, end_time, time_diff;
    double runtime = 0.0;

    int local_sum = 0;
    int global_sum = 0;

    int chunk_size = 0;
    int start = 0, end = 0;

    ierror = MPI_Init(&argc, &argv);
    ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    num_arg = check_args(argc, argv);

    if (num_arg % uni_size != 0)
    {
        if (my_rank == 0)
        {
            fprintf(stderr, "ERROR: Vector size must be divisible by number of processes.\n");
        }
        MPI_Finalize();
        exit(-1);
    }

    chunk_size = num_arg / uni_size;
    start = my_rank * chunk_size;
    end = start + chunk_size;

    timespec_get(&start_time, TIME_UTC);

    local_sum = chunk_sum(start, end);

    if (my_rank == 0)
    {
        global_sum = local_sum;

        int recv_sum = 0;
        MPI_Status status;

        for (int source = 1; source < uni_size; source++)
        {
            MPI_Recv(&recv_sum, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
            global_sum += recv_sum;
        }

        timespec_get(&end_time, TIME_UTC);
        time_diff = calculate_runtime(start_time, end_time);
        runtime = to_second_float(time_diff);

        printf("Global sum: %d\n", global_sum);
        printf("Runtime for core operations: %lf seconds\n", runtime);
    }
    else
    {
        MPI_Send(&local_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    ierror = MPI_Finalize();
    return 0;
}

int chunk_sum(int start, int end)
{
    int sum = 0;

    for (int i = start; i < end; i++)
    {
        int value = i * i + 1;
        sum += value;
    }

    return sum;
}

int check_args(int argc, char **argv)
{
    int num_arg = 0;

    if (argc == 2)
    {
        num_arg = atoi(argv[1]);
    }
    else
    {
        fprintf(stderr, "ERROR: You did not provide a numerical argument!\n");
        fprintf(stderr, "Correct use: %s [NUMBER]\n", argv[0]);
        exit(-1);
    }

    return num_arg;
}

double to_second_float(struct timespec in_time)
{
    double out_time = 0.0;
    long int seconds = 0, nanoseconds = 0;

    seconds = in_time.tv_sec;
    nanoseconds = in_time.tv_nsec;

    out_time = seconds + nanoseconds / 1e9;

    return out_time;
}

struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time)
{
    struct timespec time_diff;
    long int seconds = 0, nanoseconds = 0;

    seconds = end_time.tv_sec - start_time.tv_sec;
    nanoseconds = end_time.tv_nsec - start_time.tv_nsec;

    if (nanoseconds < 0)
    {
        seconds = seconds - 1;
        nanoseconds = ((long int)1e9) + nanoseconds;
    }

    time_diff.tv_sec = seconds;
    time_diff.tv_nsec = nanoseconds;

    return time_diff;
}