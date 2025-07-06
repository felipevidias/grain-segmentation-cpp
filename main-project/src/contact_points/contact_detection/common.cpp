#include "include/common.hpp" 
#include <fstream>
#include <iostream>

// --- Image3D Method Implementations ---

int& Image3D::at(long i, long j, long k) {
    // Calculates the 1D index from 3D coordinates.
    return data[k + z_dim * (j + y_dim * i)];
}

const int& Image3D::at(long i, long j, long k) const {
    // Calculates the 1D index from 3D coordinates (const version).
    return data[k + z_dim * (j + y_dim * i)];
}


// --- Algorithm Implementations ---

Image3D erosion(const Image3D& grains) {
    Image3D eroded = grains; // Start with a copy of the original image.
    
    // Offsets for 6-connectivity neighborhood (up, down, left, right, front, back).
    const int offsets[6][3] = {
        {-1, 0, 0}, {1, 0, 0}, {0, -1, 0},
        {0, 1, 0}, {0, 0, -1}, {0, 0, 1}
    };

    for (long i = 0; i < grains.x_dim; ++i) {
        for (long j = 0; j < grains.y_dim; ++j) {
            for (long k = 0; k < grains.z_dim; ++k) {
                // Skip background pixels.
                if (grains.at(i, j, k) == 0) continue;

                // Check all 6 neighbors.
                for (const auto& offset : offsets) {
                    long ni = i + offset[0];
                    long nj = j + offset[1];
                    long nk = k + offset[2];

                    // Check if neighbor is within image bounds.
                    if (ni >= 0 && ni < grains.x_dim && nj >= 0 && nj < grains.y_dim && nk >= 0 && nk < grains.z_dim) {
                        // If a neighbor is background, this pixel is eroded.
                        if (grains.at(ni, nj, nk) == 0) {
                            eroded.at(i, j, k) = 0;
                            break; // Move to the next pixel once eroded.
                        }
                    }
                }
            }
        }
    }
    return eroded;
}

void save_results(const std::map<std::pair<int, int>, int>& contactsStrength, const std::string& outputPath) {
    // Open the output file for writing.
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file: " << outputPath << std::endl;
        return;
    }

    // Write the CSV header.
    file << "Label1,Label2,ContactStrength\n";

    // Iterate through the map and write each contact to a new row.
    for (const auto& pair : contactsStrength) {
        const auto& contact_pair = pair.first;
        int strength = pair.second;
        file << contact_pair.first << "," << contact_pair.second << "," << strength << "\n";
    }
}