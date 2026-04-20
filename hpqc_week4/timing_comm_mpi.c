#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

// function declarations
void root_task(int my_rank, int uni_size);
void client_task(int my_rank, int uni_size);
void check_task(int my_rank, int uni_size);
void check_uni_size(int uni_size);
double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv)
{
	int ierror = 0;

	int my_rank, uni_size;
	my_rank = uni_size = 0;

	ierror = MPI_Init(&argc, &argv);

	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

	check_uni_size(uni_size);
	check_task(my_rank, uni_size);

	ierror = MPI_Finalize();
	return 0;
}

void root_task(int my_rank, int uni_size)
{
	int recv_message, count, source, tag;
	recv_message = source = tag = 0;
	count = 1;
	MPI_Status status;

	struct timespec start_time, end_time, time_diff;
	double runtime = 0.0;

	for (int their_rank = 1; their_rank < uni_size; their_rank++)
	{
		source = their_rank;

		timespec_get(&start_time, TIME_UTC);

		MPI_Recv(&recv_message, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

		timespec_get(&end_time, TIME_UTC);

		time_diff = calculate_runtime(start_time, end_time);
		runtime = to_second_float(time_diff);

		printf("Hello, I am %d of %d. Received %d from Rank %d\n",
		       my_rank, uni_size, recv_message, source);
		printf("Rank %d took %lf seconds to receive from Rank %d\n",
		       my_rank, runtime, source);
	}
}

void client_task(int my_rank, int uni_size)
{
	int send_message, count, dest, tag;
	send_message = dest = tag = 0;
	count = 1;

	struct timespec start_time, end_time, time_diff;
	double runtime = 0.0;

	dest = 0;
	send_message = my_rank * 10;

	timespec_get(&start_time, TIME_UTC);

	MPI_Send(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD);

	timespec_get(&end_time, TIME_UTC);

	time_diff = calculate_runtime(start_time, end_time);
	runtime = to_second_float(time_diff);

	printf("Hello, I am %d of %d. Sent %d to Rank %d\n",
	       my_rank, uni_size, send_message, dest);
	printf("Rank %d took %lf seconds to send to Rank %d\n",
	       my_rank, runtime, dest);
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