# DaTask
## 25051204056 - Nikolas Tiar Banjarnahor

DaTask is a C++ terminal application for task managament, with **FTXUI**.

## Features

- Terminal-based UI powered by FTXUI
- Uses CMake for build configuration
- Copies an example database file into the executable output directory after build

## Requirements

- CMake 3.14 or newer
- A C++17-compatible compiler
- Git (for fetching FTXUI via `FetchContent`)

## Build Instructions

From the repository root:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

On Windows with the default CMake generator, this will produce the `DaTask` executable in the build output directory.

## Run

After building, run the executable from the build directory:

```bash
./DaTask
```

The build process also copies `database_ex.csv` into the same output folder as the executable as `database.csv`.

## Project Structure

- `CMakeLists.txt` - CMake configuration for the project
- `src/main.cpp` - application entry point
- `database_ex.csv` - example database file copied at build time

