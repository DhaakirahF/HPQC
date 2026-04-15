import time

def main():
    start_time = time.time()

    with open("data/testfile_python.txt", "r") as f:
        lines = f.readlines()

    end_time = time.time()
    run_time = end_time - start_time

    print(f"Read {len(lines)} lines")
    print(f"Time taken to read file: {run_time} seconds")

if __name__ == "__main__":
    main()