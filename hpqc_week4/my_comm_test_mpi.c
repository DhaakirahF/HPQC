#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// function declarations
void root_task(int my_rank, int uni_size);
void client_task(int my_rank, int uni_size);
void check_task(int my_rank, int uni_size);
void check_uni_size(int uni_size);

int main(int argc, char **argv)
{
	// declare and initialise error handling variable
	int ierror = 0;

	// declare and initialise rank and size variables
	int my_rank, uni_size;
	my_rank = uni_size = 0;

    // buffer variables for MPI_Bsend
	int buffer_size;
	void *buffer;

	// initialise MPI
	ierror = MPI_Init(&argc, &argv);

    // set up the buffer for buffered send
	buffer_size = MPI_BSEND_OVERHEAD + sizeof(int);
	buffer = malloc(buffer_size);
	if (buffer == NULL)
	{
		fprintf(stderr, "Buffer allocation failed.\n");
		MPI_Finalize();
		exit(-1);
	}
	MPI_Buffer_attach(buffer, buffer_size);

	// gets the rank and world size
	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

	// check communicator size
	check_uni_size(uni_size);

	// decide which task to perform
	check_task(my_rank, uni_size);

    	// detach and free buffer
	MPI_Buffer_detach(&buffer, &buffer_size);
	free(buffer);

	// finalise MPI
	ierror = MPI_Finalize();
	return 0;
}

void root_task(int my_rank, int uni_size)
{
	int recv_message, count, source, tag;
	recv_message = source = tag = 0;
	count = 1;
	MPI_Status status;

	for (int their_rank = 1; their_rank < uni_size; their_rank++)
	{
		source = their_rank;

		MPI_Recv(&recv_message, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

		printf("Hello, I am %d of %d. Received %d from Rank %d\n",
		       my_rank, uni_size, recv_message, source);
	}
}

void client_task(int my_rank, int uni_size)
{
	int send_message, count, dest, tag;
	send_message = dest = tag = 0;
	count = 1;

	dest = 0;
	send_message = my_rank * 10;

	MPI_Bsend(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD);

	printf("Hello, I am %d of %d. Sent %d to Rank %d\n",
	       my_rank, uni_size, send_message, dest);
}

void check_task(int my_rank, int uni_size)
{
	if (0 == my_rank)
	{
		root_task(my_rank, uni_size);
	}
	else
	{
		client_task(my_rank, uni_size);
	}
}

void check_uni_size(int uni_size)
{
	if (uni_size > 1)
	{
		return;
	}
	else
	{
		printf("Unable to communicate with less than 2 processes. MPI communicator size = %d\n", uni_size);
		MPI_Finalize();
		exit(-1);
	}
}