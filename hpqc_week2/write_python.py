import sys
import time

def main():
    if len(sys.argv) != 2:
        raise Exception("Usage: python3 write_python.py [NUM_LINES]")

    input_lines = int(sys.argv[1])

    start_time = time.time()

    with open("data/testfile_python.txt", "w") as f:
        for i in range(input_lines):
            f.write(f"Line {i}\n")

    end_time = time.time()
    run_time = end_time - start_time

    print(f"Time taken to write file: {run_time} seconds")

if __name__ == "__main__":
    main()