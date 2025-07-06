#include "include/getCentroid.hpp"
#include "src/include/common.hpp" // For the Image3D struct

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <numeric> // For std::accumulate
#include <queue>   // For the Breadth-First Search (BFS)
#include <tuple>

// --- I/O Placeholders ---

// IMPORTANT: This is a placeholder. A real implementation requires a TIFF library.
Image3D loadTiffImage(const std::string& path) {
    std::cout << "WARNING: Function 'loadTiffImage' is a placeholder. Returning empty image." << std::endl;
    return {};
}

// --- Internal Helper Structures ---

/**
 * @brief A helper struct to store properties of a single connected component (region).
 */
struct RegionProps {
    int label = 0;
    std::vector<std::tuple<int, int, int>> coords;
    std::tuple<double, double, double> centroid;
};


// --- Main Module Logic ---

void run_get_centroids(const std::string& grainsPath, const std::string& minTreePath, const std::string& outputPath) {
    std::cout << "--- Module: getCentroids ---" << std::endl;
    
    // --- 1. Data Loading ---
    Image3D mintree_image = loadTiffImage(minTreePath);
    if (mintree_image.data.empty()) {
        std::cerr << "Error: Failed to load the minTree image. Aborting." << std::endl;
        return;
    }

    // This image will store the unique label for each pixel.
    Image3D labeled_image = {std::vector<int>(mintree_image.data.size(), 0), mintree_image.x_dim, mintree_image.y_dim, mintree_image.z_dim};
    std::vector<RegionProps> regions;
    int current_label = 1;

    // --- 2. Connected-Component Labeling (equivalent to skimage.measure.label) ---
    // Iterate through every voxel to find starting points for new regions.
    for (int i = 0; i < mintree_image.x_dim; ++i) {
        for (int j = 0; j < mintree_image.y_dim; ++j) {
            for (int k = 0; k < mintree_image.z_dim; ++k) {
                // If a pixel is part of an object and hasn't been labeled yet, start a new region search.
                if (mintree_image.at(i, j, k) > 0 && labeled_image.at(i, j, k) == 0) {
                    RegionProps new_region;
                    new_region.label = current_label;
                    
                    std::queue<std::tuple<int, int, int>> q; // Queue for BFS
                    q.push({i, j, k});
                    labeled_image.at(i, j, k) = current_label;

                    // Perform BFS to find all connected pixels of this object.
                    while (!q.empty()) {
                        auto [ci, cj, ck] = q.front();
                        q.pop();
                        new_region.coords.push_back({ci, cj, ck});

                        // Check 6-connectivity neighbors.
                        int ni_offsets[] = {ci-1, ci+1, ci, ci, ci, ci};
                        int nj_offsets[] = {cj, cj, cj-1, cj+1, cj, cj};
                        int nk_offsets[] = {ck, ck, ck, ck, ck-1, ck+1};

                        for (int v = 0; v < 6; ++v) {
                            int ni = ni_offsets[v], nj = nj_offsets[v], nk = nk_offsets[v];
                            // Check bounds and if the neighbor is an unlabeled part of an object.
                            if (ni >= 0 && ni < mintree_image.x_dim && nj >= 0 && nj < mintree_image.y_dim && nk >= 0 && nk < mintree_image.z_dim &&
                                mintree_image.at(ni, nj, nk) > 0 && labeled_image.at(ni, nj, nk) == 0) {
                                labeled_image.at(ni, nj, nk) = current_label;
                                q.push({ni, nj, nk});
                            }
                        }
                    }
                    regions.push_back(new_region);
                    current_label++;
                }
            }
        }
    }
    std::cout << "Component labeling complete. Found " << regions.size() << " regions." << std::endl;

    // --- 3. Centroid Calculation (equivalent to skimage.measure.regionprops) ---
    std::vector<std::tuple<int, int, int, int>> centroids_to_write;
    for (auto& region : regions) {
        if (region.coords.empty()) continue;

        double sum_x = 0, sum_y = 0, sum_z = 0;
        for (const auto& coord : region.coords) {
            sum_x += std::get<0>(coord);
            sum_y += std::get<1>(coord);
            sum_z += std::get<2>(coord);
        }
        
        // Calculate the average coordinate.
        int centroid_x = static_cast<int>(sum_x / region.coords.size());
        int centroid_y = static_cast<int>(sum_y / region.coords.size());
        int centroid_z = static_cast<int>(sum_z / region.coords.size());
        
        // As in the Python script, get the label from the labeled image at the centroid's position.
        int label_at_centroid = labeled_image.at(centroid_x, centroid_y, centroid_z);
        centroids_to_write.emplace_back(centroid_x, centroid_y, centroid_z, label_at_centroid);
    }
    std::cout << "Centroid calculation complete." << std::endl;

    // --- 4. Saving Results ---
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << outputPath << std::endl;
        return;
    }
    
    file << "X,Y,Z,Label\n";
    for (const auto& c : centroids_to_write) {
        file << std::get<0>(c) << "," << std::get<1>(c) << "," << std::get<2>(c) << "," << std::get<3>(c) << "\n";
    }
    file.close();

    std::cout << "Centroids saved to " << outputPath << std::endl;
    std::cout << "--- Module Finished: getCentroids ---" << std::endl;
}