/**
 * @file Grain.cpp
 * @brief Implements the functionality of the Grain and GrainNetwork classes.
 *
 * This file contains the definitions of the methods for creating grains,
 * managing their neighbor lists, and parsing data files to build the grain network.
 */

 #include "grain.h"

 #include <iostream>
 #include <fstream>
 #include <sstream>
 #include <algorithm> // For std::find
 #include <stdexcept> // For std::out_of_range
 
 //
 // Grain struct method implementations
 //
 
 Grain::Grain(int lbl, double x_pos, double y_pos, double z_pos)
     : label(lbl), x(x_pos), y(y_pos), z(z_pos) {}
 
 void Grain::add_neighbor(Grain* neighbor) {
     // Check if the neighbor already exists to prevent duplicate entries.
     if (std::find(neighbors.begin(), neighbors.end(), neighbor) == neighbors.end()) {
         neighbors.push_back(neighbor);
     }
 }
 
 //
 // GrainNetwork class method implementations
 //
 
 void GrainNetwork::load_from_tracking_file(const std::string& filepath) {
     std::ifstream file(filepath);
     if (!file.is_open()) {
         std::cerr << "Error: Failed to open tracking file: " << filepath << std::endl;
         return;
     }
 
     std::string line;
     std::getline(file, line); // Skip the header line.
 
     int label;
     double x, y, z;
     while (std::getline(file, line)) {
         std::stringstream ss(line);
         // Expected format: Label Zpos Ypos Xpos
         ss >> label >> z >> y >> x;
 
         // Create and insert the grain directly into the map.
         grains[label] = Grain(label, x, y, z);
     }
 }
 
 void GrainNetwork::load_contacts(const std::string& filepath) {
     std::ifstream file(filepath);
     if (!file.is_open()) {
         std::cerr << "Error: Failed to open contact file: " << filepath << std::endl;
         return;
     }
 
     std::string line;
     std::getline(file, line); // Skip the header line.
 
     int label1, label2;
     while (std::getline(file, line)) {
         std::stringstream ss(line);
         ss >> label1 >> label2;
 
         try {
             // Retrieve pointers to the grains from the map.
             // Using .at() provides bounds checking and throws if a label is not found.
             Grain* grain1 = &grains.at(label1);
             Grain* grain2 = &grains.at(label2);
 
             // Establish a bidirectional neighbor relationship.
             grain1->add_neighbor(grain2);
             grain2->add_neighbor(grain1);
         } catch (const std::out_of_range& oor) {
             std::cerr << "Warning: A grain label from the contact file was not found in the tracking data. Label: "
                       << oor.what() << std::endl;
         }
     }
 }
 
 std::pair<std::vector<Grain*>, std::vector<Grain*>> GrainNetwork::get_connected_status() {
     std::vector<Grain*> connected;
     std::vector<Grain*> unconnected;
 
     // Reserve memory to avoid reallocations if the number of grains is large.
     connected.reserve(grains.size());
     unconnected.reserve(grains.size());
 
     for (auto& pair : grains) {
         Grain* current_grain = &pair.second;
         if (current_grain->neighbors.empty()) {
             unconnected.push_back(current_grain);
         } else {
             connected.push_back(current_grain);
         }
     }
     return {connected, unconnected};
 }
 
 Grain* GrainNetwork::get_grain(int label) {
     auto it = grains.find(label);
     if (it != grains.end()) {
         // Return a pointer to the grain object in the map.
         return &it->second;
     }
     // Return nullptr if the label does not exist in the map.
     return nullptr;
 }