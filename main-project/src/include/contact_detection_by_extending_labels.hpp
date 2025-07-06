#pragma once

#include <string>

/**
 * @brief Executes the full pipeline for contact detection using the "extending labels" method.
 *
 * This function orchestrates a multi-step process that includes:
 * 1. Calling external scripts and tools (Python, Pink) to pre-process image data.
 * 2. Loading the resulting images (grains, skeleton, minTree) and centroid data.
 * 3. Running an iterative algorithm that propagates labels outwards from grain cores.
 * 4. Detecting contacts where propagation fronts meet and eroding the image to measure contact strength.
 * 5. Saving the final contact strength map to a CSV file.
 */
void run_contact_detection_by_extending_labels();