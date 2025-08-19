/**
 * @file colormap.cpp
 * @brief Applies a random colormap to a 3D label image.
 *
 * This program reads a 3D grayscale TIFF image containing integer labels,
 * generates a random RGB color for each unique label (keeping label 0 as black),
 * and saves the resulting colored image as a 3D RGB TIFF.
 */

 #include <iostream>
 #include <string>
 #include <vector>
 #include <set>
 #include <map>
 #include <random>
 #include <array>
 
 // xtensor
 #include "xtensor/xio.hpp"
 #include "xtensor/xview.hpp"
 #include "xtensor/xarray.hpp"
 #include "xtensor/xadapt.hpp"
 
 // Project utils
 #include "ImageProcessingUtils.h"
 
 int main(int argc, char* argv[]) {
     if (argc != 2) {
         std::cerr << "Usage: " << argv[0] << " <label_image.tif>" << std::endl;
         return 1;
     }
 
     std::string image_filepath = argv[1];
 
     // --- 1. Load Image ---
     // We load as uint32_t to support a large number of labels
     auto image = read_tiff_image_xt<uint32_t>(image_filepath);
     std::cout << "Loaded image with shape: " << image.shape()[0] << "x" << image.shape()[1] << "x" << image.shape()[2] << std::endl;
 
     // --- 2. Create Colormap (Look-Up Table) ---
     std::set<uint32_t> unique_labels;
     for (uint32_t label : image) {
         unique_labels.insert(label);
     }
 
     // High-quality random number generation
     std::random_device rd;
     std::mt19937 gen(rd());
     std::uniform_int_distribution<> distrib(0, 255);
 
     std::map<uint32_t, std::array<uint8_t, 3>> lut;
     for (uint32_t label : unique_labels) {
         lut[label] = {(uint8_t)distrib(gen), (uint8_t)distrib(gen), (uint8_t)distrib(gen)};
     }
 
     // Ensure the background (label 0) remains black
     lut[0] = {0, 0, 0};
 
     // --- 3. Apply Colormap ---
     auto shape = image.shape();
     size_t depth = shape[0];
     size_t height = shape[1];
     size_t width = shape[2];
     
     // Create the 4D output image (depth, height, width, color_channels)
     xt::xtensor<uint8_t, 4> colored_image = xt::zeros<uint8_t>({depth, height, width, 3});
 
     std::cout << "Applying colormap..." << std::endl;
     for (size_t d = 0; d < depth; ++d) {
         for (size_t h = 0; h < height; ++h) {
             for (size_t w = 0; w < width; ++w) {
                 uint32_t label = image(d, h, w);
                 const auto& color = lut.at(label);
                 // Use xt::view to assign the color to the (R, G, B) channels of the pixel
                 xt::view(colored_image, d, h, w, xt::all()) = xt::adapt(color);
             }
         }
     }
     
     // --- 4. Save Result ---
     std::string output_filepath = "colored.tif";
     write_rgb_tiff_image_xt(colored_image, output_filepath);
     std::cout << "Colored image saved to: " << output_filepath << std::endl;
 
     return 0;
 }