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
    set(PINK_ROOT "/home/felipe/dev/pink")
endif()
set(PINK_LIB ${PINK_ROOT}/lib/libpink.a)

# ====================================================================
# 3. Include Paths
# ====================================================================
include_directories(
    /home/felipe/dev/xtensor/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${PINK_ROOT}/include
)

# ====================================================================
# 4. Source Files Definition
# ====================================================================
set(COMMON_UTILS
    src/utils/ImageProcessingUtils.cpp
    src/utils/dstyle.cpp  # <-- Adicionado novo utilitário
    src/Grain.cpp
)

# ====================================================================
# 5. Executable Definitions
# ====================================================================

# --- Executable: min_tree_segmenter ---
add_executable(min_tree_segmenter
    src/minTree/min_tree_segmenter.cpp
    src/utils/ImageProcessingUtils.cpp
    src/utils/dstyle.cpp
)
target_link_libraries(min_tree_segmenter PRIVATE Threads::Threads TIFF::TIFF higra::higra)


# --- Outros Executáveis ---
# (Seus outros add_executable e target_link_libraries vêm aqui)
# Exemplo para o visualizer:
add_executable(visualizer
    src/graph/polyscope_compare_grains.cpp
    ${COMMON_UTILS}
)
target_link_libraries(visualizer PRIVATE stdc++fs polyscope::polyscope Threads::Threads)
# ... etc para todos os outros ...


# ====================================================================
# 6. Final Message
# ====================================================================
message(STATUS "CMake configuration complete.")