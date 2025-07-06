#pragma once

#include <string>

/**
 * @brief Binarizes a 3D TIFF image based on a specified threshold.
 *
 * This function reads a TIFF image, applies a threshold to each pixel, and saves
 * the result as a new binary TIFF image. Pixels with values greater than or equal
 * to the threshold are set to 255; otherwise, they are set to 0. This module is a
 * C++ replacement for the `tiff_binarization.py` script.
 *
 * @param inputFile The path to the input 3D TIFF file.
 * @param threshold The integer threshold value to apply.
 * @param outputFile The path to save the resulting binary TIFF file.
 */
void run_tiff_binarization(const std::string& inputFile, int threshold, const std::string& outputFile);