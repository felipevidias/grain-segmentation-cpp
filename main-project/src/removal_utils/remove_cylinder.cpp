/**
 * @file remove_cylinders.cpp
 * @brief Detects and removes cylinder artifacts from a 3D TIFF image.
 *
 * This program loads a 3D image, iterates through each 2D slice, detects
 * the boundaries of upper and lower cylinder artifacts, and overwrites these
 * regions with a dark gray value to effectively remove them.
 */

 #include <iostream>
 #include <string>
 #include <filesystem>
 
 // xtensor for data handling
 #include "xtensor/xio.hpp"
 #include "xtensor/xtranspose.hpp"
 
 // Project utils
 #include "ImageProcessingUtils.h"
 #include "ProgressBar.h"
 
 int main(int argc, char* argv[]) {
     if (argc != 2) {
         std::cerr << "Usage: " << argv[0] << " <image.tif>" << std::endl;
         return 1;
     }
 
     std::string filepath = argv[1];
     std::filesystem::path p(filepath);
     std::string filename = p.stem().string();
     
     // --- 1. Load and Transpose Image ---
     // The algorithm is designed to work on (depth, height, width), but the
     // source TIFF might be in a different order, so we transpose.
     auto raw_data_transposed = read_tiff_image_xt<uint16_t>(filepath);
     // Transposing from (d,h,w) to (h,d,w) to match python script's logic if needed.
     // Python (1,0,2) means new_axis_0 is old_axis_1, new_axis_1 is old_axis_0.
     // Let's assume the input TIFF is Z,Y,X and the algorithm works on Y,Z,X
     // For now, let's stick to the (d,h,w) order used in utils, assuming TIFF pages are Z slices.
     auto& raw_data = raw_data_transposed;
 
     const uint16_t FILL_COLOR = (128 << 8) >> 2; // BLACK_THRESHOLD >> 2
 
     // --- 2. Find and Remove Cylinders Slice by Slice ---
     for (size_t z = 0; z < raw_data.shape()[0]; ++z) {
         // Remove upper cylinder part
         CylinderBounds upper_bounds = find_cylinder_bounds(raw_data, z, true);
         write_quadshape_in_greyscale(raw_data, {upper_bounds.ydl, upper_bounds.yul, upper_bounds.ydr, upper_bounds.yur, upper_bounds.x_start - 50, upper_bounds.x_end + 50}, z, FILL_COLOR);
         
         // Remove lower cylinder part
         CylinderBounds lower_bounds = find_cylinder_bounds(raw_data, z, false);
         write_quadshape_in_greyscale(raw_data, {(int)raw_data.shape()[1], lower_bounds.yul, (int)raw_data.shape()[1], lower_bounds.yur, lower_bounds.x_start - 50, lower_bounds.x_end + 50}, z, FILL_COLOR);
         
         if ((z + 1) % 50 == 0 || z == raw_data.shape()[0] - 1) {
             print_progress_bar(z + 1, raw_data.shape()[0], "Removing cylinders...");
         }
     }
 
     // --- 3. Save Result ---
     std::string output_path = filename + "_cylinders_removed.tif";
     write_tiff_image_xt(raw_data, output_path);
     std::cout << "Result saved to " << output_path << std::endl;
 
     return 0;
 }