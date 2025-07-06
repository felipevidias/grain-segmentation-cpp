#pragma once

#include <string>

/**
 * @brief Creates and filters a component min-tree to segment image cores.
 *
 * This function implements the core extraction method described in the reference
 * report. It uses the Higra C++ library to build a min-tree from a 3D image,
 * filters the tree based on node attributes like area and height, and then
 * reconstructs a new image from the simplified tree. This module is a native C++
 * replacement for the `minTree.py` script.
 *
 * @param inputFile The path to the input 3D TIFF image.
 * @param adjacency The graph connectivity to use (e.g., 6 or 26).
 * @param outputFile The path for the output TIFF file.
 */
void run_minTree(const std::string& inputFile, int adjacency, const std::string& outputFile);