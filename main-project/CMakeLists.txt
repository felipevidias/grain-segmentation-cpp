# ====================================================================
# 1. Project Definition and C++ Standard
# ====================================================================
cmake_minimum_required(VERSION 3.16)
project(GrainAnalysisProject LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ====================================================================
# 2. Dependencies
# ====================================================================
find_package(Threads REQUIRED)
find_package(TIFF REQUIRED)
find_package(higra REQUIRED)
find_package(polyscope REQUIRED)

if(NOT DEFINED PINK_ROOT)
    set(PINK_ROOT "/home/felipe/dev/pink") # <-- ATTENTION: Adjust this path if necessary
endif()
set(PINK_LIB ${PINK_ROOT}/lib/libpink.a)

# ====================================================================
# 3. Include Paths
# ====================================================================
include_directories(
    /home/felipe/dev/xtensor/include # <-- ATTENTION: Adjust this path if necessary
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${PINK_ROOT}/include
)

# ====================================================================
# 4. Source Files Definition
# ====================================================================
# Define a list of common utility files to avoid repetition
set(COMMON_UTILS
    src/utils/ImageProcessingUtils.cpp
    src/utils/dstyle.cpp
    src/utils/print_process_bar.cpp
    src/Grain.cpp
)

# ====================================================================
# 5. Executable Definitions
# ====================================================================

# --- Main Segmentation Pipelines ---
add_executable(watershed
    src/watershed/watershed.cpp
    src/utils/ImageProcessingUtils.cpp
)
target_link_libraries(watershed PRIVATE Threads::Threads TIFF::TIFF higra::higra ${PINK_LIB})

add_executable(successive_watershed
    src/successiveWatershed/successiveWatershed.cpp
    src/successiveWatershed/watershed_function.cpp
    src/utils/ImageProcessingUtils.cpp
)
target_link_libraries(successive_watershed PRIVATE Threads::Threads TIFF::TIFF)


# --- Analysis and Pre-processing Tools ---
add_executable(min_tree_segmenter
    src/minTree/min_tree_segmenter.cpp
    src/utils/ImageProcessingUtils.cpp
    src/utils/dstyle.cpp
)
target_link_libraries(min_tree_segmenter PRIVATE Threads::Threads TIFF::TIFF higra::higra)

add_executable(max_tree_segmenter
    src/maxTree/max_tree_segmenter.cpp
    src/utils/ImageProcessingUtils.cpp
)
target_link_libraries(max_tree_segmenter PRIVATE Threads::Threads TIFF::TIFF higra::higra ${PINK_LIB})

add_executable(get_centroid_app
    get_centroid_app.cpp
    src/utils/ImageProcessingUtils.cpp
)
target_link_libraries(get_centroid_app PRIVATE Threads::Threads TIFF::TIFF higra::higra ${PINK_LIB})

add_executable(cylinder_centroid_adder
    src/removal_utils/apply_cylinder_centroid_label.cpp
    src/utils/ImageProcessingUtils.cpp
)
target_link_libraries(cylinder_centroid_adder PRIVATE TIFF::TIFF stdc++fs)

add_executable(remove_cylinders_app
    src/removal_utils/remove_cylinders.cpp
    src/utils/ImageProcessingUtils.cpp
    src/utils/print_process_bar.cpp
)
target_link_libraries(remove_cylinders_app PRIVATE Threads::Threads TIFF::TIFF stdc++fs)

add_executable(rotate_x_axis_app
    src/transform/rotateXaxis.cpp
    src/utils/ImageProcessingUtils.cpp
)
target_link_libraries(rotate_x_axis_app PRIVATE Threads::Threads TIFF::TIFF stdc++fs)


# --- Post-processing and Visualization Tools ---
add_executable(colormap_applier
    colormap_applier.cpp
    src/utils/ImageProcessingUtils.cpp
)
target_link_libraries(colormap_applier PRIVATE Threads::Threads TIFF::TIFF)

add_executable(image_viewer
    src/viewImage/viewImage.cpp
    src/utils/ImageProcessingUtils.cpp
)
target_link_libraries(image_viewer PRIVATE Threads::Threads TIFF::TIFF polyscope::polyscope)

add_executable(visualizer
    src/graph/polyscope_compare_grains.cpp
    ${COMMON_UTILS}
)
target_link_libraries(visualizer PRIVATE stdc++fs polyscope::polyscope Threads::Threads)

add_executable(follow_grains
    polyscope_follow_grains_global.cpp
    ${COMMON_UTILS}
)
target_link_libraries(follow_grains PRIVATE stdc++fs polyscope::polyscope Threads::Threads)


# ====================================================================
# 6. Final Message
# ====================================================================
message(STATUS "CMake configuration complete. All executables are configured.")