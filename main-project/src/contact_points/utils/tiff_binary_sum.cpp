#include "src/include/tiff_binary_sum.hpp"
#include "src/include/common.hpp" // For the Image3D struct

#include <iostream>
#include <string>
#include <vector>

// --- I/O Placeholders (Requires libtiff) ---

// IMPORTANT: These are placeholders. A real implementation requires a TIFF library.
Image3D loadTiffImage_generic(const std::string& path) {
    std::cout << "WARNING: Function 'loadTiffImage_generic' is not implemented." << std::endl;
    return {};
}

void saveTiffImage_generic(const std::string& path, const Image3D& image) {
    std::cout << "WARNING: Function 'saveTiffImage_generic' is not implemented." << std::endl;
}


// --- Main Module Logic ---

void run_tiff_binary_sum(const std::string& inputFile1, const std::string& inputFile2, const std::string& outputFile) {
    std::cout << "--- Module: tiff_binary_sum ---" << std::endl;

    // --- 1. Data Loading ---
    Image3D image1 = loadTiffImage_generic(inputFile1);
    Image3D image2 = loadTiffImage_generic(inputFile2);

    if (image1.data.empty() || image2.data.empty()) {
        std::cerr << "Error: Failed to load one or more images. Aborting." << std::endl;
        return;
    }

    // --- 2. Dimension Validation ---
    if (image1.x_dim != image2.x_dim || image1.y_dim != image2.y_dim || image1.z_dim != image2.z_dim) {
        std::cerr << "Error: Input image dimensions do not match! Aborting." << std::endl;
        return;
    }
    std::cout << "Images loaded and dimensions validated." << std::endl;


    // --- 3. Binary Sum (Logical OR) ---
    Image3D sum_image = {
        std::vector<int>(image1.data.size()),
        image1.x_dim,
        image1.y_dim,
        image1.z_dim
    };

    for (size_t i = 0; i < image1.data.size(); ++i) {
        // If the pixel value is >= 255 (white) in either image, the output pixel is 255.
        // The result of the || (OR) operation is `true` (1) or `false` (0).
        sum_image.data[i] = (image1.data[i] >= 255 || image2.data[i] >= 255) * 255;
    }
    std::cout << "Binary sum complete." << std::endl;


    // --- 4. Saving the Result ---
    saveTiffImage_generic(outputFile, sum_image);

    std::cout << "Summed image saved to: " << outputFile << std::endl;
    std::cout << "--- Module Finished: tiff_binary_sum ---" << std::endl;
}