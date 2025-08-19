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
# Higra and Polyscope are handled manually

# --- PINK LIBRARY DISABLED TEMPORARILY ---
# if(NOT DEFINED PINK_ROOT)
#     set(PINK_ROOT "/home/felipe/dev/pink")
# endif()
# set(PINK_LIB ${PINK_ROOT}/build/lib/libpink.a)


# ====================================================================
# 3. Source Files & Libraries Paths
# ====================================================================
set(IMAGE_PROC_UTILS src/segmentation/utils/ImageProcessingUtils.cpp)
set(GRAIN_UTILS src/graph/grain.cpp)
set(DSTYLE_UTILS src/contact_points/utils/dstyle.cpp)
set(PROGRESS_BAR_UTILS src/removal_utils/print_process_bar.cpp)
set(POLYSCOPE_TOOLS_UTILS src/graph/polyscope_tools.cpp)

set(POLYSCOPE_LIB /home/felipe/dev/polyscope/build/lib/libpolyscope.a)
set(GLFW_LIB /home/felipe/dev/polyscope/build/deps/glfw/src/libglfw3.a)

# Define paths to include directories as variables
set(XTENSOR_INCLUDE_DIR /home/felipe/dev/xtensor/include)
set(HIGRA_INCLUDE_DIR /home/felipe/dev/higra)
set(POLYSCOPE_INCLUDE_DIR /home/felipe/dev/polyscope/include)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include)


# ====================================================================
# 4. Executable Definitions
# ====================================================================

# --- Main Segmentation Pipelines ---
add_executable(watershed
    src/segmentation/watershed/watershed.cpp
    ${IMAGE_PROC_UTILS}
)
target_compile_options(watershed PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${HIGRA_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(watershed PRIVATE Threads::Threads TIFF::TIFF)

add_executable(successive_watershed
    src/segmentation/sucessiveWatershed/sucessiveWatershed.cpp
    src/segmentation/sucessiveWatershed/watershed_function.cpp
    ${IMAGE_PROC_UTILS}
)
target_compile_options(successive_watershed PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${HIGRA_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(successive_watershed PRIVATE Threads::Threads TIFF::TIFF)


# --- Analysis and Pre-processing Tools ---
add_executable(min_tree_segmenter
    src/segmentation/minTree/minTree.cpp
    ${IMAGE_PROC_UTILS}
    ${DSTYLE_UTILS}
)
target_compile_options(min_tree_segmenter PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${HIGRA_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(min_tree_segmenter PRIVATE Threads::Threads TIFF::TIFF)

add_executable(max_tree_segmenter
    src/segmentation/maxTree/maxTree.cpp
    ${IMAGE_PROC_UTILS}
)
target_compile_options(max_tree_segmenter PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${HIGRA_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(max_tree_segmenter PRIVATE Threads::Threads TIFF::TIFF)

add_executable(get_centroid_app
    src/contact_points/utils/getCentroid.cpp
    ${IMAGE_PROC_UTILS}
)
target_compile_options(get_centroid_app PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${HIGRA_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(get_centroid_app PRIVATE Threads::Threads TIFF::TIFF)

add_executable(cylinder_centroid_adder
    src/removal_utils/apply_cylinder_centroid_label.cpp
    ${IMAGE_PROC_UTILS}
)
target_compile_options(cylinder_centroid_adder PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(cylinder_centroid_adder PRIVATE TIFF::TIFF stdc++fs)

add_executable(remove_cylinders_app
    src/removal_utils/remove_cylinder.cpp
    ${IMAGE_PROC_UTILS}
    ${PROGRESS_BAR_UTILS}
)
target_compile_options(remove_cylinders_app PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(remove_cylinders_app PRIVATE Threads::Threads TIFF::TIFF stdc++fs)

add_executable(rotate_x_axis_app
    src/removal_utils/rotateXaxis.cpp
    ${IMAGE_PROC_UTILS}
)
target_compile_options(rotate_x_axis_app PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(rotate_x_axis_app PRIVATE Threads::Threads TIFF::TIFF stdc++fs)


# --- Post-processing and Visualization Tools ---
add_executable(colormap_applier
    src/segmentation/utils/colormap.cpp
    ${IMAGE_PROC_UTILS}
)
target_compile_options(colormap_applier PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(colormap_applier PRIVATE Threads::Threads TIFF::TIFF)

add_executable(image_viewer
    src/segmentation/viewImage/viewImage.cpp
    ${IMAGE_PROC_UTILS}
)
target_compile_options(image_viewer PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${POLYSCOPE_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(image_viewer PRIVATE Threads::Threads TIFF::TIFF ${POLYSCOPE_LIB} ${GLFW_LIB})

add_executable(visualizer
    src/graph/polyscope_compare_grains.cpp
    ${IMAGE_PROC_UTILS} ${GRAIN_UTILS} ${POLYSCOPE_TOOLS_UTILS}
)
target_compile_options(visualizer PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${HIGRA_INCLUDE_DIR} -I${POLYSCOPE_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(visualizer PRIVATE stdc++fs ${POLYSCOPE_LIB} ${GLFW_LIB} Threads::Threads)

add_executable(follow_grains
    src/graph/polyscope_follow_grains_with_labels.cpp
    ${IMAGE_PROC_UTILS} ${GRAIN_UTILS} ${POLYSCOPE_TOOLS_UTILS}
)
target_compile_options(follow_grains PRIVATE -I${XTENSOR_INCLUDE_DIR} -I${HIGRA_INCLUDE_DIR} -I${POLYSCOPE_INCLUDE_DIR} -I${PROJECT_INCLUDE_DIR})
target_link_libraries(follow_grains PRIVATE stdc++fs ${POLYSCOPE_LIB} ${GLFW_LIB} Threads::Threads)