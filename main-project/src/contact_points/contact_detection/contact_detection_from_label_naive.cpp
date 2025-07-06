#include "include/contact_detection_from_label_naive.hpp"
#include "include/common.hpp" // For Image3D, erosion(), and save_results()

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm> // For std::find
#include <tuple>

// --- I/O Placeholders & Helper Functions ---

// IMPORTANT: This is a placeholder. A real implementation requires a TIFF library.
Image3D loadTiffImage(const std::string& path) {
    std::cout << "WARNING: Function 'loadTiffImage' is a placeholder. Returning empty image." << std::endl;
    // A real implementation would use a library like libtiff here.
    return {};
}

/**
 * @brief Gets the 6 direct neighbors of a voxel.
 * @param i The x-coordinate.
 * @param j The y-coordinate.
 * @param k The z-coordinate.
 * @return A vector of 3D-coordinate tuples for the neighbors.
 */
std::vector<std::tuple<int, int, int>> get_neighbors(int i, int j, int k) {
    return {
        {i - 1, j, k}, {i + 1, j, k},
        {i, j - 1, k}, {i, j + 1, k},
        {i, j, k - 1}, {i, j, k + 1}
    };
}

/**
 * @brief Checks the 6 neighbors of a single voxel for contacts with different grains.
 * @param i The x-coordinate of the voxel to check.
 * @param j The y-coordinate of the voxel to check.
 * @param k The z-coordinate of the voxel to check.
 * @param input A const reference to the original, uneroded labeled image.
 * @param contactDict A reference to the map where contacts for the current iteration are stored.
 */
void detect_contact_on_pixel_naive(int i, int j, int k,
                                   const Image3D& input,
                                   std::map<int, std::vector<int>>& contactDict)
{
    int current_pixel_label = input.at(i, j, k);
    auto neighbors = get_neighbors(i, j, k);

    for (const auto& neighbor_coords : neighbors) {
        int ni = std::get<0>(neighbor_coords);
        int nj = std::get<1>(neighbor_coords);
        int nk = std::get<2>(neighbor_coords);

        // Boundary check.
        if (ni < 0 || ni >= input.x_dim || nj < 0 || nj >= input.y_dim || nk < 0 || nk >= input.z_dim) {
            continue;
        }

        int neighbor_pixel_label = input.at(ni, nj, nk);

        // Skip if the neighbor is background or part of the same grain.
        if (neighbor_pixel_label == 0 || current_pixel_label == neighbor_pixel_label) {
            continue;
        }

        // To avoid duplicates (e.g., 1-2 and 2-1), only store the contact for the smaller label.
        if (current_pixel_label < neighbor_pixel_label) {
            auto& vec = contactDict[current_pixel_label];
            // Check if this contact has already been found to avoid duplicates in the list.
            if (std::find(vec.begin(), vec.end(), neighbor_pixel_label) == vec.end()) {
                vec.push_back(neighbor_pixel_label);
            }
        }
    }
}


// --- Main Module Logic ---

void run_contact_detection_naive() {
    // --- 1. Argument Parsing (Hardcoded Placeholders) ---
    std::string filepath = "../data/label.tif";
    std::string outputPath = "../results/contacts_naive.csv";

    std::cout << "--- Module: Naive Contact Detection ---" << std::endl;

    // --- 2. Data Loading ---
    Image3D input_image = loadTiffImage(filepath);
    if (input_image.data.empty()) {
        std::cerr << "Error: Failed to load image. Aborting." << std::endl;
        return;
    }

    // --- 3. Contact Detection: Main Iterative Loop ---
    std::map<std::pair<int, int>, int> contactsStrength;
    Image3D current_labels = input_image;
    bool havingContacts = true;
    int contactStrength = 0;

    while (havingContacts) {
        contactStrength++;
        std::cout << "Erosion level (Contact Strength): " << contactStrength << std::endl;

        std::map<int, std::vector<int>> contacts_this_iteration;

        // Naive approach: iterate through every voxel of the image.
        for (int i = 0; i < current_labels.x_dim; ++i) {
            for (int j = 0; j < current_labels.y_dim; ++j) {
                for (int k = 0; k < current_labels.z_dim; ++k) {
                    if (current_labels.at(i, j, k) != 0) {
                        // Pass the original, uneroded image to check neighbor relationships.
                        // This prevents issues where a neighbor might have been eroded in the same pass.
                        detect_contact_on_pixel_naive(i, j, k, input_image, contacts_this_iteration);
                    }
                }
            }
        }

        havingContacts = !contacts_this_iteration.empty();
        if (havingContacts) {
            // Store the strength for newly found contacts.
            for (const auto& pair : contacts_this_iteration) {
                int grain1 = pair.first;
                for (int grain2 : pair.second) {
                    contactsStrength[{grain1, grain2}] = contactStrength;
                }
            }
            // Erode the image for the next iteration.
            current_labels = erosion(current_labels);
        }
    }

    // --- 4. Saving Results ---
    save_results(contactsStrength, outputPath);
    std::cout << "--- Module Finished ---" << std::endl;
}