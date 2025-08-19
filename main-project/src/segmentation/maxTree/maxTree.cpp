/**
 * @file max_tree_segmenter.cpp
 * @brief Performs image segmentation using a max-tree algorithm.
 *
 * This program translates the maxTree.py script. It merges a 3D grayscale image
 * with a core segmentation, builds a max-tree with Higra, and processes the tree
 * to generate a final labeled segmentation.
 */

 #include <iostream>
 #include <string>
 #include <vector>
 
 // xtensor and higra
 #include "xtensor/xio.hpp"
 #include "xtensor/xview.hpp"
 #include "xtensor/xadapt.hpp"
 #include "xtensor/xarray.hpp"
 #include "higra/graph.hpp"
 #include "higra/graph_core.hpp"
 #include "higra/component_tree.hpp"
 #include "higra/attribute.hpp"
 #include "higra/hierarchy/reconstruction.hpp"
 
 // Project utils
 #include "ImageProcessingUtils.h"
 
 int main(int argc, char* argv[]) {
     if (argc != 4) {
         std::cerr << "Usage: " << argv[0] << " <image.tif> <markers.tif> <adjacency(6 or 26)>" << std::endl;
         return 1;
     }
 
     std::string image_filepath = argv[1];
     std::string seed_filepath = argv[2];
     int adjacency = std::stoi(argv[3]);
 
     // --- 1. Load Images ---
     auto image_16bit = read_tiff_image_xt<uint16_t>(image_filepath);
     auto cores_16bit = read_tiff_image_xt<uint16_t>(seed_filepath);
     
     // Convert to 8-bit, as in the Python script
     auto image = xt::cast<uint8_t>(image_16bit / 256);
     auto cores = xt::cast<uint8_t>(cores_16bit);
     std::cout << "Loaded image has shape: " << image.shape()[0] << "x" << image.shape()[1] << "x" << image.shape()[2] << std::endl;
 
     // --- 2. Create Higra Graph ---
     auto graph = hg::make_graph_from_implicit_graph(hg::get_3d_implicit_graph(image.shape(), adjacency == 26 ? hg::adjacency::cube : hg::adjacency::face));
     
     // --- 3. Merge Image and Markers ---
     auto dilated_cores = dilate_with_ball(cores, 2.2);
 
     int num_cores = 0;
     label_components(dilated_cores > 0, num_cores); // Call just to get the count
     std::cout << "Number of cores in the image: " << num_cores << std::endl;
 
     uint8_t cores_val = xt::amax(image)() + 1;
     xt::view(image, xt::where(dilated_cores > 0)) = cores_val;
     
     // --- 4. Build Max-Tree ---
     std::cout << "Constructing max-tree..." << std::endl;
     auto [tree, altitudes] = hg::component_tree_max_tree(graph, hg::xtensor_to_array_view(image));
 
     // --- 5. Calculate Attributes and Compute Labels ---
     auto parents = tree.parents();
     size_t parents_size = parents.size();
     xt::xtensor<int, 1> labels = xt::zeros<int>({parents_size});
     xt::xtensor<int, 1> count = xt::zeros<int>({parents_size});
 
     int label_index = 1; // Start labels at 1 for background=0
 
     std::cout << "Start computing attributes..." << std::endl;
     for (auto leaf : tree.leaves()) {
         if (altitudes(leaf) == cores_val && count(tree.parent(leaf)) == 0) {
             auto node = leaf;
             while (node != tree.root()) {
                 count(node) += 1;
                 labels(node) = label_index;
                 node = tree.parent(node);
             }
             label_index++;
         }
     }
 
     // --- 6. Node Filtering ---
     std::cout << "Filtering..." << std::endl;
     for (auto node : tree.leaves_to_root_iterator()) {
         if (count(node) > 1) {
             labels(node) = 0; // Set label to 0 for merged regions
         }
     }
     
     // --- 7. Image Reconstruction and Saving ---
     auto res_array = hg::reconstruct_leaf_data(tree, hg::xtensor_to_array_view(labels));
     auto res_reshaped = xt::adapt(res_array.data(), image.shape());
     
     write_tiff_image_xt(xt::cast<uint32_t>(res_reshaped), "maxTree_result.tif");
     std::cout << "Result saved to maxTree_result.tif" << std::endl;
 
     // --- 8. Print Final Info ---
     int num_components_final = 0;
     label_components(xt::cast<uint8_t>(res_reshaped > 0), num_components_final);
     std::cout << "Number of components in the final image: " << num_components_final << std::endl;
     std::cout << "Number of labels: " << xt::amax(labels)() << std::endl;
 
     return 0;
 }