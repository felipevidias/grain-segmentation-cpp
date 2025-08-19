/**
 * @file ImageProcessingUtils.cpp
 * @brief Implements the consolidated utility functions for 3D image processing.
 */

 #include "ImageProcessingUtils.h"

 #include <tiffio.h>
 #include <iostream>
 #include <stdexcept>
 #include <map>
 #include <tuple>
 #include <fstream>
 #include <sstream>
 #include <algorithm>
 #include <cmath> // For std::floor, std::ceil
 #include "xtensor/xadapt.hpp"
 #include "xtensor/xview.hpp"
 #include "xtensor/xio.hpp"
 #include "xtensor/xmasked_view.hpp"
 
 
 // Explicit template instantiations to allow defining templates in a .cpp file.
 template xt::xtensor<uint8_t, 3> read_tiff_image_xt<uint8_t>(const std::string&);
 template xt::xtensor<uint16_t, 3> read_tiff_image_xt<uint16_t>(const std::string&);
 template xt::xtensor<uint32_t, 3> read_tiff_image_xt<uint32_t>(const std::string&);
 
 template void write_tiff_image_xt<uint8_t>(const xt::xtensor<uint8_t, 3>&, const std::string&);
 template void write_tiff_image_xt<uint16_t>(const xt::xtensor<uint16_t, 3>&, const std::string&);
 template void write_tiff_image_xt<uint32_t>(const xt::xtensor<uint32_t, 3>&, const std::string&);
 
 template xt::xtensor<uint8_t, 3> cropAroundPoint<uint8_t>(const xt::xtensor<uint8_t, 3>&, int, int, int, int);
 template xt::xtensor<uint16_t, 3> cropAroundPoint<uint16_t>(const xt::xtensor<uint16_t, 3>&, int, int, int, int);
 template xt::xtensor<uint32_t, 3> cropAroundPoint<uint32_t>(const xt::xtensor<uint32_t, 3>&, int, int, int, int);
 
 
 // ====================================================================
 // TIFF Image I/O Functions
 // ====================================================================
 
 template<typename T>
 xt::xtensor<T, 3> read_tiff_image_xt(const std::string& filepath) {
     TIFF* tif = TIFFOpen(filepath.c_str(), "r");
     if (!tif) {
         throw std::runtime_error("Error: Could not open TIFF file: " + filepath);
     }
 
     uint32_t width, height;
     size_t depth = 0;
     
     do {
         depth++;
     } while (TIFFReadDirectory(tif));
 
     TIFFSetDirectory(tif, 0);
     TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
     TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
 
     xt::xtensor<T, 3> image = xt::zeros<T>({depth, height, width});
 
     for (size_t d = 0; d < depth; ++d) {
         TIFFSetDirectory(tif, d);
         for (uint32_t row = 0; row < height; ++row) {
             auto row_view = xt::view(image, d, row, xt::all());
             TIFFReadScanline(tif, row_view.data(), row);
         }
     }
 
     TIFFClose(tif);
     return image;
 }
 
 template<typename T>
 void write_tiff_image_xt(const xt::xtensor<T, 3>& image, const std::string& filepath) {
     TIFF* out = TIFFOpen(filepath.c_str(), "w");
     if (!out) {
         throw std::runtime_error("Error: Could not open file for writing: " + filepath);
     }
 
     auto shape = image.shape();
     size_t depth = shape[0];
     size_t height = shape[1];
     size_t width = shape[2];
 
     for (size_t d = 0; d < depth; ++d) {
         TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
         TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
         TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
         TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, sizeof(T) * 8);
         TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
         TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
         TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
         
         for (size_t row = 0; row < height; ++row) {
             auto row_view = xt::view(image, d, row, xt::all());
             TIFFWriteScanline(out, const_cast<T*>(row_view.data()), row);
         }
         TIFFWriteDirectory(out);
     }
 
     TIFFClose(out);
 }
 
 void write_rgb_tiff_image_xt(const xt::xtensor<uint8_t, 4>& image, const std::string& filepath) {
     TIFF* out = TIFFOpen(filepath.c_str(), "w");
     if (!out) {
         throw std::runtime_error("Error: Could not open file for writing: " + filepath);
     }
 
     auto shape = image.shape();
     if (shape.size() != 4 || shape[3] != 3) {
         TIFFClose(out);
         throw std::runtime_error("Error: Image for RGB TIFF must have 4 dimensions with the last one being size 3.");
     }
 
     size_t depth = shape[0];
     size_t height = shape[1];
     size_t width = shape[2];
 
     for (size_t d = 0; d < depth; ++d) {
         TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
         TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
         TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
         TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
         TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
         TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
         TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
         
         for (size_t row = 0; row < height; ++row) {
             auto row_view = xt::view(image, d, row, xt::all(), xt::all());
             TIFFWriteScanline(out, const_cast<uint8_t*>(row_view.data()), row);
         }
         TIFFWriteDirectory(out);
     }
 
     TIFFClose(out);
 }
 
 // ====================================================================
 // PINK Library Wrappers (Placeholders)
 // ====================================================================
 
 xt::xtensor<uint8_t, 3> dilate_with_ball(const xt::xtensor<uint8_t, 3>& image, float radius) {
     // TODO: Replace this placeholder with the actual call to the PINK library's dilation function.
     std::cout << "[INFO] Placeholder for PINK Dilation with ball of radius " << radius << std::endl;
     return image;
 }
 
 xt::xtensor<uint32_t, 3> label_components(const xt::xtensor<uint8_t, 3>& image, int& num_components) {
     // TODO: Replace this placeholder with the actual call to the PINK library's labeling function.
     std::cout << "[INFO] Placeholder for PINK Label Components" << std::endl;
     num_components = 5; // Example value
     return xt::cast<uint32_t>(image);
 }
 
 xt::xtensor<uint8_t, 3> area_closing(const xt::xtensor<uint8_t, 3>& binary_image) {
     // TODO: Replace this placeholder with the actual call to the PINK library's area closing function.
     std::cout << "[INFO] Placeholder for PINK area_closing" << std::endl;
     return binary_image;
 }
 
 xt::xtensor<float, 3> distance_transform_edt(const xt::xtensor<uint8_t, 3>& binary_image) {
     // TODO: Replace this placeholder with the actual call to the PINK library's distance transform function.
     std::cout << "[INFO] Placeholder for PINK distance_transform_edt" << std::endl;
     return xt::cast<float>(binary_image);
 }
 
 // ====================================================================
 // Analysis and Manipulation Functions
 // ====================================================================
 
 std::vector<Centroid> loadCentroidsFromCSV(const std::string& filepath) {
     std::vector<Centroid> centroids;
     std::ifstream file(filepath);
     if (!file.is_open()) {
         throw std::runtime_error("Error: Could not open centroids CSV file: " + filepath);
     }
 
     std::string line;
     std::getline(file, line); // Skip header
 
     while (std::getline(file, line)) {
         std::stringstream ss(line);
         std::string value;
         Centroid c;
         
         std::getline(ss, value, ','); c.x = std::stoi(value);
         std::getline(ss, value, ','); c.y = std::stoi(value);
         std::getline(ss, value, ','); c.z = std::stoi(value);
         std::getline(ss, value, ','); c.label = std::stoi(value);
         
         centroids.push_back(c);
     }
     return centroids;
 }
 
 std::vector<Centroid> calculate_centroids(const xt::xtensor<uint32_t, 3>& labeled_image) {
     std::map<uint32_t, std::tuple<std::array<double, 3>, size_t>> sums;
     auto shape = labeled_image.shape();
     for (size_t d = 0; d < shape[0]; ++d) {
         for (size_t h = 0; h < shape[1]; ++h) {
             for (size_t w = 0; w < shape[2]; ++w) {
                 uint32_t label = labeled_image(d, h, w);
                 if (label > 0) {
                     auto& [coords, count] = sums[label];
                     coords[0] += d; coords[1] += h; coords[2] += w;
                     count++;
                 }
             }
         }
     }
     std::vector<Centroid> centroids;
     for (const auto& pair : sums) {
         uint32_t label = pair.first;
         const auto& [coords, count] = pair.second;
         if (count > 0) {
             centroids.push_back({coords[0] / count, coords[1] / count, coords[2] / count, (int)label});
         }
     }
     return centroids;
 }
 
 void write_centroids_csv(const std::string& filepath, const std::vector<Centroid>& centroids) {
     std::ofstream file(filepath);
     if (!file.is_open()) {
         throw std::runtime_error("Error: Could not open CSV for writing: " + filepath);
     }
     file << "X,Y,Z,Label\n";
     for (const auto& c : centroids) {
         file << c.x << "," << c.y << "," << c.z << "," << c.label << "\n";
     }
     std::cout << "Centroids have been saved to " << filepath << std::endl;
 }
 
 template<typename T>
 xt::xtensor<T, 3> cropAroundPoint(const xt::xtensor<T, 3>& image, int cx, int cy, int cz, int crop_size) {
     int half_side = crop_size / 2;
     auto shape = image.shape();
     int x_min = std::max(0, cx - half_side);
     int x_max = std::min((int)shape[0], cx + half_side + 1);
     int y_min = std::max(0, cy - half_side);
     int y_max = std::min((int)shape[1], cy + half_side + 1);
     int z_min = std::max(0, cz - half_side);
     int z_max = std::min((int)shape[2], cz + half_side + 1);
     return xt::view(image, xt::range(x_min, x_max), xt::range(y_min, y_max), xt::range(z_min, z_max));
 }
 
 void positioning_label(xt::xtensor<uint32_t, 3>& labeled_image, const xt::xtensor<uint32_t, 3>& labeled_crop,
                        const Centroid& centroid, int crop_size) {
     int half_crop_size = crop_size / 2;
     auto img_shape = labeled_image.shape();
     auto crop_shape = labeled_crop.shape();
     int x_min_img = std::max(0, centroid.x - half_crop_size);
     int y_min_img = std::max(0, centroid.y - half_crop_size);
     int z_min_img = std::max(0, centroid.z - half_crop_size);
     for (size_t i = 0; i < crop_shape[0]; ++i) {
         for (size_t j = 0; j < crop_shape[1]; ++j) {
             for (size_t k = 0; k < crop_shape[2]; ++k) {
                 if (labeled_crop(i, j, k) > 0) {
                     size_t target_x = x_min_img + i;
                     size_t target_y = y_min_img + j;
                     size_t target_z = z_min_img + k;
                     if (target_x < img_shape[0] && target_y < img_shape[1] && target_z < img_shape[2]) {
                          labeled_image(target_x, target_y, target_z) = centroid.label;
                     }
                 }
             }
         }
     }
 }
 
 CylinderBounds find_cylinder_bounds(const xt::xtensor<uint16_t, 3>& image, size_t z_slice, bool is_upper_cylinder) {
     const uint16_t BLACK_THRESHOLD = (128 << 8);
     int step = 1;
     int y_start = 0;
     int y_max = image.shape()[1] - 1;
 
     if (!is_upper_cylinder) {
         y_start = image.shape()[1] - 1;
         y_max = 0;
         step = -1;
     }
     
     int y = y_start;
     size_t x = image.shape()[2] / 2;
     while (y != y_max && image(z_slice, y, x) < BLACK_THRESHOLD) y += step;
     int yum = y;
     while (y != y_max && image(z_slice, y, x) > BLACK_THRESHOLD) y += step;
     int ydm = y;
     int cylinder_height = std::abs(yum - ydm) / 2;
     cylinder_height = (is_upper_cylinder) ? yum - cylinder_height : ydm + cylinder_height;
     
     size_t x_start = 0;
     while (x_start < image.shape()[2] && image(z_slice, cylinder_height, x_start) < BLACK_THRESHOLD) x_start++;
     size_t x_end = image.shape()[2] - 1;
     while (x_end > 0 && image(z_slice, cylinder_height, x_end) < BLACK_THRESHOLD) x_end--;
 
     y = y_start;
     x = x_start + (x_end - x_start) / 3;
     while (y != y_max && image(z_slice, y, x) < BLACK_THRESHOLD) y += step;
     int yul = y;
     while (y != y_max && image(z_slice, y, x) > BLACK_THRESHOLD) y += step;
     int ydl = y;
 
     y = y_start;
     x += (x - x_start);
     while (y != y_max && image(z_slice, y, x) < BLACK_THRESHOLD) y += step;
     int yur = y;
     while (y != y_max && image(z_slice, y, x) > BLACK_THRESHOLD) y += step;
     int ydr = y;
 
     ydl -= ((ydl - ydm) / 2);
     yul -= ((yul - yum) / 2);
     ydr -= ((ydr - ydm) / 2);
     yur -= ((yur - yum) / 2);
 
     if (is_upper_cylinder) {
         return {ydl, yul, ydr, yur, (int)x_start, (int)x_end};
     } else {
         return {yul, ydl, yur, ydr, (int)x_start, (int)x_end};
     }
 }
 
 void write_quadshape_in_greyscale(xt::xtensor<uint16_t, 3>& image, const CylinderBounds& bounds, size_t z_slice, uint16_t greyscale) {
     if (bounds.x_start >= bounds.x_end) return;
     
     double coef_dir_up = static_cast<double>(bounds.yul - bounds.yur) / (bounds.x_end - bounds.x_start);
     double coef_dir_down = static_cast<double>(bounds.ydl - bounds.ydr) / (bounds.x_end - bounds.x_start);
 
     for (int x = bounds.x_start; x < std::min(bounds.x_end + 1, (int)image.shape()[2]); ++x) {
         int y_start_fill = std::max(0, bounds.yur + static_cast<int>(std::ceil(coef_dir_up * (x - bounds.x_start))));
         int y_end_fill = std::min((int)image.shape()[1], bounds.ydr + static_cast<int>(std::floor(coef_dir_down * (x - bounds.x_start))) + 1);
 
         for (int y = y_start_fill; y < y_end_fill; ++y) {
             image(z_slice, y, x) = greyscale;
         }
     }
 }