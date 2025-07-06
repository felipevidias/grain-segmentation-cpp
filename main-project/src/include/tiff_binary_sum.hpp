#pragma once

#include <string>

/**
 * @brief Performs a binary sum (logical OR operation) on two 3D TIFF images.
 *
 * This function reads two TIFF images, verifies they have identical dimensions,
 * and produces an output image where a pixel is set to 255 if the corresponding
 * pixel in either of the input images has a value of 255 (or greater). Otherwise,
 * the output pixel is 0.
 *
 * @param inputFile1 The path to the first input TIFF image.
 * @param inputFile2 The path to the second input TIFF image.
 * @param outputFile The path for the output TIFF file containing the result.
 */
void run_tiff_binary_sum(const std::string& inputFile1, const std::string& inputFile2, const std::string& outputFile);