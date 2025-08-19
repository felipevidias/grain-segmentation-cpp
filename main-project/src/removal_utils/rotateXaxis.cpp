/**
 * @file rotateXaxis.cpp
 * @brief Rotates a 3D image 90 degrees around its X-axis by transposing the Z and Y axes.
 *
 * This program reads a 3D TIFF image, swaps its first two dimensions (depth and height),
 * and saves the resulting rotated image to a new TIFF file.
 */

 #include <iostream>
 #include <string>
 #include <filesystem>
 
 // xtensor for data handling
 #include "xtensor/xtranspose.hpp"
 #include "xtensor/xio.hpp"
 
 // Project utils for reading/writing TIFF files
 #include "ImageProcessingUtils.h"
 
 int main(int argc, char* argv[]) {
     if (argc != 2) {
         std::cerr << "Usage: " << argv[0] << " <image.tif>" << std::endl;
         return 1;
     }
 
     std::string filepath = argv[1];
     std::filesystem::path p(filepath);
     std::string filename = p.stem().string();
 
     try {
         // --- 1. Load Image ---
         // Assuming 16-bit data, which is common for this project's TIFFs.
         auto image = read_tiff_image_xt<uint16_t>(filepath);
         std::cout << "Loaded image with shape: (" << image.shape()[0] << ", "
                   << image.shape()[1] << ", " << image.shape()[2] << ")" << std::endl;
 
         // --- 2. Rotate (Transpose) Image ---
         // The permutation {1, 0, 2} swaps the first two axes (depth and height).
         auto rotated_image = xt::transpose(image, {1, 0, 2});
         std::cout << "Rotated image to shape: (" << rotated_image.shape()[0] << ", "
                   << rotated_image.shape()[1] << ", " << rotated_image.shape()[2] << ")" << std::endl;
 
         // --- 3. Save Result ---
         std::string output_path = filename + "_rotated.tif";
         write_tiff_image_xt(rotated_image, output_path);
         std::cout << "Successfully saved rotated image to: " << output_path << std::endl;
 
     } catch (const std::exception& e) {
         std::cerr << "An error occurred: " << e.what() << std::endl;
         return 1;
     }
 
     return 0;
 }