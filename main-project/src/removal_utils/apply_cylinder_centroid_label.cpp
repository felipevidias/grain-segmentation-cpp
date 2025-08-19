/**
 * @file apply_cylinder_centroid_label.cpp
 * @brief Finds cylinder boundaries in a 3D image and adds their centroids to a CSV file.
 *
 * This program reads a 3D TIFF image and a CSV file of existing centroids.
 * It algorithmically detects the upper and lower cylinder regions, calculates their
 * center points, and writes a new CSV file containing both the original and the
 * new cylinder centroids with special labels (-1 and -2).
 */

 #include <iostream>
 #include <string>
 #include <vector>
 #include <algorithm> // For std::min/max
 #include <filesystem>
 
 // xtensor for data handling
 #include "xtensor/xio.hpp"
 
 // Project utils
 #include "ImageProcessingUtils.h"
 
 // --- Global Constants ---
 const uint16_t BLACK_THRESHOLD = (128 << 8); // Greyscale value for 16-bit images
 const int CYLINDER_LABEL_HIGH = -1;
 const int CYLINDER_LABEL_LOW = -2;
 
 // --- Core Algorithm Functions ---
 
 /**
  * @struct CylinderBounds
  * @brief Holds the calculated boundary coordinates for a cylinder region.
  */
 struct CylinderBounds {
     int ydl, yul, ydr, yur, x_start, x_end;
 };
 
 /**
  * @brief Scans a 3D image to find the physical boundaries of a cylinder.
  * @note This is a direct C++ translation of the Python pixel-scanning algorithm.
  * The input image is expected to be indexed as (depth, height, width).
  */
 CylinderBounds find_cylinder_bounds(const xt::xtensor<uint16_t, 3>& image, size_t z, bool is_upper_cylinder) {
     int step = 1;
     int y_start = 0;
     int y_max = image.shape()[1] - 1; // dimension 1 is height
 
     if (!is_upper_cylinder) {
         y_start = image.shape()[1] - 1;
         y_max = 0;
         step = -1;
     }
 
     // Middle + cylinder height
     int y = y_start;
     size_t x = image.shape()[2] / 2; // dimension 2 is width
     while (y != y_max && image(z, y, x) < BLACK_THRESHOLD) y += step;
     int yum = y;
     while (y != y_max && image(z, y, x) > BLACK_THRESHOLD) y += step;
     int ydm = y;
     int cylinder_height = std::abs(yum - ydm) / 2;
     // Adjust height to be relative to the starting edge of the cylinder object
     cylinder_height = (is_upper_cylinder) ? yum - cylinder_height : ydm + cylinder_height;
 
 
     // Cylinder width
     size_t x_start = 0;
     while (x_start < image.shape()[2] && image(z, cylinder_height, x_start) < BLACK_THRESHOLD) x_start++;
     size_t x_end = image.shape()[2] - 1;
     while (x_end > 0 && image(z, cylinder_height, x_end) < BLACK_THRESHOLD) x_end--;
 
     // Left
     y = y_start;
     x = x_start + (x_end - x_start) / 3;
     while (y != y_max && image(z, y, x) < BLACK_THRESHOLD) y += step;
     int yul = y;
     while (y != y_max && image(z, y, x) > BLACK_THRESHOLD) y += step;
     int ydl = y;
 
     // Right
     y = y_start;
     x += (x - x_start);
     while (y != y_max && image(z, y, x) < BLACK_THRESHOLD) y += step;
     int yur = y;
     while (y != y_max && image(z, y, x) > BLACK_THRESHOLD) y += step;
     int ydr = y;
 
     // Readjust to middle
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
 
 Centroid compute_average(const CylinderBounds& bounds, size_t z_coord) {
     Centroid center;
     // Note: Python script assigns x_average = x_end - x_start, etc.
     // This seems to be calculating size, not a coordinate. We will store
     // the calculated sizes in the coordinate fields as per the script's logic.
     center.x = bounds.x_end - bounds.x_start;
     center.y = std::min(bounds.ydl, bounds.ydr) - std::max(bounds.yul, bounds.yur);
     center.z = z_coord;
     return center;
 }
 
 void add_cylinder_centroids(const xt::xtensor<uint16_t, 3>& image, std::vector<Centroid>& centroids) {
     size_t z_slice = image.shape()[0] / 2; // z in python was depth (dim 0)
 
     // Add centroid to upper cylinder
     CylinderBounds upper_bounds = find_cylinder_bounds(image, z_slice, true);
     Centroid upper_center = compute_average(upper_bounds, z_slice);
     upper_center.label = CYLINDER_LABEL_HIGH;
     centroids.push_back(upper_center);
 
     // Add centroid to lower cylinder
     CylinderBounds lower_bounds = find_cylinder_bounds(image, z_slice, false);
     Centroid lower_center = compute_average(lower_bounds, z_slice);
     lower_center.label = CYLINDER_LABEL_LOW;
     centroids.push_back(lower_center);
 }
 
 // --- Main Program ---
 int main(int argc, char* argv[]) {
     if (argc != 4) {
         std::cerr << "Usage: " << argv[0] << " <watershed.tif> <centroids.csv> <seed.tif>" << std::endl;
         return 1;
     }
 
     std::string filepath = argv[1];
     std::string centroids_path = argv[2];
     std::string seed_path = argv[3];
 
     std::filesystem::path p(centroids_path);
     std::string centroids_name = p.stem().string();
 
     // The script logic uses the raw watershed image for scanning
     auto raw_data = read_tiff_image_xt<uint16_t>(filepath);
     
     // The seed image and labeled_centroids are loaded but not used in the core C++ logic,
     // mirroring the Python script's structure.
     // auto seed_data = read_tiff_image_xt<uint16_t>(seed_path);
     
     std::vector<Centroid> centroids = loadCentroidsFromCSV(centroids_path);
     
     add_cylinder_centroids(raw_data, centroids);
 
     std::string output_filename = centroids_name + "_with_cylinder_labels.csv";
     write_centroids_csv(output_filename, centroids);
     
     return 0;
 }