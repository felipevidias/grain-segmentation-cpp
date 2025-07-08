# ====================================================================
# 1. Project Definition and C++ Standard
# ====================================================================

cmake_minimum_required(VERSION 3.16)

project(GrainAnalysisProject LANGUAGES CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ====================================================================
# 2. Dependencies (Higra, TIFF, Threads, Pink)
# ====================================================================

# Threads
find_package(Threads REQUIRED)

# TIFF for image loading
find_package(TIFF REQUIRED)

# Higra - assumes installed system-wide
find_package(higra REQUIRED)

# Pink library - manually specify path if necessary
# Set PINK_ROOT to the root of the compiled Pink project
# Example: set(PINK_ROOT "/home/felipe/dev/pink")
if(NOT DEFINED PINK_ROOT)
    set(PINK_ROOT "/home/felipe/dev/pink")  # <-- Update this if needed
endif()

# Add include directory for Pink
include_directories(${PINK_ROOT}/include)

# Add static Pink library manually
set(PINK_LIB ${PINK_ROOT}/lib/libpink.a)  # Or path to your compiled lib
if(NOT EXISTS ${PINK_LIB})
    message(FATAL_ERROR "Pink static library not found at ${PINK_LIB}")
endif()


# ====================================================================
# 3. Executable and Source Files
# ====================================================================

add_executable(grain_analysis
    main.cpp
    src/contact_points/contact_detection/common.cpp
    src/contact_points/contact_detection/contact_detection_by_extending_labels.cpp
    src/contact_points/contact_detection/contact_detection_from_label_and_skeleton.cpp
    src/contact_points/contact_detection/contact_detection_naive.cpp
    src/utils/dstyle.cpp
    src/utils/getCentroid.cpp
    src/utils/minTree.cpp
)


# ====================================================================
# 4. Include Paths
# ====================================================================

target_include_directories(grain_analysis PRIVATE
    # xtensor headers
    /home/felipe/dev/xtensor/include  # <-- Adjust if necessary

    # Your project headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/contact_points/contact_detection/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/include

    # Pink headers (already added above for clarity)
    ${PINK_ROOT}/include
)


# ====================================================================
# 5. Link Libraries
# ====================================================================

target_link_libraries(grain_analysis PRIVATE
    Threads::Threads
    TIFF::TIFF
    higra::higra
    ${PINK_LIB}
)


# ====================================================================
# 6. Final Message
# ====================================================================

message(STATUS "✅ CMake configuration complete. Run 'make' to build.")
message(STATUS "ℹ️  Using Pink from: ${PINK_ROOT}")
