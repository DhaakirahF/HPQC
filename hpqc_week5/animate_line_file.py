import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import pandas as pd
from pathlib import Path
import sys


def generate_path(home_folder=str(Path.home()), subfolder='/Desktop/data/', basename='output', extension='txt'):
    """Create a default output path outside the working directory."""
    output_folder = home_folder + subfolder
    filename = basename + '.' + extension
    output_path = output_folder + filename
    return output_path


def execute_time_step(data, rope, i):
    """Update the rope for a single time step."""
    x_positions, y_positions = extract_position(data, i=i)
    rope.set_data(x_positions, y_positions)
    return i


def reset_plots(rope, y_positions):
    """Reset the rope to its initial state."""
    x_positions, y_positions = configure_rope()
    rope.set_data(x_positions, y_positions)


def animate(time, data, rope):
    """Carry out one animation update."""
    x_positions, y_positions = extract_position(data, i=time)
    rope.set_data(x_positions, y_positions)
    return rope,


def configure_animation(frame_count=125, fps=25):
    """Configure frame timing for the animation."""
    run_time = frame_count / fps
    interval = max([int(1000 / fps), 1])
    times = np.linspace(0., run_time, int(frame_count))
    return times, interval, fps


def configure_rope(length=50, end_point=True, step_size=1, start=0):
    """Configure the initial rope positions."""
    points = int((length / step_size) + end_point)
    stop = start + length
    x_positions = np.linspace(start=start, stop=stop, num=points, endpoint=end_point)
    y_positions = np.zeros(points)
    return x_positions, y_positions


def configure_plot(x_positions, y_positions):
    """Set up the figure and line plot for the animation."""
    fig, ax1 = plt.subplots(1, 1)
    fig.suptitle('Simulation of Vibrations in a String')
    ax1.set_ylim(-1.1, 1.1)

    rope, = ax1.plot(
        x_positions,
        y_positions,
        "o",
        markersize=5,
        color="green",
        label="points on string"
    )

    ax1.legend(handles=[rope], bbox_to_anchor=(0, 0), loc='upper left')
    return fig, rope


def get_data(filename, other=2):
    """Read the CSV data file and return the data plus metadata."""
    try:
        data = pd.read_csv(filename, skipinitialspace=True)
    except FileNotFoundError:
        print(f"The file you specified, {filename}, does not exist.")
        exit(-1)

    num_positions = len(data.columns) - other
    num_times = len(data)
    return data, num_positions, num_times


def extract_position(data, i=0, other=2):
    """Extract x and y positions for one row of the dataset."""
    row = data.iloc[i]
    y_positions = np.array(row[other:]).flatten()
    x_positions = np.arange(len(row) - other)
    return x_positions, y_positions


def check_args():
    """Read and validate command-line arguments.

    Expected usage:
        python animate_line_file.py <input_csv> <output_gif>

    Returns:
        input_file (str): path to the input CSV file
        output_file (str): path to the output GIF file
    """
    if len(sys.argv) != 3:
        print("Incorrect number of arguments.")
        print(f"Correct usage:\n\tpython {sys.argv[0]} <input_csv> <output_gif>")
        print("Example:")
        print(f"\tpython {sys.argv[0]} /Users/dhaakirahfasetire/Desktop/data/string_wave.csv /Users/dhaakirahfasetire/Desktop/data/string_wave.gif")
        exit(-1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    if input_file.split('.')[-1] != "csv":
        print(f"The input file {input_file} does not appear to be a csv file.")
        exit(-1)

    if output_file.split('.')[-1] != "gif":
        print(f"The output file {output_file} does not appear to be a gif file.")
        exit(-1)

    return input_file, output_file


def main():
    """Main execution function."""
    input_file, output_file = check_args()

    data, num_positions, num_times = get_data(input_file)
    times, interval, fps = configure_animation(frame_count=num_times)

    x_positions, y_positions = extract_position(data)
    fig, rope = configure_plot(x_positions, y_positions)

    ani = animation.FuncAnimation(
        fig,
        animate,
        num_times,
        interval=interval,
        blit=True,
        fargs=(data, rope)
    )

    ani.save(filename=output_file, writer="pillow", fps=fps)


if __name__ == "__main__":
    main()