#pragma once

#include <string>

/**
 * @brief Executes a "naive" contact detection algorithm by performing a full-image scan.
 *
 * This method iteratively checks every non-background voxel for neighbors with different labels.
 * After each full scan that finds contacts, the image is eroded, and the process is repeated.
 * The number of erosion steps required to separate two grains defines their contact strength.
 * This approach is simple but computationally intensive.
 */
void run_contact_detection_naive();