/**
 * @file watershed_function.cpp
 * @brief Implements the core watershed segmentation algorithm.
 */

 #include "watershed_function.h"
 #include "ImageProcessingUtils.h"
 
 #include <iostream>
 #include "xtensor/xio.hpp"
 #include "xtensor/xview.hpp"
 #include "xtensor/xoperation.hpp"
 #include "xtensor/xadapt.hpp"
 
 #include "higra/graph.hpp"
 #include "higra/component_tree.hpp"
 #include "higra/attribute.hpp"
 #include "higra/hierarchy/simplification.hpp"
 #include "higra/hierarchy/reconstruction.hpp"
 #include "higra/watershed.hpp"
 
 
 xt::xtensor<uint32_t, 3> watershed(
     const xt::xtensor<uint8_t, 3>& image,
     const xt::xtensor<uint8_t, 3>& seed,
     const xt::xtensor<uint32_t, 3>& labeled_centroids,
     int adjacency) {
 
     if (image.shape() != seed.shape()) {
         throw std::runtime_error("Error: seed resolution does not match image resolution!");
     }
 
     // --- 1. Create Higra Graph ---
     auto graph = hg::make_graph_from_implicit_graph(
         hg::get_3d_implicit_graph(image.shape(), adjacency == 26 ? hg::adjacency::cube : hg::adjacency::face)
     );
 
     // --- 2. Build and Simplify Max-Tree ---
     std::cout << "Starting max-tree filtering..." << std::endl;
     auto [tree, altitudes] = hg::component_tree_max_tree(graph, hg::xtensor_to_array_view(image));
     
     auto height = hg::attribute_height(tree, hg::xtensor_to_array_view(altitudes));
     auto area = hg::attribute_area(tree);
 
     // Filter nodes based on height and area
     auto unwanted_nodes = xt::operator||(height > 100, area < 100);
     auto [simplified_tree, node_map] = hg::simplify_tree(tree, hg::xtensor_to_array_view(unwanted_nodes));
     auto new_altitudes = hg::map_on_tree(simplified_tree, node_map, hg::xtensor_to_array_view(altitudes));
 
     // --- 3. Reconstruct Binary Image ---
     auto reconstructed_array = hg::reconstruct_leaf_data(simplified_tree, new_altitudes);
     auto binary_image_f = xt::adapt(reconstructed_array.data(), image.shape());
     
     // Binarize the result
     xt::xtensor<uint8_t, 3> binary_image = xt::cast<uint8_t>((binary_image_f - xt::amin(image)()) > 0);
 
     // --- 4. Morphological Closing ---
     binary_image = area_closing(binary_image);
 
     // --- 5. Create Gradient from Distance Transform ---
     auto edt = distance_transform_edt(binary_image);
     auto gradient = xt::amax(edt)() - edt; // Invert the distance transform
 
     // --- 6. Run Seeded Watershed ---
     auto explicit_graph = graph.as_explicit_graph();
     auto edge_weights = hg::weight_graph(explicit_graph, hg::xtensor_to_array_view(gradient), hg::WeightFunction::mean);
     
     // Use the labeled_centroids as markers
     auto labels_array = hg::labelisation_seeded_watershed(explicit_graph, edge_weights, hg::xtensor_to_array_view(labeled_centroids));
     auto labels = xt::adapt(labels_array.data(), image.shape());
 
     // --- 7. Mask final result ---
     // Where binary_image is true (part of a grain), use the watershed label, otherwise 0.
     auto result = xt::where(binary_image > 0, labels, xt::zeros<uint32_t>(labels.shape()));
 
     return result;
 }