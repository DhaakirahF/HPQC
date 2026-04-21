#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Function declarations */
int check_args(int argc, char **argv);
void initialise_vector(double vector[], int size, double initial);
void print_vector(double vector[], int size);
void update_positions(double* positions, double* velocities, int points, double time, double dt, double k, double mass);
int generate_timestamps(double* time_stamps, int time_steps, double step_size);
double driver(double time);
void print_header(FILE** p_out_file, int points);

int main(int argc, char **argv)
{
	/* Declare and initialise the numerical argument variable */
	int points = check_args(argc, argv);

	/* Creates variables for the vibration */
	int cycles = 40;              /* number of cycles to show */
	int samples = 25;            /* sampling rate in samples per cycle */
	int time_steps = cycles * samples + 1;
	double step_size = 1.0 / samples;

	/* Spring model parameters */
	double dt = step_size;
	double k = 5.0;
	double mass = 1.0;

	/* Creates a vector for the time stamps in the data */
	double* time_stamps = (double*) malloc(time_steps * sizeof(double));
	initialise_vector(time_stamps, time_steps, 0.0);
	generate_timestamps(time_stamps, time_steps, step_size);

	/* Creates a vector for the current positions */
	double* positions = (double*) malloc(points * sizeof(double));
	initialise_vector(positions, points, 0.0);

	/* Creates a vector for the velocities */
	double* velocities = (double*) malloc(points * sizeof(double));
	initialise_vector(velocities, points, 0.0);

	/* Creates a file */
	FILE* out_file;
	out_file = fopen("/Users/dhaakirahfasetire/Desktop/data/string_wave_spring.csv", "w");
	print_header(&out_file, points);

	/* Iterates through each time step in the collection */
	for (int i = 0; i < time_steps; i++)
	{
		/* Update the positions using the spring model */
		update_positions(positions, velocities, points, time_stamps[i], dt, k, mass);

		/* Prints an index and time stamp */
		fprintf(out_file, "%d, %lf", i, time_stamps[i]);

		/* Iterates over all of the points on the line */
		for (int j = 0; j < points; j++)
		{
			fprintf(out_file, ", %lf", positions[j]);
		}
		fprintf(out_file, "\n");
	}

	/* Free dynamically allocated memory */
	free(time_stamps);
	free(positions);
	free(velocities);

	/* Close the file */
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

/* Updates the positions using a spring-force model */
void update_positions(double* positions, double* velocities, int points, double time, double dt, double k, double mass)
{
	double* new_positions = (double*) malloc(points * sizeof(double));
	double* new_velocities = (double*) malloc(points * sizeof(double));

	if (new_positions == NULL || new_velocities == NULL)
	{
		fprintf(stderr, "Memory allocation failed in update_positions.\n");
		exit(-1);
	}

	/* Drive the first point */
	new_positions[0] = driver(time);
	new_velocities[0] = 0.0;

	/* Keep the last point fixed */
	new_positions[points - 1] = 0.0;
	new_velocities[points - 1] = 0.0;

	/* Update the interior points */
	for (int i = 1; i < points - 1; i++)
	{
		double force = k * (positions[i + 1] - 2.0 * positions[i] + positions[i - 1]);
		double acceleration = force / mass;

		new_velocities[i] = velocities[i] + acceleration * dt;
		new_positions[i] = positions[i] + new_velocities[i] * dt;
	}

	/* Copy the new values back */
	for (int i = 0; i < points; i++)
	{
		positions[i] = new_positions[i];
		velocities[i] = new_velocities[i];
	}

	free(new_positions);
	free(new_velocities);
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

/* Defines a function to initialise all values in a vector */
void initialise_vector(double vector[], int size, double initial)
{
	for (int i = 0; i < size; i++)
	{
		vector[i] = initial;
	}
}

/* Defines a function to print a vector of doubles */
void print_vector(double vector[], int size)
{
	for (int i = 0; i < size; i++)
	{
		printf("%d, %lf\n", i, vector[i]);
	}
}

/* Checks the command-line arguments */
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