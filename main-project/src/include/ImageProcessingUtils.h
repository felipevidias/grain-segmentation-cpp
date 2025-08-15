/**
 * @file ImageProcessingUtils.h
 * @brief Declares utility functions for 3D image processing tasks.
 *
 * This file provides a consolidated interface for various utilities used across the project, including:
 * - Reading and writing 3D TIFF images (grayscale and RGB) using xtensor.
 * - Wrappers for image processing libraries (e.g., PINK).
 */

 #ifndef IMAGE_PROCESSING_UTILS_H
 #define IMAGE_PROCESSING_UTILS_H
 
 #include <string>
 #include "xtensor/xtensor.hpp"
 
 /**
  * @brief Reads a 3D grayscale TIFF file into a 3D xtensor array.
  * @tparam T The data type of the pixels (e.g., uint8_t, uint16_t).
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
 
 #endif // IMAGE_PROCESSING_UTILS_H