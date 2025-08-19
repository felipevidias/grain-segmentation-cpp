/**
 * @file ImageProcessingUtils.h
 * @brief Declares utility functions for 3D image processing tasks.
 *
 * This file provides a consolidated interface for various utilities used across the project, including:
 * - Reading and writing 3D TIFF images (grayscale and RGB) using xtensor.
 * - Structs for data representation (e.g., Centroid, CylinderBounds).
 * - Wrappers for image processing libraries (e.g., PINK).
 * - Functions for data analysis (e.g., calculating centroids).
 * - CSV file I/O for centroids.
 * - Image cropping, pasting, and manipulation utilities.
 */

 #ifndef IMAGE_PROCESSING_UTILS_H
 #define IMAGE_PROCESSING_UTILS_H
 
 #include <string>
 #include <vector>
 #include "xtensor/core/xtensor.hpp"
 
 // ====================================================================
 // Data Structures
 // ====================================================================
 
 /**
  * @struct Centroid
  * @brief Represents a single centroid with its 3D coordinates and label.
  * Uses 'int' to match the data types from the source CSV file.
  */
 struct Centroid {
     int x, y, z;
     int label;
 };
 
 /**
  * @struct CylinderBounds
  * @brief Holds the calculated boundary coordinates for a cylinder region in a 2D slice.
  */
 struct CylinderBounds {
     int ydl, yul, ydr, yur, x_start, x_end;
 };
 
 
 // ====================================================================
 // TIFF Image I/O Functions
 // ====================================================================
 
 /**
  * @brief Reads a 3D grayscale TIFF file into a 3D xtensor array.
  * @tparam T The data type of the pixels (e.g., uint8_t, uint16_t, uint32_t).
  * @param filepath The path to the TIFF file.
  * @return An xt::xtensor<T, 3> containing the image data.
  */
 template<typename T>
 xt::xtensor<T, 3> read_tiff_image_xt(const std::string& filepath);
 
 /**
  * @brief Writes a 3D grayscale xtensor array to a TIFF file.
  * @tparam T The data type of the pixels.
  * @param image The xt::xtensor<T, 3> to be saved.
  * @param filepath The path for the output TIFF file.
  */
 template<typename T>
 void write_tiff_image_xt(const xt::xtensor<T, 3>& image, const std::string& filepath);
 
 /**
  * @brief Writes a 3D RGB xtensor array to a multi-page TIFF file.
  * @param image The xt::xtensor<uint8_t, 4> to be saved, with shape (depth, height, width, 3).
  * @param filepath The path for the output TIFF file.
  */
 void write_rgb_tiff_image_xt(const xt::xtensor<uint8_t, 4>& image, const std::string& filepath);
 
 
 // ====================================================================
 // PINK Library Wrappers (Placeholders)
 // ====================================================================
 
 /**
  * @brief Performs 3D morphological dilation with a ball structuring element.
  * @note This is a wrapper for a function from the PINK image processing library.
  * @param image The input 8-bit 3D image.
  * @param radius The radius of the ball structuring element.
  * @return The dilated 3D image.
  */
 xt::xtensor<uint8_t, 3> dilate_with_ball(const xt::xtensor<uint8_t, 3>& image, float radius);
 
 /**
  * @brief Finds and labels connected components in a 3D binary image.
  * @note This is a wrapper for a function from the PINK image processing library.
  * @param image The input 8-bit 3D binary image.
  * @param num_components A reference to an integer that will store the total number of components found.
  * @return A 3D image with each component assigned a unique integer label.
  */
 xt::xtensor<uint32_t, 3> label_components(const xt::xtensor<uint8_t, 3>& image, int& num_components);
 
 /**
  * @brief Performs a morphological area closing on a 3D binary image.
  * @note This is a wrapper for a function from the PINK image processing library.
  * @param binary_image The input 3D binary image.
  * @return The image after the area closing operation.
  */
 xt::xtensor<uint8_t, 3> area_closing(const xt::xtensor<uint8_t, 3>& binary_image);
 
 /**
  * @brief Computes the exact Euclidean Distance Transform (EDT) of a binary image.
  * @note This is a wrapper for a function from the PINK image processing library.
  * @param binary_image The input 3D binary image.
  * @return A 3D float image where each pixel's value is its distance to the nearest background pixel.
  */
 xt::xtensor<float, 3> distance_transform_edt(const xt::xtensor<uint8_t, 3>& binary_image);
 
 
 // ====================================================================
 // Analysis and Manipulation Functions
 // ====================================================================
 
 /**
  * @brief Load centroids from a CSV file.
  * @param filepath Path to the CSV file containing centroids.
  * @return A vector of Centroid structs.
  */
 std::vector<Centroid> loadCentroidsFromCSV(const std::string& filepath);
 
 /**
  * @brief Calculates the centroid (center of mass) for each labeled region in a 3D image.
  * @param labeled_image A 3D image where each connected component has a unique integer label.
  * @return A vector of Centroid structs.
  */
 std::vector<Centroid> calculate_centroids(const xt::xtensor<uint32_t, 3>& labeled_image);
 
 /**
  * @brief Writes a vector of Centroid structs to a CSV file.
  * @param filepath The path for the output CSV file.
  * @param centroids The vector of Centroid structs to write.
  */
 void write_centroids_csv(const std::string& filepath, const std::vector<Centroid>& centroids);
 
 /**
  * @brief Crops a cube around the point (x, y, z) in the given 3D image.
  */
 template<typename T>
 xt::xtensor<T, 3> cropAroundPoint(const xt::xtensor<T, 3>& image, int cx, int cy, int cz, int crop_size);
 
 /**
  * @brief Pastes a labeled crop back into a larger image.
  */
 void positioning_label(xt::xtensor<uint32_t, 3>& labeled_image, const xt::xtensor<uint32_t, 3>& labeled_crop,
                        const Centroid& centroid, int crop_size);
 
 /**
  * @brief Scans a 2D image slice to find the physical boundaries of a cylinder.
  */
 CylinderBounds find_cylinder_bounds(const xt::xtensor<uint16_t, 3>& image, size_t z_slice, bool is_upper_cylinder);
 
 /**
  * @brief Fills a quadrilateral area in a 2D slice with a specified grayscale value.
  */
 void write_quadshape_in_greyscale(xt::xtensor<uint16_t, 3>& image, const CylinderBounds& bounds, size_t z_slice, uint16_t greyscale);
 
 
 #endif // IMAGE_PROCESSING_UTILS_H