#pragma once

#include <string>

/**
 * @brief Executes the contact detection pipeline using a pre-labeled image and a skeleton.
 *
 * This method orchestrates external tools to generate a skeleton from the input images.
 * It then iteratively checks for contacts only on the skeleton voxels, which significantly
 * reduces the search space compared to a naive full-image scan. At each iteration,
 * the labeled image is eroded to measure the strength of the detected contacts.
 */
void run_contact_detection_from_label_and_skeleton();