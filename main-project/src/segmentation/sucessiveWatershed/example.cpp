/**
 * @file example.cpp
 * @brief Main orchestrator for the successive watershed segmentation pipeline.
 */

 #include <iostream>
 #include <string>
 #include <vector>
 #include <cstdlib>
 #include <filesystem>
 
 #include "xtensor/xio.hpp"
 #include "ImageProcessingUtils.h"
 #include "watershed_function.h"
 
 int main(int argc, char* argv[]) {
     if (argc != 3) {
         std::cerr << "Usage: " << argv[0] << " <image.tif> <adjacency(6 or 26)>" << std::endl;
         return 1;
     }
 
     std::string filepath = argv[1];
     std::string adjacency = argv[2];
     std::filesystem::path p(filepath);
     std::string filename = p.stem().string();
     std::string register_filepath = "results";
 
     if (!std::filesystem::exists(register_filepath)) {
         std::filesystem::create_directory(register_filepath);
     }
 
     // --- 1. Run Centroid Extraction ---
     // This replaces 'os.system("python3 utils/getCentroid.py ...")'
     std::string centroid_command = "./get_centroid_app " + filepath + " " + adjacency;
     std::cout << "Running command: " << centroid_command << std::endl;
     if (std::system(centroid_command.c_str()) != 0) {
         std::cerr << "Error: Centroid extraction step failed." << std::endl;
         return 1;
     }
 
     // --- 2. Load Images and Centroids ---
     auto image = read_tiff_image_xt<uint16_t>(filepath);
     
     std::string seed_filepath = register_filepath + "/" + filename + "_minTree_segment_raw.tif";
     // NOTE: minTree (called by get_centroid_app) outputs 32-bit, let's read it as such
     auto seed = read_tiff_image_xt<uint32_t>(seed_filepath); 
 
     auto image_8bit = xt::cast<uint8_t>(image / 256);
     auto seed_8bit = xt::cast<uint8_t>(seed > 0);
 
     std::string csv_centroids_filename = filename + "_centroids.csv";
     std::vector<Centroid> centroids = loadCentroidsFromCSV(csv_centroids_filename);
     
     // Create labeled centroids image
     xt::xtensor<uint32_t, 3> label_of_centroids = xt::zeros<uint32_t>(seed.shape());
     for (const auto& c : centroids) {
         // Python used (x,y,z) which mapped to (depth,height,width)
         label_of_centroids(static_cast<size_t>(c.x), static_cast<size_t>(c.y), static_cast<size_t>(c.z)) = c.label;
     }
 
     // --- 3. Run Watershed ---
     xt::xtensor<uint32_t, 3> segmented_image = watershed(image_8bit, seed_8bit, label_of_centroids, std::stoi(adjacency));
 
     // --- 4. Save and Post-process ---
     std::string result_path = register_filepath + "/result.tif";
     write_tiff_image_xt(segmented_image, result_path);
     std::cout << "Segmented image saved to " << result_path << std::endl;
 
     std::string colormap_command = "./colormap_applier " + result_path;
     std::cout << "Running command: " << colormap_command << std::endl;
     std::system(colormap_command.c_str());
     
     // The viewImage.py is a Python-specific viewer.
     // TODO: You could replace this with a call to one of your Polyscope visualizers.
     std::string view_command = "python3 viewImage/viewImage.py " + result_path;
     std::cout << "Running command: " << view_command << std::endl;
     std::system(view_command.c_str());
 
     return 0;
 }