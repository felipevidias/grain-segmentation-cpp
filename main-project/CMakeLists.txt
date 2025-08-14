# ====================================================================
# 1. Project Definition and C++ Standard
# ====================================================================

cmake_minimum_required(VERSION 3.16)
project(GrainAnalysisProject LANGUAGES CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ====================================================================
# 2. Dependencies
# ====================================================================

# Threads
find_package(Threads REQUIRED)

# TIFF for image loading
find_package(TIFF REQUIRED)

# Higra - assumes installed system-wide
find_package(higra REQUIRED)

# Polyscope - Adicionado para o visualizador
# Lembre-se de informar ao CMake onde encontrá-lo se não estiver em um local padrão
# Ex: cmake .. -DCMAKE_PREFIX_PATH=/path/to/polyscope/build
find_package(polyscope REQUIRED)

# Pink library - manually specify path if necessary
if(NOT DEFINED PINK_ROOT)
    set(PINK_ROOT "/home/felipe/dev/pink")  # <-- Update this if needed
endif()

set(PINK_LIB ${PINK_ROOT}/lib/libpink.a)
if(NOT EXISTS ${PINK_LIB})
    message(FATAL_ERROR "Pink static library not found at ${PINK_LIB}")
endif()

# ====================================================================
# 3. Include Paths (Globais para todos os executáveis)
# ====================================================================

include_directories(
    # xtensor headers
    /home/felipe/dev/xtensor/include  # <-- Adjust if necessary

    # Your project headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/contact_points/contact_detection/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/include

    # Pink headers
    ${PINK_ROOT}/include
)

# ====================================================================
# 4. Executable 1: grain_analysis (Ferramenta de Linha de Comando)
# ====================================================================

add_executable(grain_analysis
    main.cpp # Ponto de entrada para a análise
    src/Grain.cpp # Compartilhado com o visualizador
    src/contact_points/contact_detection/common.cpp
    src/contact_points/contact_detection/contact_detection_by_extending_labels.cpp
    src/contact_points/contact_detection/contact_detection_from_label_and_skeleton.cpp
    src/contact_points/contact_detection/contact_detection_naive.cpp
    src/utils/dstyle.cpp
    src/utils/getCentroid.cpp
    src/utils/minTree.cpp
)

target_link_libraries(grain_analysis PRIVATE
    Threads::Threads
    TIFF::TIFF
    higra::higra
    ${PINK_LIB}
)

# ====================================================================
# 5. Executable 2: visualizer (Ferramenta Gráfica com Polyscope)
# ====================================================================

add_executable(visualizer
    src/polyscope_compare_grains.cpp  # Ponto de entrada para o visualizador
    src/Grain.cpp       # Reutiliza a classe Grain
)

target_link_libraries(visualizer PRIVATE
    # A biblioteca filesystem é necessária para listar arquivos
    stdc++fs

    # Polyscope
    polyscope::polyscope
)

# ====================================================================
# 6. Final Message
# ====================================================================

message(STATUS "CMake configuration complete.")
message(STATUS "Targets available: grain_analysis, visualizer")
message(STATUS "Run 'make <target>' to build.")

# ====================================================================
# 7. Executable 3: follow_grains (Visualizador com Filtro de Labels)
# ====================================================================

add_executable(follow_grains
    polyscope_follow_grains_global.cpp # Ponto de entrada para este visualizador
    src/Grain.cpp                      # Reutiliza a classe Grain
)

target_link_libraries(follow_grains PRIVATE
    # Biblioteca filesystem para listar arquivos
    stdc++fs

    # Polyscope
    polyscope::polyscope
)