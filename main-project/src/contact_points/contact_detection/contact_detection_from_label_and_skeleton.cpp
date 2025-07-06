#include "include/contact_detection_from_label_and_skeleton.hpp"
#include "include/common.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // For system()
#include <map>
#include <algorithm> // For std::find
#include <tuple>

// --- I/O Placeholders (TO BE IMPLEMENTED) ---

// IMPORTANT: These are placeholders. A real implementation requires a TIFF/RAW library.
Image3D loadTiffImage(const std::string& path) {
    std::cout << "WARNING: Function 'loadTiffImage' is a placeholder. Returning empty image." << std::endl; return {};
}
Image3D loadRawImage(const std::string& path, int x, int y, int z) {
    std::cout << "WARNING: Function 'loadRawImage' is a placeholder. Returning empty image." << std::endl; return {};
}


// --- Algorithm Helper Functions ---

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
 * @brief Checks the 6 neighbors of a single voxel for contacts with other grains.
 * @param i The x-coordinate of the voxel to check.
 * @param j The y-coordinate of the voxel to check.
 * @param k The z-coordinate of the voxel to check.
 * @param labels A const reference to the labeled image for the current erosion step.
 * @param contactDict A reference to the map where contacts for the current iteration are stored.
 */
void detect_contact_on_pixel_cpp(int i, int j, int k,
                                 const Image3D& labels,
                                 std::map<int, std::vector<int>>& contactDict)
{
    int current_pixel_label = labels.at(i, j, k);
    auto neighbors = get_neighbors(i, j, k);

    for (const auto& neighbor_coords : neighbors) {
        int ni = std::get<0>(neighbor_coords);
        int nj = std::get<1>(neighbor_coords);
        int nk = std::get<2>(neighbor_coords);

        // Boundary check.
        if (ni < 0 || ni >= labels.x_dim || nj < 0 || nj >= labels.y_dim || nk < 0 || nk >= labels.z_dim) {
            continue;
        }

        int neighbor_pixel_label = labels.at(ni, nj, nk);

        // Skip if the neighbor is background or part of the same grain.
        if (neighbor_pixel_label == 0 || current_pixel_label == neighbor_pixel_label) {
            continue;
        }

        // To avoid duplicates (e.g., 1-2 and 2-1), only store the contact for the smaller label.
        if (current_pixel_label < neighbor_pixel_label) {
            // Check if this contact has already been found for this grain to avoid duplicates in the list.
            auto& vec = contactDict[current_pixel_label];
            if (std::find(vec.begin(), vec.end(), neighbor_pixel_label) == vec.end()) {
                vec.push_back(neighbor_pixel_label);
            }
        }
    }
}


// --- Main Module Logic ---

void run_contact_detection_from_label_and_skeleton() {
    // --- 1. Argument Parsing (Hardcoded Placeholders) ---
    std::string grainsPath = "../data/grains.tif";
    std::string labelPath = "../data/label.tif";
    int x = 100, y = 100, z = 100;
    bool keep_files = false;
    std::string pinkDir = "../Pink/linux/bin/";
    std::string outputPath = "../results/contacts_using_skeleton.csv";

    // --- 2. Pre-processing via External Tools ---
    std::cout << "--- Module: Contact Detection from Label and Skeleton ---" << std::endl;
    std::cout << "Starting pre-processing using external scripts..." << std::endl;
    system("mkdir -p tmp");
    system(("python3 ../utils/minTree.py " + grainsPath + " 6 --output=tmp/minTree.tif").c_str());
    // ... other system() calls for binarization, conversion, and skeletonization go here ...
    system((pinkDir + "pgm2raw tmp/skeleton").c_str());
    std::cout << "Pre-processing complete." << std::endl;

    // --- 3. Data Loading ---
    std::cout << "Loading data..." << std::endl;
    Image3D label = loadTiffImage(labelPath);
    Image3D skeleton = loadRawImage("tmp/skeleton.raw", x, y, z);
    
    // --- 4. Contact Detection: Main Iterative Loop ---
    std::map<std::pair<int, int>, int> contactsStrength;
    Image3D current_labels = label;
    bool havingContacts = true;
    int contactStrength = 0;

    while (havingContacts) {
        contactStrength++;
        std::cout << "Erosion level (Contact Strength): " << contactStrength << std::endl;
        
        std::map<int, std::vector<int>> contacts_this_iteration;
        
        // Iterate through all voxels.
        for (int i = 0; i < x; ++i) {
            for (int j = 0; j < y; ++j) {
                for (int k = 0; k < z; ++k) {
                    // The key optimization: only check for contacts on skeleton voxels.
                    if (skeleton.at(i, j, k) != 0 && current_labels.at(i, j, k) != 0) {
                        detect_contact_on_pixel_cpp(i, j, k, current_labels, contacts_this_iteration);
                    }
                }
            }
        }

        // If contacts were found in this pass, store their strength and erode the image for the next pass.
        havingContacts = !contacts_this_iteration.empty();
        if (havingContacts) {
            for (const auto& pair : contacts_this_iteration) {
                int grain1 = pair.first;
                for (int grain2 : pair.second) {
                    contactsStrength[{grain1, grain2}] = contactStrength;
                }
            }
            current_labels = erosion(current_labels); // from common.hpp
        }
    }
    
    // --- 5. Cleanup & Saving Results ---
    if (!keep_files) {
        system("rm -r tmp");
    }

    save_results(contactsStrength, outputPath); // from common.hpp
    std::cout << "--- Module Finished ---" << std::endl;
}