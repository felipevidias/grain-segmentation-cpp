# Topological Grain Analysis in C++

[![Status](https://img.shields.io/badge/Status-Work%20In%20Progress-yellow.svg)](#)
[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://isocpp.org/)
[![Build](https://img.shields.io/badge/Build-CMake-orange.svg)](https://cmake.org/)

## Overview

This repository contains a C++ implementation of algorithms for the segmentation and topological analysis of 3D images of granular materials. The primary goal is to detect and quantify contacts between individual grains based on high-resolution 3D scan data.

**Please note: This project is currently under active development.** The core algorithms have been translated from Python to C++, but critical features like file I/O for TIFF images are still using placeholder functions.

## Academic Context & Acknowledgment

The work and algorithms implemented in this project are heavily based on the methods described in the master's thesis by Lysandre Macke. This project serves as a practical, high-performance C++ implementation of the concepts presented in this academic work.

**Primary Reference:**
> Macke, L. (2024). *ANALYSE TOPOLOGIQUE DE MOUVEMENTS DE GRAINS DANS UNE SÉQUENCE D'IMAGES 3D*. Master's Thesis, GREYC - Université de Caen Normandie & Université Gustave Eiffel.

## Project Status & Roadmap

### Implemented Modules
The following Python scripts and concepts from the reference work have been successfully refactored into a modular C++ structure:
* **Core Algorithms:**
    * Naive Contact Detection
    * Skeleton-based Contact Detection
    * Extending Labels Contact Detection
* **Utility Modules:**
    * `minTree` (using the Higra C++ library)
    * `getCentroid` (native C++ implementation of connected-component labeling)
    * Image operations (`tiff_binarization`, `tiff_binary_sum`, `tiff2raw`)
    * Terminal styling and loading animations.

### Roadmap / To-Do List
The following tasks are planned to bring the project to a fully functional state:
-   [ ] **Implement TIFF File I/O:** Replace all placeholder functions (`loadTiffImage_...`, `saveTiffImage_...`) with a complete implementation using a C++ library like **libtiff**. This is the highest priority task.
-   [ ] **Implement Command-Line Argument Parsing:** Replace the hardcoded variables in the C++ modules with a proper argument parser (e.g., `cxxopts`) to allow for dynamic input from the command line.
-   [ ] **(Advanced) Native Pink Library Integration:** As a future optimization, replace the `system()` calls to the Pink command-line tools with direct C++ function calls by compiling and linking against `libpink`.

## Dependencies

To build and run this project, you will need the following dependencies installed:
* **Build Tools:** `g++` (C++17), `CMake` (3.16+), `git`.
* **Core Libraries:** **Higra**, **xtensor**, and **libtiff**.

## How to Build

1.  **Clone the Repository:**
    ```bash
    git clone <[your-repository-url](https://github.com/felipevidias/grain-segmentation-cpp.git)>
    cd <grain-segmentation-cpp>
    ```
2.  **Install Dependencies:** Ensure all required libraries are installed.
3.  **Configure with CMake:**
    ```bash
    mkdir build && cd build
    cmake -Dhigra_DIR=~/dev/higra/build ..
    ```
4.  **Compile the Project:**
    ```bash
    make -j$(nproc)
    ```

## How to Run

The program can be compiled and executed, but it currently operates with placeholder data due to the incomplete file I/O.

1.  Navigate to the `build` directory.
2.  Execute the program:
    ```bash
    ./meu_programa
    ```
3.  To choose which algorithm to run, edit the `main.cpp` file, comment/uncomment the desired `run_...()` function, and recompile using `make`.

---

## License
This project is available under the CeCILL License.
