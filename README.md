# Sensor Data Manager

A thread-safe C++ module that simulates real-time acquisition and processing of sensor data from a longitudinal wood board scanning system. This system buffers timestamped position and density readings and performs statistical calculations (mean, min, median) on filtered data using linear interpolation.

---

## Features

- Thread-safe collection of asynchronous sensor streams (position & density)
- Linear interpolation of non-aligned timestamps
- Sliding window filtering of stale data (default set to 5 seconds)
- Efficient statistical summary (mean, min, and median)
- Customizable median computation algorithms (`NthElement`, `FullSort`, `HeapMedian`)
- Simple concurrent tests with simulated sensor input

---

## Project Structure

- **[Dockerfile](./Dockerfile)**
- **[build.sh](./build.sh)**
    - Compiles project using g++
- **[valgrind.sh](./valgrind.sh)**
    - Runs Valgrind for memory checks
- **[SensorDataManager.h](./SensorDataManager.h)**
    - Header for the SensorDataManager class
- **[SensorDataManager.cpp](./SensorDataManager.cpp)** 
    - Implementation of the three required thread-safe functions
- **[MedianStrategy.h](./MedianStrategy.h)**
    - Additional median computation strategies
- **[unit_tests_microtec.cpp](./unit_tests_microtec.cpp)**
    - Simulates data input and query threads

## Build and Run

### Local (without Docker)

#### Requirements
- `g++`
- `make` (optional)
- `valgrind` (optional, for memory checking)

#### Compile and Run
bash build.sh
./microtec_test

#### Check for memory leaks
bash valgrind.sh

### Run Using Docker

1. Build the Container Image
    - `docker build -t microtec-dev .`

2. Run the Container and Mount Your Code
    - `docker run -it --rm -v "${PWD}:/app" microtec-dev`

3. Inside the Container
    - `bash build.sh`
    - `./microtec_test`
    - `bash valgrind.sh`
        - for memory leak checking

### Testing and Thread Safety
This module is designed for use in multi-threaded environments. It uses std::mutex to guard access to shared buffers, ensuring thread-safe operation.

The **[unit_tests_microtec.cpp](./unit_tests_microtec.cpp)** file launches three concurrent threads to simulate a realistic asynchronous environment:

- `simulate_density_input()` — high-frequency (~1800 Hz) density samples

- `simulate_position_input()` — lower-frequency (~500 Hz) board positions

- `simulate_query()` — periodically queries the statistical summary over a position interval

These simulate how real hardware might drive the SensorDataManager and confirm thread-safety and correctness.



### Notes
The build process uses `-lpthread` for POSIX thread support.

Dockerfile includes common C++ development tools and Valgrind.

The `-g` flag enables debug symbols for better memory diagnostics.