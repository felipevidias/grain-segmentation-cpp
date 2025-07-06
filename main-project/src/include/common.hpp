#pragma once

#include <vector>
#include <string>
#include <map>
#include <utility>

/**
 * @brief A simple container for 3D image data.
 *
 * This struct stores voxel data in a flattened 1D vector for cache
 * efficiency and provides methods for easy 3D coordinate access.
 */
struct Image3D {
    std::vector<int> data;
    long x_dim = 0, y_dim = 0, z_dim = 0;

    /**
     * @brief Provides direct write access to a voxel at coordinates (i, j, k).
     * @param i The x-coordinate (row).
     * @param j The y-coordinate (column).
     * @param k The z-coordinate (slice).
     * @return A reference to the voxel value.
     */
    int& at(long i, long j, long k);

    /**
     * @brief Provides direct read-only access to a voxel at coordinates (i, j, k).
     * @param i The x-coordinate (row).
     * @param j The y-coordinate (column).
     * @param k The z-coordinate (slice).
     * @return A const reference to the voxel value.
     */
    const int& at(long i, long j, long k) const;
};

/**
 * @brief Performs one step of morphological erosion on a 3D image.
 *
 * This function iterates through each non-zero voxel and sets it to zero
 * if any of its 6-connected neighbors has a value of zero.
 * @param grains The input Image3D object to be eroded.
 * @return A new Image3D object representing the eroded result.
 */
Image3D erosion(const Image3D& grains);

/**
 * @brief Saves detected contacts and their strengths to a CSV file.
 *
 * The output CSV will have three columns: "Label1", "Label2", and "ContactStrength".
 * @param contactsStrength A map where the key is a pair of grain labels and
 * the value is the calculated contact strength.
 * @param outputPath The path, including filename, for the output CSV file.
 */
void save_results(const std::map<std::pair<int, int>, int>& contactsStrength, const std::string& outputPath);