#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0
#define CLIENT 1
#define TAG 0

void check_mpi_error(int err, const char *message);
int get_num_pings(int argc, char *argv[]);
int get_message_size_bytes(int argc, char *argv[]);
double get_time(void);
void initialise_array(int *array, int num_elements, int initial_value);

int main(int argc, char *argv[])
{
    int rank, size;
    int err;
    int num_pings;
    int message_size_bytes;
    int num_elements;
    int *buffer = NULL;
    MPI_Status status;

    double start_time, end_time, elapsed_time, average_time;

    err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS) {
        fprintf(stderr, "Failed to initialize MPI.\n");
        MPI_Abort(MPI_COMM_WORLD, err);
    }

    err = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    check_mpi_error(err, "Could not get MPI rank.");

    err = MPI_Comm_size(MPI_COMM_WORLD, &size);
    check_mpi_error(err, "Could not get MPI size.");

    if (size != 2) {
        if (rank == ROOT) {
            fprintf(stderr, "Error: This program must be run with exactly 2 processes.\n");
            fprintf(stderr, "Usage: mpirun -np 2 ./pingpong_array <num_pings> <message_size_bytes>\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    num_pings = get_num_pings(argc, argv);
    message_size_bytes = get_message_size_bytes(argc, argv);

    if (num_pings <= 0 || message_size_bytes <= 0) {
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    /* Work out how many int elements are needed */
    num_elements = message_size_bytes / (int)sizeof(int);

    /* If message_size_bytes is not exactly divisible by sizeof(int),
       add one extra element so the buffer is large enough */
    if (message_size_bytes % (int)sizeof(int) != 0) {
        num_elements++;
    }

    buffer = (int *)malloc(num_elements * sizeof(int));
    if (buffer == NULL) {
        fprintf(stderr, "Error: malloc failed.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    initialise_array(buffer, num_elements, 0);

    /* Use element 0 as the ping-pong counter */
    buffer[0] = 0;

    start_time = get_time();

    while (buffer[0] < num_pings) {
        if (rank == ROOT) {
            /* Root sends whole array to client */
            err = MPI_Send(buffer, num_elements, MPI_INT, CLIENT, TAG, MPI_COMM_WORLD);
            check_mpi_error(err, "Root failed to send array to client.");

            /* Root receives whole array back from client */
            err = MPI_Recv(buffer, num_elements, MPI_INT, CLIENT, TAG, MPI_COMM_WORLD, &status);
            check_mpi_error(err, "Root failed to receive array from client.");
        }
        else if (rank == CLIENT) {
            /* Client receives whole array from root */
            err = MPI_Recv(buffer, num_elements, MPI_INT, ROOT, TAG, MPI_COMM_WORLD, &status);
            check_mpi_error(err, "Client failed to receive array from root.");

            /* Increment counter stored in element 0 */
            buffer[0]++;

            /* Client sends whole array back to root */
            err = MPI_Send(buffer, num_elements, MPI_INT, ROOT, TAG, MPI_COMM_WORLD);
            check_mpi_error(err, "Client failed to send array to root.");
        }
    }

    end_time = get_time();

    if (rank == ROOT) {
        elapsed_time = end_time - start_time;
        average_time = elapsed_time / num_pings;

         printf("%d %e\n", message_size_bytes, average_time);

        //printf("Final counter        = %d\n", buffer[0]);
        //printf("Message size         = %d bytes\n", message_size_bytes);
        //printf("Elements transmitted = %d ints\n", num_elements);
        //printf("Elapsed time         = %f seconds\n", elapsed_time);
        //printf("Average time         = %e seconds per ping-pong\n", average_time);
    }

    free(buffer);

    err = MPI_Finalize();
    if (err != MPI_SUCCESS) {
        fprintf(stderr, "Failed to finalize MPI.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void check_mpi_error(int err, const char *message)
{
    if (err != MPI_SUCCESS) {
        fprintf(stderr, "MPI error: %s\n", message);
        MPI_Abort(MPI_COMM_WORLD, err);
    }
}

int get_num_pings(int argc, char *argv[])
{
    int num_pings;

    if (argc != 3) {
        fprintf(stderr, "Usage: mpirun -np 2 ./pingpong_array <num_pings> <message_size_bytes>\n");
        return -1;
    }

    num_pings = atoi(argv[1]);

    if (num_pings <= 0) {
        fprintf(stderr, "Error: <num_pings> must be a positive integer.\n");
        return -1;
    }

    return num_pings;
}

int get_message_size_bytes(int argc, char *argv[])
{
    int message_size_bytes;

    if (argc != 3) {
        fprintf(stderr, "Usage: mpirun -np 2 ./pingpong_array <num_pings> <message_size_bytes>\n");
        return -1;
    }

    message_size_bytes = atoi(argv[2]);

    if (message_size_bytes <= 0) {
        fprintf(stderr, "Error: <message_size_bytes> must be a positive integer.\n");
        return -1;
    }

    return message_size_bytes;
}

double get_time(void)
{
    return MPI_Wtime();
}

void initialise_array(int *array, int num_elements, int initial_value)
{
    for (int i = 0; i < num_elements; i++) {
        array[i] = initial_value;
    }
}