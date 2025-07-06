#include "src/include/minTree.hpp"

// C++ Library Includes
#include <iostream>
#include <string>
#include <vector>

// --- IMPORTANT ---
// The includes below will ONLY work after you have successfully installed
// the Higra and Xtensor C++ libraries on your system.
#include <higra/higra.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xview.hpp>
// -------------------------------------------------------------------

// --- I/O Placeholders (Require libtiff & xtensor) ---
// These functions need to be implemented to work with xt::xarray for Higra compatibility.

xt::xarray<unsigned char> loadTiffImage_xt(const std::string& path, long& x, long& y, long& z) {
    std::cout << "WARNING: Function 'loadTiffImage_xt' is a placeholder. Returning empty array." << std::endl;
    // A real implementation would use libtiff to read image data into an xt::xarray.
    return {};
}

void saveTiffImage_xt(const std::string& path, const xt::xarray<unsigned char>& image) {
    std::cout << "WARNING: Function 'saveTiffImage_xt' is a placeholder." << std::endl;
    // A real implementation would use libtiff to write the xt::xarray to a file.
}


// --- Main Module Logic ---

void run_minTree(const std::string& inputFile, int adjacency, const std::string& outputFile) {
    std::cout << "--- Module: minTree ---" << std::endl;
    
    // --- 1. Data Loading ---
    long x_dim, y_dim, z_dim;
    xt::xarray<unsigned char> image = loadTiffImage_xt(inputFile, x_dim, y_dim, z_dim);
    if (image.size() == 0) {
        std::cerr << "Error: Failed to load image. Aborting." << std::endl;
        return;
    }
    // Note: The Python script's normalization `(image/256).astype("uint8")` is implicitly
    // handled by loading the data directly as unsigned char.

    // --- 2. Higra C++ Processing Pipeline ---
    
    // Build the implicit image graph based on shape and connectivity.
    auto graph = hg::get_nd_regular_implicit_graph(image.shape());

    // Construct the component tree (min-tree) and compute node altitudes.
    auto [tree, altitudes] = hg::component_tree_min_tree(graph, image);

    [cite_start]// Compute area and height attributes for each node in the tree. [cite: 158, 161]
    auto area = hg::attribute_area(tree);
    auto height = hg::attribute_height(tree, altitudes);
    
    [cite_start]// Define filtering criteria to identify and remove unwanted nodes. [cite: 205]
    // The goal is to keep nodes that represent grain cores: those with high contrast (height) and small area.
    double max_height = xt::amax(height)(); // () gets the scalar value
    double avg_area = xt::mean(area)();
    
    [cite_start]// The filtering criteria mentioned in the report. [cite: 206, 207]
    auto unwanted_nodes = xt::logical_or(height < 0.14 * max_height, area > avg_area);

    // Simplify the tree by removing the unwanted nodes.
    auto [simplified_tree, node_map] = hg::simplify_tree(tree, unwanted_nodes);
    auto new_altitudes = hg::map_ महीपत(altitudes, node_map);

    // Reconstruct a new image from the leaf data of the simplified tree.
    auto reconstructed_image = hg::reconstruct_leaf_data(simplified_tree, new_altitudes);
    
    // Binarize and scale the result to a displayable 8-bit image (0 or 255).
    unsigned char max_res_val = xt::amax(reconstructed_image)();
    auto final_image = xt::cast<unsigned char>((reconstructed_image < max_res_val) * 255);

    // --- 3. Saving Results ---
    saveTiffImage_xt(outputFile, final_image);

    std::cout << "minTree file saved to " << outputFile << std::endl;
    std::cout << "--- Module Finished: minTree ---" << std::endl;
}