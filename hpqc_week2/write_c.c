#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    int input, i;
    clock_t start_time, end_time;
    double run_time;
    FILE *fp;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./write_file [NUM_LINES]\n");
        return 1;
    }

    input = atoi(argv[1]);

    fp = fopen("data/testfile_c.txt", "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file for writing.\n");
        return 1;
    }

    start_time = clock();

    for (i = 0; i < input; i++)
    {
        fprintf(fp, "Line %d\n", i);
    }

    end_time = clock();

    fclose(fp);

    run_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken to write file: %f seconds\n", run_time);

    return 0;
}