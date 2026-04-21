#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0
#define CLIENT 1
#define TAG 0

/* ---------- Helper function for MPI error checking ---------- */
void check_mpi_error(int err, const char *message) {
    if (err != MPI_SUCCESS) {
        fprintf(stderr, "MPI error: %s\n", message);
        MPI_Abort(MPI_COMM_WORLD, err);
    }
}

/* ---------- Helper function for argument checking ---------- */
int get_num_pings(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: mpirun -np 2 ./pingpong <num_pings>\n");
        return -1;
    }

    int num_pings = atoi(argv[1]);

    if (num_pings <= 0) {
        fprintf(stderr, "Error: <num_pings> must be a positive integer.\n");
        return -1;
    }

    return num_pings;
}

/* ---------- Helper function for timing ---------- */
double get_time(void) {
    return MPI_Wtime();
}

int main(int argc, char *argv[]) {
    int rank, size;
    int num_pings;
    int counter = 0;
    double start_time, end_time, elapsed_time, average_time;
    MPI_Status status;
    int err;

    /* Initialize MPI */
    err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS) {
        fprintf(stderr, "Failed to initialize MPI.\n");
        MPI_Abort(MPI_COMM_WORLD, err);
    }

    err = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    check_mpi_error(err, "Could not get MPI rank.");

    err = MPI_Comm_size(MPI_COMM_WORLD, &size);
    check_mpi_error(err, "Could not get MPI size.");

    /* This program requires exactly 2 processes */
    if (size != 2) {
        if (rank == ROOT) {
            fprintf(stderr, "Error: This program must be run with exactly 2 processes.\n");
            fprintf(stderr, "Example: mpirun -np 2 ./pingpong 1000\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    num_pings = get_num_pings(argc, argv);
    if (num_pings < 0) {
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    /* Start timing before the ping-pong loop */
    start_time = get_time();

    while (counter < num_pings) {
        if (rank == ROOT) {
            /* Root sends counter to client (ping) */
            err = MPI_Send(&counter, 1, MPI_INT, CLIENT, TAG, MPI_COMM_WORLD);
            check_mpi_error(err, "Root failed to send counter to client.");

            /* Root receives incremented counter from client (pong) */
            err = MPI_Recv(&counter, 1, MPI_INT, CLIENT, TAG, MPI_COMM_WORLD, &status);
            check_mpi_error(err, "Root failed to receive counter from client.");
        }
        else if (rank == CLIENT) {
            /* Client receives counter from root */
            err = MPI_Recv(&counter, 1, MPI_INT, ROOT, TAG, MPI_COMM_WORLD, &status);
            check_mpi_error(err, "Client failed to receive counter from root.");

            /* Client increments counter by 1 */
            counter++;

            /* Client sends counter back to root */
            err = MPI_Send(&counter, 1, MPI_INT, ROOT, TAG, MPI_COMM_WORLD);
            check_mpi_error(err, "Client failed to send counter to root.");
        }
    }

    /* End timing after the loop */
    end_time = get_time();

    if (rank == ROOT) {
        elapsed_time = end_time - start_time;
        average_time = elapsed_time / num_pings;

        printf("Final counter   = %d\n", counter);
        printf("Elapsed time    = %f seconds\n", elapsed_time);
        printf("Average time    = %e seconds per ping-pong\n", average_time);
    }

    err = MPI_Finalize();
    if (err != MPI_SUCCESS) {
        fprintf(stderr, "Failed to finalize MPI.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}