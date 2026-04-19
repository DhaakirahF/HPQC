#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size, int initial);
void fill_vector(int vector[], int size);
int sum_vector(int vector[], int size);
double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv)
{
    // declare and initialise the numerical argument variable
    int num_arg = check_args(argc, argv);

    // creates a vector variable dynamically
    int *my_vector = malloc(num_arg * sizeof(int));
    if (my_vector == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(-1);
    }

    // initialises every element to zero
    initialise_vector(my_vector, num_arg, 0);

    // timing variables
    struct timespec start_time, end_time, time_diff;
    double runtime = 0.0;

    // gets the time before the core work
    timespec_get(&start_time, TIME_UTC);

    // fills the vector with meaningful values
    fill_vector(my_vector, num_arg);

    // sums the vector
    int my_sum = sum_vector(my_vector, num_arg);

    // gets the time after the core work
    timespec_get(&end_time, TIME_UTC);

    // calculates the runtime
    time_diff = calculate_runtime(start_time, end_time);
    runtime = to_second_float(time_diff);

    // prints the sum
    printf("Sum: %d\n", my_sum);

    // prints the internal runtime
    printf("Runtime for core operations: %lf seconds.\n", runtime);

    // free allocated memory
    free(my_vector);

    return 0;
}

// defines a function to sum a vector of ints into another int
int sum_vector(int vector[], int size)
{
    int sum = 0;

    for (int i = 0; i < size; i++)
    {
        sum += vector[i];
    }

    return sum;
}

// defines a function to initialise all values in a vector to a given initial value
void initialise_vector(int vector[], int size, int initial)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] = initial;
    }
}

// fills the vector with non-trivial values
void fill_vector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] = i * i + 1;
    }
}

// defines a function that checks your arguments
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
    long int seconds, nanoseconds;
    seconds = nanoseconds = 0;

    seconds = in_time.tv_sec;
    nanoseconds = in_time.tv_nsec;

    out_time = seconds + nanoseconds / 1e9;

    return out_time;
}

struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time)
{
    struct timespec time_diff;
    long int seconds, nanoseconds;
    seconds = nanoseconds = 0;

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