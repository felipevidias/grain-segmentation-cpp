/**
 * @file watershed_function.h
 * @brief Declares the core watershed segmentation algorithm function.
 */

 #ifndef WATERSHED_FUNCTION_H
 #define WATERSHED_FUNCTION_H
 
 #include "xtensor/xtensor.hpp"
 
 /**
  * @brief Performs a seeded watershed segmentation.
  * @note This is a placeholder. You must implement your actual watershed algorithm here.
  * @param image The input 3D grayscale image to be segmented.
  * @param seed The 3D marker/seed image.
  * @param labeled_centroids A 3D image where the location of each centroid is marked with its unique label.
  * @param adjacency The neighborhood connectivity (6 or 26).
  * @return A 3D image with segmented regions labeled according to the seeds.
  */
 xt::xtensor<uint32_t, 3> watershed(
     const xt::xtensor<uint8_t, 3>& image,
     const xt::xtensor<uint8_t, 3>& seed,
     const xt::xtensor<uint32_t, 3>& labeled_centroids,
     int adjacency);
 
 #endif // WATERSHED_FUNCTION_H