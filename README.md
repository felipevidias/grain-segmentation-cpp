# Topological Grain Analysis in C++

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-CeCILL-blue.svg)](https://cecill.info/)
[![Build](https://img.shields.io/badge/Build-CMake-orange.svg)](https://cmake.org/)

## Overview

[cite_start]This repository contains a C++ implementation of algorithms for the segmentation and topological analysis of 3D images of granular materials[cite: 3]. The primary goal is to detect and quantify contacts between individual grains based on high-resolution 3D scan data.

This project is developed for study and research purposes, refactoring several Python-based image processing scripts into a modular and efficient C++ application.

## Academic Context & Acknowledgment

[cite_start]The work and algorithms implemented in this project are heavily based on the methods described in the master's thesis by Lysandre Macke[cite: 22, 26]. This project serves as a practical, high-performance C++ implementation of the concepts presented in this academic work.

**Primary Reference:**
> Macke, L. (2024). *ANALYSE TOPOLOGIQUE DE MOUVEMENTS DE GRAINS DANS UNE SÉQUENCE D'IMAGES 3D*. [cite_start]Master's Thesis, GREYC - Université de Caen Normandie & Université Gustave Eiffel. [cite: 18, 20, 21, 22, 28]

[cite_start]The original research leverages libraries such as Higra, Tifffile, and Skimage to perform segmentation using component trees[cite: 5, 373]. [cite_start]A core part of the original work also involved contributing a 3D Tree of Shapes implementation to the Higra library[cite: 399, 401, 402]. This C++ project aims to create a native toolchain for similar analyses, reducing dependencies on Python scripts for performance-critical tasks.

## Implemented Modules

This project includes C++ implementations of the following algorithms and utilities inspired by or required for the methods in the reference paper:

* **Core Image Structures:** A basic 3D image container to handle voxel data.
* **Component Tree-based Segmentation:**
    * Naive Contact Detection: Iterative erosion and contact detection on a labeled image.
    * Skeleton-based Contact Detection: Uses a pre-computed skeleton to guide contact analysis during erosion.
    * Extending Labels Contact Detection: Propagates labels from grain cores outwards to determine contact zones.
* **Utility Modules:**
    * [cite_start]`minTree`: Core extraction using min-trees to find markers within grains[cite: 200, 202].
    * [cite_start]`getCentroid`: Connected-component labeling and centroid calculation[cite: 211, 213].
    * [cite_start]`tiff_binarization`: Image thresholding[cite: 258, 264].
    * `tiff_binary_sum`: Logical OR operation on two images.
    * `tiff2raw`: File format conversion.
* **Terminal Utilities:** A helper class for displaying status and loading animations in the terminal.

## Dependencies

To build and run this project, you will need the following dependencies installed on your system (tested on Linux):

* **Build Tools:**
    * A C++17 compliant compiler (e.g., `g++`)
    * `CMake` (version 3.16 or higher)
    * `git`
* **Core Libraries:**
    * [cite_start]**Higra**: A C++ library for Hierarchical Graph Analysis[cite: 387].
    * **xtensor**: A C++ library for multi-dimensional array expressions (used by Higra).
    * [cite_start]**libtiff**: A library for reading and writing TIFF files[cite: 375, 388].

## How to Build

1.  **Clone the Repository:**
    ```bash
    git clone <your-repository-url>
    cd <your-repository-name>
    ```

2.  **Install Dependencies:** Ensure that `libtiff-dev`, `xtensor`, and `higra` are installed. Please refer to previous instructions on how to clone and install `xtensor` and `higra` (e.g., in a `~/dev` directory).

3.  **Configure with CMake:** It's recommended to build in a separate directory.
    ```bash
    mkdir build
    cd build
    ```
    Now, run CMake to configure the project. You must provide the paths to your `higra` installation and `xtensor` includes.
    ```bash
    cmake -Dhigra_DIR=~/dev/higra/build -DCMAKE_PREFIX_PATH=/usr/local ..
    ```
    * `-Dhigra_DIR=...`: Points to the build directory of your compiled `higra` library.
    * `-DCMAKE_PREFIX_PATH=...`: Tells CMake to search for other libraries (like a system-installed `higra`) in `/usr/local`.

4.  **Compile the Project:**
    Once CMake has configured successfully, simply run `make`.
    ```bash
    make -j$(nproc) # Using -j with the number of your CPU cores will speed up compilation.
    ```
    The final executable, `meu_programa`, will be located inside the `build` directory.

## How to Run

1.  Navigate to the `build` directory.
2.  Execute the program:
    ```bash
    ./meu_programa
    ```
3.  To choose which algorithm to run, edit the `main.cpp` file, comment/uncomment the desired `run_...()` function, and recompile using `make`.

---

## License
[cite_start]This project is available under the CeCILL License[cite: 417], in accordance with the license of the original Pink library and the academic context of the work.
