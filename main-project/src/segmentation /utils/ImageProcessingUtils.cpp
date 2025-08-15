/**
 * @file ImageProcessingUtils.cpp
 * @brief Implements the utility functions for 3D image processing.
 */

 #include "ImageProcessingUtils.h"
 #include <tiffio.h>
 #include <iostream>
 #include <stdexcept>
 #include "xtensor/xadapt.hpp"
 
 // Explicit template instantiations
 template xt::xtensor<uint8_t, 3> read_tiff_image_xt<uint8_t>(const std::string&);
 template xt::xtensor<uint16_t, 3> read_tiff_image_xt<uint16_t>(const std::string&);
 template void write_tiff_image_xt<uint32_t>(const xt::xtensor<uint32_t, 3>&, const std::string&);
 template void write_tiff_image_xt<uint8_t>(const xt::xtensor<uint8_t, 3>&, const std::string&);
 
 
 template<typename T>
 xt::xtensor<T, 3> read_tiff_image_xt(const std::string& filepath) {
     TIFF* tif = TIFFOpen(filepath.c_str(), "r");
     if (!tif) {
         throw std::runtime_error("Error: Could not open TIFF file: " + filepath);
     }
 
     uint32_t width, height;
     size_t depth = 0;
     
     do {
         depth++;
     } while (TIFFReadDirectory(tif));
 
     TIFFSetDirectory(tif, 0);
     TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
     TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
 
     xt::xtensor<T, 3> image = xt::zeros<T>({depth, height, width});
 
     for (size_t d = 0; d < depth; ++d) {
         TIFFSetDirectory(tif, d);
         for (uint32_t row = 0; row < height; ++row) {
             auto row_view = xt::view(image, d, row, xt::all());
             TIFFReadScanline(tif, row_view.data(), row);
         }
     }
 
     TIFFClose(tif);
     return image;
 }
 
 template<typename T>
 void write_tiff_image_xt(const xt::xtensor<T, 3>& image, const std::string& filepath) {
     TIFF* out = TIFFOpen(filepath.c_str(), "w");
     if (!out) {
         throw std::runtime_error("Error: Could not open file for writing: " + filepath);
     }
 
     auto shape = image.shape();
     size_t depth = shape[0];
     size_t height = shape[1];
     size_t width = shape[2];
 
     for (size_t d = 0; d < depth; ++d) {
         TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
         TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
         TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
         TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, sizeof(T) * 8);
         TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
         TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
         TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
         
         for (size_t row = 0; row < height; ++row) {
             auto row_view = xt::view(image, d, row, xt::all());
             TIFFWriteScanline(out, const_cast<T*>(row_view.data()), row);
         }
         TIFFWriteDirectory(out);
     }
 
     TIFFClose(out);
 }
 
 xt::xtensor<uint8_t, 3> dilate_with_ball(const xt::xtensor<uint8_t, 3>& image, float radius) {
     // TODO: Substitua este placeholder pela chamada real da função de dilatação da biblioteca PINK.
     std::cout << "[INFO] Placeholder for PINK Dilation with ball of radius " << radius << std::endl;
     // Por enquanto, apenas retorna a imagem original
     return image;
 }
 
 xt::xtensor<uint32_t, 3> label_components(const xt::xtensor<uint8_t, 3>& image, int& num_components) {
     // TODO: Substitua este placeholder pela chamada real da função de rotulação da biblioteca PINK.
     std::cout << "[INFO] Placeholder for PINK Label Components" << std::endl;
     num_components = 5; // Valor de exemplo
     // Por enquanto, apenas converte a imagem de entrada para um formato de rótulo (uint32_t)
     return xt::cast<uint32_t>(image);
 }