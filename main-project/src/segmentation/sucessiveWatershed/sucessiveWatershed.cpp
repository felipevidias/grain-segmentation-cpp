/**
 * @file successiveWatershed.cpp
 * @brief Main orchestrator for the successive watershed segmentation pipeline.
 */

 #include <iostream>
 #include <string>
 #include <vector>
 #include <cstdlib>
 #include <filesystem>
 #include <iomanip> // For progress printing
 
 #include "xtensor/xio.hpp"
 #include "ImageProcessingUtils.h"
 #include "watershed_function.h"
 
 int main(int argc, char* argv[]) {
     if (argc != 3) {
         std::cerr << "Usage: " << argv[0] << " <image.tif> <adjacency(6 or 26)>" << std::endl;
         return 1;
     }
 
     const size_t crop_size = 80;
     std::string filepath = argv[1];
     std::string adjacency = argv[2];
     std::filesystem::path p(filepath);
     std::string filename = p.stem().string();
     std::string register_filepath = "results";
 
     if (!std::filesystem::exists(register_filepath)) {
         std::filesystem::create_directory(register_filepath);
     }
 
     // --- 1. Run Centroid Extraction ---
     std::string centroid_command = "./get_centroid_app " + filepath + " " + adjacency;
     std::cout << "[INFO] Running pre-processing: " << centroid_command << std::endl;
     if (std::system(centroid_command.c_str()) != 0) {
         std::cerr << "Error: Centroid extraction step failed." << std::endl; return 1;
     }
 
     // --- 2. Load Images and Centroids ---
     std::cout << "[INFO] Loading images and data..." << std::endl;
     auto image = read_tiff_image_xt<uint16_t>(filepath);
     std::string seed_filepath = register_filepath + "/" + filename + "_minTree_segment_raw.tif";
     auto seed = read_tiff_image_xt<uint32_t>(seed_filepath); 
 
     auto image_8bit = xt::cast<uint8_t>(image / 256);
     auto seed_8bit = xt::cast<uint8_t>(seed > 0);
 
     std::string csv_centroids_filename = filename + "_centroids.csv";
     std::vector<Centroid> centroids = loadCentroidsFromCSV(csv_centroids_filename);
     
     xt::xtensor<uint32_t, 3> label_of_centroids = xt::zeros<uint32_t>(seed.shape());
     for (const auto& c : centroids) {
         label_of_centroids(c.x, c.y, c.z) = c.label;
     }
 
     // --- 3. Successive Watershed Loop ---
     xt::xtensor<uint32_t, 3> final_labeled_image = xt::zeros<uint32_t>(image.shape());
     std::cout << "[INFO] Starting successive watershed on " << centroids.size() << " centroids..." << std::endl;
 
     for (size_t i = 0; i < centroids.size(); ++i) {
         const auto& c = centroids[i];
         
         // Crop all necessary images around the current centroid
         auto croped_image = cropAroundPoint(image_8bit, c.x, c.y, c.z, crop_size);
         auto croped_seed = cropAroundPoint(seed_8bit, c.x, c.y, c.z, crop_size);
         auto croped_label_of_centroids = cropAroundPoint(label_of_centroids, c.x, c.y, c.z, crop_size);
 
         // Run watershed on the small crop
         auto labeled_crop = watershed(croped_image, croped_seed, croped_label_of_centroids, std::stoi(adjacency));
 
         // Paste the result back into the final image
         positioning_label(final_labeled_image, labeled_crop, c.x, c.y, c.z, crop_size, c.label);
 
         // Print progress
         std::cout << "\r[PROGRESS] " << (i + 1) << " / " << centroids.size() << " centroids processed." << std::flush;
     }
     std::cout << std::endl; // New line after progress bar finishes
 
     // --- 4. Save and Post-process ---
     std::string result_path = register_filepath + "/successiveWatershed_labeled_image.tif";
     write_tiff_image_xt(final_labeled_image, result_path);
     std::cout << "[INFO] Final labeled image saved to " << result_path << std::endl;
 
     std::string colormap_command = "./colormap_applier " + result_path;
     std::cout << "[INFO] Running command: " << colormap_command << std::endl;
     std::system(colormap_command.c_str());
     
     // TODO: You could replace this with a call to one of your Polyscope visualizers.
     std::string view_command = "python3 viewImage/viewImage.py " + register_filepath + "/result.tif";
     std::cout << "[INFO] Running command: " << view_command << std::endl;
     std::system(view_command.c_str());
 
     return 0;
 }