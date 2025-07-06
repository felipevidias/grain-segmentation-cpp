#pragma once

#include <string>

/**
 * @brief Calculates the centroids of connected components in a 3D image and saves them to a CSV file.
 *
 * This function serves as a C++ replacement for the getCentroid.py script, which originally
 * relied on `skimage.measure.label` and `skimage.measure.regionprops`. It performs a
 * connected-component labeling pass on the input image to identify distinct regions,
 * calculates the geometric center (centroid) of each region, and writes the results.
 *
 * @param grainsPath The path to the grains image file (maintained for argument consistency but not used in this function).
 * @param minTreePath The path to the input 3D TIFF image (e.g., a min-tree image) to be processed.
 * @param outputPath The path for the output CSV file where the centroid data will be saved.
 */
void run_get_centroids(const std::string& grainsPath, const std::string& minTreePath, const std::string& outputPath);