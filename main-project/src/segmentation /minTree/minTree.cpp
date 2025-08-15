/**
 * @file minTree.cpp
 * @brief Core extraction from a 3D image using min-tree filtering.
 *
 * This program reads a 3D TIFF image, builds a min-tree with Higra,
 * computes attributes (area, height), simplifies the tree based on these
 * attributes, and reconstructs a binary image from the simplified tree.
 */

 #include <iostream>
 #include <string>
 #include <vector>
 #include <chrono>
 #include <filesystem>
 
 // xtensor and higra
 #include "xtensor/xio.hpp"
 #include "xtensor/xview.hpp"
 #include "xtensor/xadapt.hpp"
 #include "xtensor/xarray.hpp"
 #include "xtensor/xoperation.hpp"
 #include "higra/graph.hpp"
 #include "higra/component_tree.hpp"
 #include "higra/attribute.hpp"
 #include "higra/hierarchy/simplification.hpp"
 #include "higra/hierarchy/reconstruction.hpp"
 
 // Project utils
 #include "ImageProcessingUtils.h"
 #include "dstyle.h"
 
 int main(int argc, char* argv[]) {
     if (argc != 3) {
         std::cerr << "Usage: " << argv[0] << " <image.tif> <adjacency(6 or 26)>" << std::endl;
         return 1;
     }
 
     std::string filepath = argv[1];
     std::filesystem::path p(filepath);
     std::string filename = p.stem().string();
     int adjacency = std::stoi(argv[2]);
     std::string register_filepath = "results";
 
     if (!std::filesystem::exists(register_filepath)) {
         std::filesystem::create_directory(register_filepath);
     }
     
     auto start_time = std::chrono::high_resolution_clock::now();
     TerminalAnimator animation;
     animation.show("Processing " + filename);
 
     // --- 1. Load Image ---
     auto image_16bit = read_tiff_image_xt<uint16_t>(filepath);
     auto image = xt::cast<uint8_t>(image_16bit / 256);
 
     // --- 2. Create Higra Graph ---
     auto graph = hg::make_graph_from_implicit_graph(hg::get_3d_implicit_graph(image.shape(), adjacency == 26 ? hg::adjacency::cube : hg::adjacency::face));
 
     // --- 3. Build Min-Tree and Compute Attributes ---
     auto [tree, altitudes] = hg::component_tree_min_tree(graph, hg::xtensor_to_array_view(image));
     auto area = hg::attribute_area(tree);
     auto height = hg::attribute_height(tree, hg::xtensor_to_array_view(altitudes));
 
     // --- 4. Tree Simplification ---
     double max_height = xt::amax(height)();
     double avg_area = xt::average(area)();
     
     auto unwanted_nodes = xt::operator||(height < 0.14 * max_height, area > avg_area);
 
     auto [simplified_tree, node_map] = hg::simplify_tree(tree, hg::xtensor_to_array_view(unwanted_nodes));
     auto new_altitudes = hg::map_on_tree(simplified_tree, node_map, hg::xtensor_to_array_view(altitudes));
 
     // --- 5. Image Reconstruction ---
     auto res_array = hg::reconstruct_leaf_data(simplified_tree, new_altitudes);
     auto res_reshaped = xt::adapt(res_array.data(), image.shape());
     auto binary_res = xt::cast<uint8_t>((res_reshaped < xt::amax(res_reshaped)()) * 255);
 
     // --- 6. Save Result ---
     std::string output_path = register_filepath + "/" + filename + "_minTree_segment_raw.tif";
     write_tiff_image_xt(binary_res, output_path);
     
     auto end_time = std::chrono::high_resolution_clock::now();
     std::chrono::duration<double> elapsed = end_time - start_time;
     
     char finish_msg[200];
     sprintf(finish_msg, "\x1b[2K-- Generated %s successfully (time : %.2f s)", output_path.c_str(), elapsed.count());
     animation.succeed();
     std::cout << style::BOLD << style::GREEN << finish_msg << style::NORMAL << std::endl;
 
     return 0;
 }