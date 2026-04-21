#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ROOT 0

typedef struct
{
	int points;
	int cycles;
	int samples;
	char *output_path;
} ProgramArgs;

/* Function declarations */
ProgramArgs check_args(int argc, char **argv);
void initialise_vector(double vector[], int size, double initial);
int generate_timestamps(double* time_stamps, int time_steps, double step_size);
double driver(double time);
void print_header(FILE** p_out_file, int points);

int main(int argc, char **argv)
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	ProgramArgs args = check_args(argc, argv);

	int points = args.points;
	int cycles = args.cycles;
	int samples = args.samples;
	char *output_path = args.output_path;

	/* For this simpler version, points must divide equally */
	if (points % size != 0)
	{
		if (rank == ROOT)
		{
			fprintf(stderr, "Error: points must be divisible by number of processes.\n");
		}
		MPI_Finalize();
		return -1;
	}

	int local_points = points / size;
	int time_steps = cycles * samples + 1;
	double step_size = 1.0 / samples;

	/* Root creates timestamps */
	double *time_stamps = NULL;
	if (rank == ROOT)
	{
		time_stamps = (double*) malloc(time_steps * sizeof(double));
		initialise_vector(time_stamps, time_steps, 0.0);
		generate_timestamps(time_stamps, time_steps, step_size);
	}

	/* Local chunk of the string */
	double *positions = (double*) malloc(local_points * sizeof(double));
	double *new_positions = (double*) malloc(local_points * sizeof(double));

	if (positions == NULL || new_positions == NULL)
	{
		fprintf(stderr, "Rank %d: memory allocation failed.\n", rank);
		MPI_Abort(MPI_COMM_WORLD, -1);
	}

	initialise_vector(positions, local_points, 0.0);
	initialise_vector(new_positions, local_points, 0.0);

	/* Root file output */
	FILE *out_file = NULL;
	if (rank == ROOT)
	{
		out_file = fopen(output_path, "w");
		if (out_file == NULL)
		{
			fprintf(stderr, "Could not open output file.\n");
			MPI_Abort(MPI_COMM_WORLD, -1);
		}
		print_header(&out_file, points);
	}

	/* Root gathers full string here */
	double *full_positions = NULL;
	if (rank == ROOT)
	{
		full_positions = (double*) malloc(points * sizeof(double));
		if (full_positions == NULL)
		{
			fprintf(stderr, "Root: memory allocation failed.\n");
			MPI_Abort(MPI_COMM_WORLD, -1);
		}
	}

	/* Time loop: sequential */
	for (int t = 0; t < time_steps; t++)
	{
		double current_time = 0.0;
		if (rank == ROOT)
		{
			current_time = time_stamps[t];
		}

		/* Send current time to everyone */
		MPI_Bcast(&current_time, 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

		/* Exchange boundary value with left neighbour */
		double left_value = 0.0;
		MPI_Status status;

		if (rank > 0)
		{
			MPI_Sendrecv(&positions[0], 1, MPI_DOUBLE, rank - 1, 0,
			             &left_value, 1, MPI_DOUBLE, rank - 1, 1,
			             MPI_COMM_WORLD, &status);
		}

		if (rank < size - 1)
		{
			double dummy;
			MPI_Sendrecv(&positions[local_points - 1], 1, MPI_DOUBLE, rank + 1, 1,
			             &dummy, 1, MPI_DOUBLE, rank + 1, 0,
			             MPI_COMM_WORLD, &status);
		}

		/* Update local positions */
		for (int i = 0; i < local_points; i++)
		{
			int global_index = rank * local_points + i;

			if (global_index == 0)
			{
				new_positions[i] = driver(current_time);
			}
			else if (i == 0)
			{
				new_positions[i] = left_value;
			}
			else
			{
				new_positions[i] = positions[i - 1];
			}
		}

		/* Copy new values back */
		for (int i = 0; i < local_points; i++)
		{
			positions[i] = new_positions[i];
		}

		/* Gather full string to root */
		MPI_Gather(positions, local_points, MPI_DOUBLE,
		           full_positions, local_points, MPI_DOUBLE,
		           ROOT, MPI_COMM_WORLD);

		/* Root writes one row */
		if (rank == ROOT)
		{
			fprintf(out_file, "%d, %lf", t, current_time);
			for (int j = 0; j < points; j++)
			{
				fprintf(out_file, ", %lf", full_positions[j]);
			}
			fprintf(out_file, "\n");
		}
	}

	free(positions);
	free(new_positions);

	if (rank == ROOT)
	{
		free(time_stamps);
		free(full_positions);
		fclose(out_file);
	}

	MPI_Finalize();
	return 0;
}

/* Print file header */
void print_header(FILE** p_out_file, int points)
{
	fprintf(*p_out_file, "#, time");
	for (int j = 0; j < points; j++)
	{
		fprintf(*p_out_file, ", y[%d]", j);
	}
	fprintf(*p_out_file, "\n");
}

/* Driving oscillator */
double driver(double time)
{
	return sin(time * 2.0 * M_PI);
}

/* Generate timestamps */
int generate_timestamps(double* timestamps, int time_steps, double step_size)
{
	for (int i = 0; i < time_steps; i++)
	{
		timestamps[i] = i * step_size;
	}
	return time_steps;
}

/* Initialise vector */
void initialise_vector(double vector[], int size, double initial)
{
	for (int i = 0; i < size; i++)
	{
		vector[i] = initial;
	}
}

/* Check command-line arguments */
ProgramArgs check_args(int argc, char **argv)
{
	ProgramArgs args;

	if (argc == 5)
	{
		args.points = atoi(argv[1]);
		args.cycles = atoi(argv[2]);
		args.samples = atoi(argv[3]);
		args.output_path = argv[4];

		if (args.points <= 0 || args.cycles <= 0 || args.samples <= 0)
		{
			fprintf(stderr, "Error: points, cycles and samples must be positive.\n");
			MPI_Abort(MPI_COMM_WORLD, -1);
		}
	}
	else
	{
		fprintf(stderr, "Usage: %s <points> <cycles> <samples> <output_path>\n", argv[0]);
		fprintf(stderr, "Example: mpirun -np 5 %s 50 5 25 /Users/dhaakirahfasetire/Desktop/data/string_wave_mpi.csv\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, -1);
	}

	return args;
}