# WEEK 2 SUMMARY OF TASK 

task1.pdf - includes results for week 2 task 


## Programs
The following programs were used:

- `repeat_adder.c / .py`  
  Multiplies two numbers using repeated addition to demonstrate how runtime scales with loop size.

- `time_print.c / .py`  
  Prints values to the screen and measures how long the printing operation takes.

- `write_c.c / write_python.py`  
  Writes a specified number of lines to a file and measures the time taken.

- `read_c.c / read_python.py`  
  Reads the contents of a file into memory and measures the time taken.


## Results Summary

### File Writing

| Input | C (s) | Python (s) |
|------|------|-----------|
| 10   | 0.000030 | 0.003099 |
| 100  | 0.000046 | 0.003746 |
| 1000 | 0.000211 | 0.004186 |
| 5000 | 0.000943 | 0.005330 |

### File Reading

| Input | C (s) | Python (s) |
|------|------|-----------|
| 5000 lines | 0.000469 | 0.003897 |

---

## Evaluation

The results show that runtime varies depending on the type of operation being performed. For simple computations such as repeated addition, runtime increases as the number of loop iterations increases, meaning the first input has the greatest impact on performance.

For printing operations, runtime increases significantly with input size. This demonstrates that printing to the screen is relatively slow compared to computation, as each output operation adds overhead.

File input/output operations showed a clear increase in runtime as file size increased. Writing and reading larger files required more time, as these operations depend not only on the program but also on the operating system and storage system.

In all experiments, the C implementations were consistently faster than the Python implementations. This is because C is a compiled language and runs directly as machine code, while Python is interpreted and introduces additional overhead.

Python performed similarly to C for very small inputs, but as input size increased, the difference in performance became more noticeable. This shows that Python is suitable for smaller tasks but less efficient for performance-critical operations.

---

## Conclusion

Overall, the experiments demonstrate that:

- Runtime increases with input size for all operations  
- The number of loop iterations has a major impact on execution time  
- Printing and file I/O operations are significantly slower than simple computations  
- C provides better performance than Python, especially for larger inputs  
- Python is easier to use but introduces more overhead  

These results highlight the importance of choosing the appropriate programming language and minimising unnecessary input/output operations when optimising performance.
