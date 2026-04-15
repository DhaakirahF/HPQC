#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    FILE *fp;
    char buffer[256];
    clock_t start_time, end_time;
    double run_time;
    int line_count = 0;

    fp = fopen("data/testfile_c.txt", "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file for reading.\n");
        return 1;
    }

    start_time = clock();

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        line_count++;
    }

    end_time = clock();

    fclose(fp);

    run_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Read %d lines\n", line_count);
    printf("Time taken to read file: %f seconds\n", run_time);

    return 0;
}