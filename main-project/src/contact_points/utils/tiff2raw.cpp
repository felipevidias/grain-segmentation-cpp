#include "src/include/tiff2raw.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdint> // For int16_t

// --- Placeholder for TIFF Loading ---
// This is the most critical part. You need a real TIFF library (like libtiff)
// to implement this function properly. It should load the image data into
// a vector of 16-bit signed integers.

std::vector<int16_t> loadTiffData_int16(const std::string& path) {
    std::cout << "AVISO: Funcao 'loadTiffData_int16' nao implementada. Retornando vetor vazio." << std::endl;
    // Exemplo de como seria com libtiff:
    // 1. Abrir o arquivo TIFF com 'TIFFOpen'.
    // 2. Ler as dimensões da imagem (largura, altura, profundidade).
    // 3. Alocar um buffer.
    // 4. Ler os dados da imagem (scanline por scanline ou em tiles) para o buffer.
    // 5. Converter o buffer para std::vector<int16_t>.
    // 6. Fechar o arquivo TIFF com 'TIFFClose'.
    return {}; // Retorna um vetor vazio para o placeholder.
}


// --- Função Principal do Módulo ---

void run_tiff_to_raw(const std::string& inputFile) {
    std::cout << "--- Modulo: tiff2raw ---" << std::endl;

    // ############ Geração do nome do arquivo de saída ############
    // Encontra a posição dos últimos 4 caracteres (".tif")
    size_t dot_pos = inputFile.rfind(".tif");
    if (dot_pos == std::string::npos) {
        std::cerr << "Erro: O arquivo de entrada nao parece ser um .tif: " << inputFile << std::endl;
        return;
    }
    std::string outputPath = inputFile.substr(0, dot_pos) + ".raw";


    // ############ Carregamento de Dados ############
    std::vector<int16_t> data = loadTiffData_int16(inputFile);
    if (data.empty()) {
        std::cerr << "Erro: Falha ao carregar dados do TIFF. Abortando." << std::endl;
        return;
    }
    std::cout << "Dados do TIFF carregados." << std::endl;


    // ############ Escrita do Arquivo Raw ############
    // Abre o arquivo de saída em modo binário
    std::ofstream outFile(outputPath, std::ios::out | std::ios::binary);
    if (!outFile) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo de saida: " << outputPath << std::endl;
        return;
    }

    // Escreve os bytes brutos do vetor para o arquivo
    // sizeof(int16_t) é o tamanho de um elemento (2 bytes)
    // data.size() é o número de elementos
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(int16_t));
    outFile.close();

    std::cout << "Arquivo RAW salvo em: " << outputPath << std::endl;
    std::cout << "--- Fim do Modulo: tiff2raw ---" << std::endl;
}