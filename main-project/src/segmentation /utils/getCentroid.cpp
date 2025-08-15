/**
 * @file getCentroid.cpp
 * @brief Extracts the centroid of each labeled grain in a 3D image.
 *
 * This program first calls the 'max_tree_segmenter' executable to perform
 * an initial segmentation. It then reads the resulting segmented image,
 * finds connected components (grains), calculates the centroid of each one,
 * and saves the results to a CSV file.
 */

 #include <iostream>
 #include <string>
 #include <vector>
 #include <cstdlib> // For std::system
 #include <filesystem>
 
 // xtensor
 #include "xtensor/xview.hpp"
 
 // Project utils
 #include "ImageProcessingUtils.h"
 
 int main(int argc, char* argv[]) {
     if (argc != 3) {
         std::cerr << "Usage: " << argv[0] << " <image.tif> <adjacency(6 or 26)>" << std::endl;
         return 1;
     }
 
     std::string filepath = argv[1];
     std::string adjacency = argv[2];
     std::filesystem::path p(filepath);
     std::string filename = p.stem().string();
     
     // --- 1. Chamar o 'max_tree_segmenter' para pré-processamento ---
     // NOTA: Isso cria uma dependência de que o executável 'max_tree_segmenter'
     // esteja compilado e no mesmo diretório ou em um local acessível pelo PATH.
     std::string command = "./max_tree_segmenter " + filepath + " " + filepath + " " + adjacency;
     std::cout << "Running pre-processing command: " << command << std::endl;
     int return_code = std::system(command.c_str());
     if (return_code != 0) {
         std::cerr << "Error: Pre-processing step failed." << std::endl;
         return 1;
     }
 
     // --- 2. Ler a imagem segmentada ---
     // O nome do arquivo de saída do max_tree_segmenter é fixo: "maxTree_result.tif"
     std::string mintree_image_path = "maxTree_result.tif";
     auto mintree_image = read_tiff_image_xt<uint32_t>(mintree_image_path);
 
     // --- 3. Encontrar e rotular componentes ---
     // A imagem já está rotulada pelo passo anterior, então podemos pular a rotulação separada.
     // Se a imagem fosse binária, chamaríamos a função `label_components` aqui.
     // xt::xtensor<uint8_t, 3> binary_image = mintree_image > 0;
     // int num_components = 0;
     // auto labeled_image = label_components(binary_image, num_components);
     auto& labeled_image = mintree_image; // Reutilizando a imagem já rotulada.
 
     // --- 4. Calcular os centroides ---
     std::vector<Centroid> centroids = calculate_centroids(labeled_image);
 
     // --- 5. Salvar os centroides em CSV ---
     std::string csv_output_path = filename + "_centroids.csv";
     write_centroids_csv(csv_output_path, centroids);
 
     return 0;
 }