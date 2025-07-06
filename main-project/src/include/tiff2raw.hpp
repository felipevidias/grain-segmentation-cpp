#pragma once

#include <string>

/**
 * @brief Converts a TIFF image file to a raw binary file.
 * Replaces the tiff2raw.py script.
 * @param inputFile The path to the TIFF file to convert.
 */
void run_tiff_to_raw(const std::string& inputFile);