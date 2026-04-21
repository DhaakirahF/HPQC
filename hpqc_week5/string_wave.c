#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Structure to store all command-line settings */
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
void print_vector(double vector[], int size);
int sum_vector(int vector[], int size);
void update_positions(double* positions, int points, double time);
int generate_timestamps(double* time_stamps, int time_steps, double step_size);
double driver(double time);
void print_header(FILE** p_out_file, int points);

int main(int argc, char **argv)
{
	/* Read command-line arguments */
	ProgramArgs args = check_args(argc, argv);

	int points = args.points;
	int cycles = args.cycles;
	int samples = args.samples;
	char *output_path = args.output_path;

	/* Creates variables for the vibration */
	int time_steps = cycles * samples + 1;
	double step_size = 1.0 / samples;

	/* Creates a vector for the time stamps in the data */
	double* time_stamps = (double*) malloc(time_steps * sizeof(double));
	if (time_stamps == NULL)
	{
		fprintf(stderr, "Memory allocation failed for time_stamps.\n");
		exit(-1);
	}
	initialise_vector(time_stamps, time_steps, 0.0);
	generate_timestamps(time_stamps, time_steps, step_size);

	/* Creates a vector variable for the current positions */
	double* positions = (double*) malloc(points * sizeof(double));
	if (positions == NULL)
	{
		fprintf(stderr, "Memory allocation failed for positions.\n");
		free(time_stamps);
		exit(-1);
	}
	initialise_vector(positions, points, 0.0);

	/* Opens the output file */
	FILE* out_file = fopen(output_path, "w");
	if (out_file == NULL)
	{
		fprintf(stderr, "Error: Could not open output file: %s\n", output_path);
		free(time_stamps);
		free(positions);
		exit(-1);
	}
	print_header(&out_file, points);

	/* Iterates through each time step in the collection */
	for (int i = 0; i < time_steps; i++)
	{
		update_positions(positions, points, time_stamps[i]);

		fprintf(out_file, "%d, %lf", i, time_stamps[i]);

		for (int j = 0; j < points; j++)
		{
			fprintf(out_file, ", %lf", positions[j]);
		}
		fprintf(out_file, "\n");
	}

	free(time_stamps);
	free(positions);
	fclose(out_file);

	return 0;
}

/* Prints a header to the file */
void print_header(FILE** p_out_file, int points)
{
	fprintf(*p_out_file, "#, time");
	for (int j = 0; j < points; j++)
	{
		fprintf(*p_out_file, ", y[%d]", j);
	}
	fprintf(*p_out_file, "\n");
}

/* Defines a simple harmonic oscillator as the driving force */
double driver(double time)
{
	double value = sin(time * 2.0 * M_PI);
	return value;
}

/* Updates the positions along the string */
void update_positions(double* positions, int points, double time)
{
	double* new_positions = (double*) malloc(points * sizeof(double));
	if (new_positions == NULL)
	{
		fprintf(stderr, "Memory allocation failed in update_positions.\n");
		exit(-1);
	}

	int i = 0;
	new_positions[i] = driver(time);

	for (i = 1; i < points; i++)
	{
		new_positions[i] = positions[i - 1];
	}

	for (i = 0; i < points; i++)
	{
		positions[i] = new_positions[i];
	}

	free(new_positions);
}

/* Defines a set of timestamps */
int generate_timestamps(double* timestamps, int time_steps, double step_size)
{
	for (int i = 0; i < time_steps; i++)
	{
		timestamps[i] = i * step_size;
	}
	return time_steps;
}

/* Defines a function to sum a vector of ints into another int */
int sum_vector(int vector[], int size)
{
	int sum = 0;

	for (int i = 0; i < size; i++)
	{
		sum += vector[i];
	}

	return sum;
}

/* Initialises all values in a vector to a given initial value */
void initialise_vector(double vector[], int size, double initial)
{
	for (int i = 0; i < size; i++)
	{
		vector[i] = initial;
	}
}

/* Prints a vector of doubles */
void print_vector(double vector[], int size)
{
	for (int i = 0; i < size; i++)
	{
		printf("%d, %lf\n", i, vector[i]);
	}
}

/* Checks the command-line arguments and returns them in a struct */
ProgramArgs check_args(int argc, char **argv)
{
	ProgramArgs args;

	/* Expected usage:
	   ./string_wave <points> <cycles> <samples> <output_path>
	*/
	if (argc == 5)
	{
		args.points = atoi(argv[1]);
		args.cycles = atoi(argv[2]);
		args.samples = atoi(argv[3]);
		args.output_path = argv[4];

		if (args.points <= 0 || args.cycles <= 0 || args.samples <= 0)
		{
			fprintf(stderr, "Error: points, cycles, and samples must all be positive integers.\n");
			exit(-1);
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Incorrect number of arguments.\n");
		fprintf(stderr, "Correct use: %s <points> <cycles> <samples> <output_path>\n", argv[0]);
		fprintf(stderr, "Example: %s 50 5 25 /Users/dhaakirahfasetire/Desktop/data/string_wave.csv\n", argv[0]);
		exit(-1);
	}

	return args;
}