/**
 * @file watershed.cpp
 * @brief Performs a seeded watershed segmentation on a 3D image.
 *
 * This program takes a 3D grayscale image and a 3D seed image as input. It first
 * binarizes the input image using a max-tree simplification. It then computes a
 * gradient based on the Euclidean Distance Transform of the binary image. Finally,
 * it uses the centroids of the seed image to perform a seeded watershed on the
 * gradient, masking the result with the binary image.
 */

 #include <iostream>
 #include <string>
 #include <vector>
 #include <cstdlib> // For std::system
 #include <filesystem>
 
 // xtensor and higra
#include "xtensor/io/xio.hpp"
 #include "xtensor/xview.hpp"
 #include "xtensor/xadapt.hpp"
 #include "xtensor/xoperation.hpp"
 #include "higra/graph.hpp"
 #include "higra/component_tree.hpp"
 #include "higra/attribute.hpp"
 #include "higra/hierarchy/simplification.hpp"
 #include "higra/hierarchy/reconstruction.hpp"
 #include "higra/watershed.hpp"
 
 // Project utils
 #include "ImageProcessingUtils.h"
 
 int main(int argc, char* argv[]) {
     if (argc != 4) {
         std::cerr << "Usage: " << argv[0] << " <image.tif> <seed.tif> <adjacency(6 or 26)>" << std::endl;
         return 1;
     }
 
     std::string image_filepath = argv[1];
     std::string seed_filepath = argv[2];
     int adjacency = std::stoi(argv[3]);
     std::string register_filepath = "results";
 
     if (!std::filesystem::exists(register_filepath)) {
         std::filesystem::create_directory(register_filepath);
     }
 
     // --- 1. Load Images ---
     auto image_16bit = read_tiff_image_xt<uint16_t>(image_filepath);
     auto seed_16bit = read_tiff_image_xt<uint16_t>(seed_filepath);
 
     if (image_16bit.shape() != seed_16bit.shape()) {
         std::cerr << "Error: Seed resolution does not match image!" << std::endl;
         return 1;
     }
 
     auto image = xt::cast<uint8_t>(image_16bit / 256);
     auto seed = xt::cast<uint8_t>(seed_16bit / 255); // Assuming seed is binary
     std::cout << "Loaded image has shape: " << image.shape()[0] << "x" << image.shape()[1] << "x" << image.shape()[2] << std::endl;
 
     // --- 2. Binarize Image with Max-Tree ---
     auto graph = hg::make_graph_from_implicit_graph(hg::get_3d_implicit_graph(image.shape(), adjacency == 26 ? hg::adjacency::cube : hg::adjacency::face));
 
     auto [tree, altitudes] = hg::component_tree_max_tree(graph, hg::xtensor_to_array_view(image));
 
     std::cout << "Starting max-tree filtering..." << std::endl;
     auto height = hg::attribute_height(tree, hg::xtensor_to_array_view(altitudes));
     auto area = hg::attribute_area(tree);
     
     auto unwanted_nodes = xt::operator||(height > 100, area < 100);
     auto [simplified_tree, node_map] = hg::simplify_tree(tree, hg::xtensor_to_array_view(unwanted_nodes));
     auto new_altitudes = hg::map_on_tree(simplified_tree, node_map, hg::xtensor_to_array_view(altitudes));
 
     auto reconstructed_array = hg::reconstruct_leaf_data(simplified_tree, new_altitudes);
     auto binary_image_f = xt::adapt(reconstructed_array.data(), image.shape());
     xt::xtensor<uint8_t, 3> binary_image = xt::cast<uint8_t>((binary_image_f - xt::amin(image)()) > 0) * 255;
 
     // --- 3. Pre-process Binary Image ---
     binary_image = area_closing(binary_image);
     std::cout << "Created binary image." << std::endl;
 
     auto edt = distance_transform_edt(binary_image);
     auto gradient = xt::amax(edt)() - edt; // Invert EDT to create watershed landscape
 
     // --- 4. Prepare Seeds ---
     int num_seed_components = 0;
     auto labeled_seed_regions = label_components(seed > 0, num_seed_components);
     auto centroids = calculate_centroids(labeled_seed_regions);
     
     xt::xtensor<uint32_t, 3> labeled_seeds = xt::zeros<uint32_t>(seed.shape());
     for (const auto& c : centroids) {
         // Cast centroid coordinates to size_t for indexing
         labeled_seeds(static_cast<size_t>(c.x), static_cast<size_t>(c.y), static_cast<size_t>(c.z)) = c.label;
     }
 
     // --- 5. Compute Watershed ---
     auto explicit_graph = graph.as_explicit_graph();
     auto edge_weights = hg::weight_graph(explicit_graph, hg::xtensor_to_array_view(gradient), hg::WeightFunction::mean);
     auto labels_array = hg::labelisation_seeded_watershed(explicit_graph, edge_weights, hg::xtensor_to_array_view(labeled_seeds));
     auto labels = xt::adapt(labels_array.data(), image.shape());
 
     // --- 6. Final Result ---
     // Where binary_image is non-zero, take the watershed label; otherwise, it's background (0).
     auto result = xt::where(binary_image > 0, labels, xt::zeros<uint32_t>(labels.shape()));
 
     // --- 7. Save and Post-process ---
     std::string result_path = "results/watershed_labeled_image.tif";
     write_tiff_image_xt(result, result_path);
     std::cout << "Watershed result saved to " << result_path << std::endl;
 
     std::string colormap_command = "./colormap_applier " + result_path;
     std::cout << "Running: " << colormap_command << std::endl;
     std::system(colormap_command.c_str());
 
     return 0;
 }