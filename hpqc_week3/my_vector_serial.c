#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size, int initial);
void fill_vector(int vector[], int size);
void print_vector(int vector[], int size);
int sum_vector(int vector[], int size);

int main(int argc, char **argv)
{
    int num_arg = check_args(argc, argv);

    int *my_vector = malloc(num_arg * sizeof(int));
    if (my_vector == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(-1);
    }

    initialise_vector(my_vector, num_arg, 0);

    clock_t start_time, end_time;
    double run_time;

    start_time = clock();

    fill_vector(my_vector, num_arg);
    int my_sum = sum_vector(my_vector, num_arg);

    end_time = clock();

    run_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Sum: %d\n", my_sum);
    printf("Internal runtime: %f seconds\n", run_time);

    free(my_vector);
    return 0;
}

int sum_vector(int vector[], int size)
{
    int sum = 0;

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

void print_vector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d\n", vector[i]);
    }
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