import numpy as np
import matplotlib.pyplot as plt

# Load data (2 columns: size (bytes), time (seconds))
data = np.loadtxt("Desktop/data/results.txt")

x = data[:, 0]   # message size (bytes)
y = data[:, 1]   # average time (seconds)

# Linear fit: y = m*x + c
m, c = np.polyfit(x, y, 1)

# Generate fitted line
x_fit = np.linspace(min(x), max(x), 100)
y_fit = m * x_fit + c

# Plot
plt.figure()
plt.scatter(x, y, label="Data")
plt.plot(x_fit, y_fit, label=f"Fit: y = {m:.2e}x + {c:.2e}")

plt.xlabel("Message Size (bytes)")
plt.ylabel("Time per Ping-Pong (s)")
plt.title("MPI Ping-Pong Latency Benchmark")
plt.legend()
plt.grid()

plt.show()

# Print results
print("Slope (m) =", m, "seconds per byte")
print("Intercept (c) =", c, "seconds")

# Bandwidth = 1 / slope
bandwidth = 1 / m
print("Bandwidth =", bandwidth, "bytes/second")

# Convert to MB/s
bandwidth_MB = bandwidth / (1024 * 1024)
print("Bandwidth =", bandwidth_MB, "MB/s")