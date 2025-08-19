 /**
 * @file viewImage.cpp
 * @brief An interactive 2D slice viewer for 3D TIFF images using Polyscope.
 *
 * This application loads a 3D TIFF image and displays its 2D slices on a plane
 * in a 3D Polyscope scene. A slider in the GUI allows for interactive
 * navigation through the image slices.
 */

#include <iostream>
#include <string>
#include <vector>

// xtensor for data handling
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"

// Polyscope for visualization
#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"
#include "polyscope/scalar_image_quantity.h"

// Project utils
#include "ImageProcessingUtils.h"

// --- Global State Variables for the Viewer ---
xt::xtensor<uint8_t, 3> image_data;
int current_slice = 0;
int total_slices = 0;
polyscope::SurfaceScalarImageQuantity* image_quantity = nullptr;

/**
 * @brief The main callback function for the ImGui user interface.
 * This function creates the slider and updates the displayed image slice.
 */
void user_callback() {
    // A persistent flag to track if the slice has changed
    static int previous_slice = -1;

    ImGui::PushItemWidth(200); // Set a nice width for the slider

    // Create a slider to control the current slice index
    if (ImGui::SliderInt("Slice", &current_slice, 0, total_slices - 1)) {
        // The slider was moved
    }

    ImGui::PopItemWidth();

    // If the slice has changed since the last frame, update the texture
    if (current_slice != previous_slice) {
        // Extract the 2D slice from the 3D data volume
        auto slice = xt::view(image_data, current_slice, xt::all(), xt::all());
        
        // Update the image data on the Polyscope quantity
        image_quantity->update_image(slice);
        
        previous_slice = current_slice;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image.tif>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];

    // --- 1. Load Image ---
    try {
        // Attempt to load as 16-bit and convert, which is a common format
        auto image_16bit = read_tiff_image_xt<uint16_t>(filepath);
        image_data = xt::cast<uint8_t>(image_16bit / 256);
    } catch (const std::exception& e) {
        try {
            // If that fails, try loading directly as 8-bit
            image_data = read_tiff_image_xt<uint8_t>(filepath);
        } catch (const std::exception& e2) {
            std::cerr << "Error: Failed to load TIFF image. " << e2.what() << std::endl;
            return 1;
        }
    }

    auto shape = image_data.shape();
    total_slices = shape[0];
    size_t img_height = shape[1];
    size_t img_width = shape[2];
    std::cout << "Loaded image with " << total_slices << " slices of size " << img_height << "x" << img_width << std::endl;


    // --- 2. Initialize Polyscope ---
    polyscope::init();

    // --- 3. Create a Plane to Display the Image On ---
    // Define the four corners of a quadrilateral (plane)
    std::vector<glm::vec3> corners = {
        {0, 0, 0},
        {(float)img_width, 0, 0},
        {(float)img_width, (float)img_height, 0},
        {0, (float)img_height, 0}
    };
    std::vector<std::vector<size_t>> faces = {{0, 1, 2, 3}};

    // Register the plane as a surface mesh
    auto* plane_mesh = polyscope::registerSurfaceMesh("Image Plane", corners, faces);

    // --- 4. Add the Image as a Texture to the Plane ---
    // Extract the first slice to display initially
    auto first_slice = xt::view(image_data, 0, xt::all(), xt::all());

    // Add the 2D slice data as a "scalar image quantity" to the mesh
    // We store the pointer to update it later in the callback
    image_quantity = plane_mesh->addScalarImageQuantity("Image Slice", first_slice, polyscope::ImageOrigin::UpperLeft);

    // --- 5. Set Viewer Options and Callback ---
    polyscope::view::resetCameraToHomeView();
    polyscope::setUserCallback(user_callback);
    polyscope::show();

    return 0;
}